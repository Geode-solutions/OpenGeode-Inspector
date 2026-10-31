/*
 * Copyright (c) 2019 - 2026 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <geode/inspector/inspection/topology/brep_blocks_topology.hpp>

#include <optional>
#include <queue>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>
#include <absl/container/inlined_vector.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/bounding_box.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/graph_builder.hpp>
#include <geode/mesh/builder/polygonal_surface_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/graph.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>

#include <geode/model/helpers/ray_tracing.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/model_boundaries.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/relationships.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/inspection/topology/internal/expected_nb_cmvs.hpp>
#include <geode/inspector/inspection/topology/internal/topology_helpers.hpp>

namespace
{
    bool line_is_boundary_of_at_least_two_surfaces_or_one_embedding_surface(
        const geode::BRep& brep,
        const geode::uuid& line_uuid,
        const std::vector< geode::uuid >& surface_uuids )
    {
        geode::local_index_t counter{ 0 };
        for( const auto& surface_id : surface_uuids )
        {
            if( brep.Relationships::is_boundary( line_uuid, surface_id ) )
            {
                if( brep.nb_embeddings( surface_id ) > 0 )
                {
                    return true;
                }
                counter++;
                if( counter > 1 )
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool is_line_incident_to_another_block_boundary_surface(
        const geode::Line3D& line,
        const geode::BRep& brep,
        const geode::Block3D& block,
        const geode::uuid& boundary_surface_id )
    {
        for( const auto& incident_surface : brep.incidences( line ) )
        {
            if( incident_surface.id() != boundary_surface_id
                && brep.is_boundary( incident_surface, block ) )
            {
                return true;
            }
        }
        return false;
    }

    bool surface_boundaries_are_not_linked_to_another_block_boundary(
        const geode::Surface3D& surface,
        const geode::BRep& brep,
        const geode::Block3D& block )
    {
        for( const auto& line : brep.boundaries( surface ) )
        {
            if( !is_line_incident_to_another_block_boundary_surface(
                    line, brep, block, surface.id() ) )
            {
                return true;
            }
        }
        return false;
    }

    void create_graph( const geode::BRep& brep,
        geode::GenericMapping< geode::uuid, geode::index_t >&
            surface_uuids_to_graph_edges,
        geode::BijectiveMapping< geode::uuid, geode::index_t >&
            line_uuids_to_graph_vertices,
        const geode::Graph& graph,
        geode::GraphBuilder& graph_builder )
    {
        for( const auto& line : brep.lines() )
        {
            line_uuids_to_graph_vertices.map(
                line.id(), graph_builder.create_vertex() );
        }
        for( const auto& line : brep.lines() )
        {
            const auto line_vertex =
                line_uuids_to_graph_vertices.in2out( line.id() );
            for( const auto& incident_surface : brep.incidences( line ) )
            {
                for( const auto& boundary_line :
                    brep.boundaries( incident_surface ) )
                {
                    const auto boundary_line_vertex =
                        line_uuids_to_graph_vertices.in2out(
                            boundary_line.id() );
                    if( const auto existing_edge = graph.edge_from_vertices(
                            boundary_line_vertex, line_vertex ) )
                    {
                        if( surface_uuids_to_graph_edges
                                .out2in( existing_edge.value() )
                                .at( 0 )
                            == incident_surface.id() )
                        {
                            continue;
                        }
                    }
                    surface_uuids_to_graph_edges.map( incident_surface.id(),
                        graph_builder.create_edge(
                            boundary_line_vertex, line_vertex ) );
                }
            }
        }
    }

    std::vector< geode::uuid > find_not_boundary_surfaces(
        const geode::BRep& brep )
    {
        std::vector< geode::uuid > not_boundaries_surfaces;
        geode::GenericMapping< geode::uuid, geode::index_t >
            surface_uuids_to_graph_edges;
        geode::BijectiveMapping< geode::uuid, geode::index_t >
            line_uuids_to_graph_vertices;
        auto graph = geode::Graph::create();
        auto graph_builder = geode::GraphBuilder::create( *graph );
        create_graph( brep, surface_uuids_to_graph_edges,
            line_uuids_to_graph_vertices, *graph, *graph_builder );
        bool found_not_boundary_surface{ true };
        while( found_not_boundary_surface )
        {
            std::vector< bool > to_delete( graph->nb_edges(), false );
            for( const auto graph_vertex :
                geode::Range{ graph->nb_vertices() } )
            {
                if( graph->edges_around_vertex( graph_vertex ).empty() )
                {
                    continue;
                }
                const auto surface_id =
                    surface_uuids_to_graph_edges
                        .out2in( graph->edges_around_vertex( graph_vertex )
                                .at( 0 )
                                .edge_id )
                        .at( 0 );
                bool should_delete{ true };
                for( const auto& edge_around :
                    graph->edges_around_vertex( graph_vertex ) )
                {
                    if( surface_uuids_to_graph_edges
                            .out2in( edge_around.edge_id )
                            .at( 0 )
                        != surface_id )
                    {
                        should_delete = false;
                        break;
                    }
                }
                if( !should_delete )
                {
                    continue;
                }
                if( absl::c_contains( not_boundaries_surfaces, surface_id ) )
                {
                    continue;
                }
                for( const auto& edge :
                    surface_uuids_to_graph_edges.in2out( surface_id ) )
                {
                    to_delete[edge] = true;
                }
                not_boundaries_surfaces.push_back( surface_id );
            }
            if( !absl::c_contains( to_delete, true ) )
            {
                found_not_boundary_surface = false;
                continue;
            }
            const auto old2new = graph_builder->delete_edges( to_delete );
            geode::GenericMapping< geode::uuid, geode::index_t >
                new_surface_uuids_to_graph_edges;
            for( const auto& [surface_id, graph_edges] :
                surface_uuids_to_graph_edges.in2out_map() )
            {
                for( const auto graph_edge : graph_edges )
                {
                    if( old2new.at( graph_edge ) != geode::NO_ID )
                    {
                        new_surface_uuids_to_graph_edges.map(
                            surface_id, old2new.at( graph_edge ) );
                    }
                }
            }
            surface_uuids_to_graph_edges = new_surface_uuids_to_graph_edges;
            graph_builder->delete_isolated_vertices();
        }
        return not_boundaries_surfaces;
    }

    std::vector< geode::uuid > find_dangling_surfaces( const geode::BRep& brep,
        absl::Span< const geode::uuid > not_boundary_surfaces )
    {
        std::vector< geode::uuid > dangling_surfaces;
        geode::BRepRayTracing ray_tracing{ brep };
        for( const auto& surface_id : not_boundary_surfaces )
        {
            const auto& surface_mesh = brep.surface( surface_id ).mesh();
            if( surface_mesh.nb_polygons() == 0 )
            {
                continue;
            }
            // Ray tracing may fail on ambiguous rays: try a few polygons, a
            // surface which cannot be located is not reported as dangling
            constexpr geode::index_t MAX_TRIALS{ 5 };
            std::optional< bool > is_dangling;
            for( const auto polygon_id : geode::Range{
                     std::min( surface_mesh.nb_polygons(), MAX_TRIALS ) } )
            {
                try
                {
                    const auto polygon_barycenter =
                        surface_mesh.polygon_barycenter( polygon_id );
                    is_dangling = true;
                    for( const auto& block : brep.active_blocks() )
                    {
                        if( ray_tracing.is_point_inside_block(
                                polygon_barycenter, block ) )
                        {
                            is_dangling = false;
                            break;
                        }
                    }
                    break;
                }
                catch( const geode::OpenGeodeException& )
                {
                    is_dangling.reset();
                }
            }
            if( is_dangling.value_or( false ) )
            {
                dangling_surfaces.push_back( surface_id );
            }
        }
        return dangling_surfaces;
    }

    bool find_linked_processed_surface( const geode::BRep& brep,
        const geode::uuid& surface_uuid,
        absl::Span< const geode::uuid > processed )
    {
        const auto& surface = brep.surface( surface_uuid );
        for( const auto& line : brep.boundaries( surface ) )
        {
            for( const auto& other_surface : brep.incidences( line ) )
            {
                if( absl::c_contains( processed, other_surface.id() ) )
                {
                    return true;
                }
            }
        }
        return false;
    }

    std::vector< geode::uuid > closed_linked_surfaces(
        const geode::BRep& brep, std::queue< geode::uuid >& to_process )
    {
        std::vector< geode::uuid > processed;
        if( to_process.empty() )
        {
            return processed;
        }
        const auto first_surface_uuid = to_process.front();
        to_process.pop();
        processed.emplace_back( first_surface_uuid );
        geode::index_t skip_counter{ 0 };
        while( !to_process.empty() )
        {
            if( skip_counter == to_process.size() )
            {
                return processed;
            }
            const auto surface_uuid = to_process.front();
            to_process.pop();
            if( absl::c_contains( processed, surface_uuid ) )
            {
                continue;
            }
            if( find_linked_processed_surface( brep, surface_uuid, processed ) )
            {
                processed.emplace_back( surface_uuid );
                skip_counter = 0;
            }
            else
            {
                to_process.emplace( surface_uuid );
                skip_counter++;
            }
        }
        return processed;
    }

    std::queue< geode::uuid > queue_with_block_boundaries(
        const geode::BRep& brep, const geode::Block3D& block )
    {
        std::queue< geode::uuid > to_process;
        for( const auto& surface : brep.boundaries( block ) )
        {
            to_process.emplace( surface.id() );
        }
        return to_process;
    }

    std::queue< geode::uuid > queue_with_model_boundaries(
        const geode::BRep& brep )
    {
        std::queue< geode::uuid > to_process;
        for( const auto& model_boundary : brep.model_boundaries() )
        {
            for( const auto& surface :
                brep.model_boundary_items( model_boundary ) )
            {
                to_process.emplace( surface.id() );
            }
        }
        return to_process;
    }

    geode::BoundingBox3D surface_list_box(
        const geode::BRep& brep, absl::Span< const geode::uuid > surface_list )
    {
        geode::BoundingBox3D result;
        for( const auto& surface_id : surface_list )
        {
            result.add_box( brep.surface( surface_id ).mesh().bounding_box() );
        }
        return result;
    }

    std::optional< geode::index_t > enclosing_surface_list(
        const geode::BRep& brep,
        const std::vector< std::vector< geode::uuid > >& linked_surfaces_list )
    {
        std::vector< geode::BoundingBox3D > bounding_boxes;
        for( const auto& surface_list : linked_surfaces_list )
        {
            bounding_boxes.emplace_back(
                surface_list_box( brep, surface_list ) );
        }
        for( const auto first_surface_list_id :
            geode::Indices{ linked_surfaces_list } )
        {
            bool contains_others{ true };
            for( const auto second_surface_list_id :
                geode::Indices{ linked_surfaces_list } )
            {
                if( second_surface_list_id != first_surface_list_id
                    && !bounding_boxes[first_surface_list_id].contains(
                        bounding_boxes[second_surface_list_id] ) )
                {
                    contains_others = false;
                    break;
                }
            }
            if( contains_others )
            {
                return first_surface_list_id;
            }
        }
        return std::nullopt;
    }

    std::unique_ptr< geode::PolygonalSurface3D > fuse_brep_surfaces_from_list(
        const geode::BRep& brep,
        const std::vector< geode::uuid >& surface_list )
    {
        auto polygonal_surface = geode::PolygonalSurface3D::create();
        auto polygonal_surface_builder =
            geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );
        geode::BijectiveMapping< geode::index_t, geode::index_t >
            unique_vertices_to_polygonal_surface_vertices;
        for( const auto& surface_id : surface_list )
        {
            const auto& surface = brep.surface( surface_id );
            const auto& surface_mesh = surface.mesh();
            for( const auto polygon :
                geode::Range{ surface_mesh.nb_polygons() } )
            {
                auto polygon_vertices =
                    surface_mesh.polygon_vertices( polygon );
                for( auto& polygon_vertex : polygon_vertices )
                {
                    const auto unique_vertex = brep.unique_vertex(
                        { surface.component_id(), polygon_vertex } );
                    if( unique_vertex == geode::NO_ID )
                    {
                        // Unlinked vertices cannot be merged with others
                        polygon_vertex =
                            polygonal_surface_builder->create_point(
                                surface_mesh.point( polygon_vertex ) );
                        continue;
                    }
                    if( !unique_vertices_to_polygonal_surface_vertices
                            .has_mapping_input( unique_vertex ) )
                    {
                        const auto new_vertex =
                            polygonal_surface_builder->create_point(
                                surface_mesh.point( polygon_vertex ) );
                        unique_vertices_to_polygonal_surface_vertices.map(
                            unique_vertex, new_vertex );
                        polygon_vertex = new_vertex;
                    }
                    else
                    {
                        polygon_vertex =
                            unique_vertices_to_polygonal_surface_vertices
                                .in2out( unique_vertex );
                    }
                }
                polygonal_surface_builder->create_polygon( polygon_vertices );
            }
        }
        polygonal_surface_builder->compute_polygon_adjacencies();
        return polygonal_surface;
    }

    bool block_boundaries_are_closed(
        const geode::BRep& brep, const geode::Block3D& block )
    {
        auto to_process = queue_with_block_boundaries( brep, block );
        std::vector< std::vector< geode::uuid > > linked_boundary_parts;
        while( !to_process.empty() )
        {
            linked_boundary_parts.emplace_back(
                closed_linked_surfaces( brep, to_process ) );
        }
        if( linked_boundary_parts.size() < 2 )
        {
            return true;
        }
        const auto enclosing_surface_index =
            enclosing_surface_list( brep, linked_boundary_parts );
        if( !enclosing_surface_index )
        {
            return false;
        }
        const auto enclosing_surface_mesh = fuse_brep_surfaces_from_list(
            brep, linked_boundary_parts[enclosing_surface_index.value()] );
        const auto enclosing_surface_aabb =
            geode::create_aabb_tree( *enclosing_surface_mesh );
        for( const auto surface_list_id :
            geode::Indices{ linked_boundary_parts } )
        {
            if( surface_list_id == enclosing_surface_index.value() )
            {
                continue;
            }
            for( const auto& surface_id :
                linked_boundary_parts[surface_list_id] )
            {
                const auto& surface_mesh = brep.surface( surface_id ).mesh();
                for( const auto vertex_id :
                    geode::Range{ surface_mesh.nb_vertices() } )
                {
                    try
                    {
                        if( !geode::is_point_inside_closed_surface(
                                surface_mesh.point( vertex_id ),
                                *enclosing_surface_mesh,
                                enclosing_surface_aabb ) )
                        {
                            return false;
                        }
                    }
                    catch( const geode::OpenGeodeException& )
                    {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    bool brep_boundaries_are_closed( const geode::BRep& brep )
    {
        auto to_process = queue_with_model_boundaries( brep );
        if( to_process.empty() )
        {
            // No boundary surface to check
            return true;
        }
        auto compute_closed_linked_surfaces =
            closed_linked_surfaces( brep, to_process );
        return to_process.empty();
    }

    absl::flat_hash_set< geode::uuid > block_boundary_lines(
        const geode::BRep& brep, const geode::Block3D& block )
    {
        absl::flat_hash_set< geode::uuid > lines;
        for( const auto& surface : brep.boundaries( block ) )
        {
            for( const auto& line : brep.boundaries( surface ) )
            {
                lines.insert( line.id() );
            }
            for( const auto& line : brep.internal_lines( surface ) )
            {
                lines.insert( line.id() );
            }
        }
        return lines;
    }

    std::pair< geode::index_t, geode::index_t > sorted_unique_vertices(
        const geode::BRep& brep,
        const geode::ComponentID& component_id,
        const std::array< geode::index_t, 2 >& vertices )
    {
        const auto first = brep.unique_vertex( { component_id, vertices[0] } );
        const auto second = brep.unique_vertex( { component_id, vertices[1] } );
        return { std::min( first, second ), std::max( first, second ) };
    }

    bool surface_has_border_outside_its_lines(
        const geode::BRep& brep, const geode::Surface3D& surface )
    {
        const auto& mesh = surface.mesh();
        if( mesh.nb_polygons() == 0 )
        {
            return true;
        }
        absl::flat_hash_set< std::pair< geode::index_t, geode::index_t > >
            line_edges;
        const auto add_line_edges = [&brep, &line_edges](
                                        const geode::Line3D& line ) {
            const auto& line_mesh = line.mesh();
            for( const auto edge : geode::Range{ line_mesh.nb_edges() } )
            {
                line_edges.insert( sorted_unique_vertices( brep,
                    line.component_id(), line_mesh.edge_vertices( edge ) ) );
            }
        };
        for( const auto& line : brep.boundaries( surface ) )
        {
            add_line_edges( line );
        }
        for( const auto& line : brep.internal_lines( surface ) )
        {
            add_line_edges( line );
        }
        for( const auto polygon : geode::Range{ mesh.nb_polygons() } )
        {
            for( const auto edge :
                geode::LRange{ mesh.nb_polygon_edges( polygon ) } )
            {
                const geode::PolygonEdge polygon_edge{ polygon, edge };
                if( !mesh.is_edge_on_border( polygon_edge ) )
                {
                    continue;
                }
                const auto edge_unique_vertices =
                    sorted_unique_vertices( brep, surface.component_id(),
                        mesh.polygon_edge_vertices( polygon_edge ) );
                if( edge_unique_vertices.second == geode::NO_ID
                    || !line_edges.contains( edge_unique_vertices ) )
                {
                    return true;
                }
            }
        }
        return false;
    }

    std::vector< geode::uuid > internal_surfaces_without_free_border(
        const geode::BRep& brep,
        const geode::Block3D& block,
        const absl::flat_hash_set< geode::uuid >& block_lines )
    {
        std::vector< geode::uuid > surfaces;
        for( const auto& surface : brep.internal_surfaces( block ) )
        {
            if( surface.is_active()
                && !surface_has_border_outside_its_lines( brep, surface ) )
            {
                surfaces.push_back( surface.id() );
            }
        }
        while( true )
        {
            absl::flat_hash_map< geode::uuid, geode::index_t >
                nb_surfaces_around_line;
            for( const auto& surface_id : surfaces )
            {
                for( const auto& line :
                    brep.boundaries( brep.surface( surface_id ) ) )
                {
                    nb_surfaces_around_line[line.id()]++;
                }
            }
            const auto has_free_line = [&]( const geode::uuid& surface_id ) {
                for( const auto& line :
                    brep.boundaries( brep.surface( surface_id ) ) )
                {
                    if( !block_lines.contains( line.id() )
                        && nb_surfaces_around_line.at( line.id() ) == 1 )
                    {
                        return true;
                    }
                }
                return false;
            };
            const auto nb_surfaces = surfaces.size();
            surfaces.erase( std::remove_if( surfaces.begin(), surfaces.end(),
                                has_free_line ),
                surfaces.end() );
            if( surfaces.size() == nb_surfaces )
            {
                return surfaces;
            }
        }
    }

    geode::index_t nb_block_boundary_parts( const geode::BRep& brep,
        const geode::Block3D& block,
        const absl::flat_hash_set< geode::uuid >& cut_lines )
    {
        absl::flat_hash_set< geode::uuid > boundary_surfaces;
        for( const auto& surface : brep.boundaries( block ) )
        {
            if( surface.is_active() )
            {
                boundary_surfaces.insert( surface.id() );
            }
        }
        absl::flat_hash_set< geode::uuid > visited;
        geode::index_t nb_parts{ 0 };
        for( const auto& surface_id : boundary_surfaces )
        {
            if( !visited.insert( surface_id ).second )
            {
                continue;
            }
            nb_parts++;
            std::queue< geode::uuid > to_visit;
            to_visit.push( surface_id );
            while( !to_visit.empty() )
            {
                const auto& surface = brep.surface( to_visit.front() );
                to_visit.pop();
                for( const auto& line : brep.boundaries( surface ) )
                {
                    if( cut_lines.contains( line.id() ) )
                    {
                        continue;
                    }
                    absl::InlinedVector< geode::uuid, 2 > line_surfaces;
                    for( const auto& other_surface : brep.incidences( line ) )
                    {
                        if( boundary_surfaces.contains( other_surface.id() ) )
                        {
                            line_surfaces.push_back( other_surface.id() );
                        }
                    }
                    if( line_surfaces.size() != 2 )
                    {
                        continue;
                    }
                    for( const auto& line_surface_id : line_surfaces )
                    {
                        if( visited.insert( line_surface_id ).second )
                        {
                            to_visit.push( line_surface_id );
                        }
                    }
                }
            }
        }
        return nb_parts;
    }

    bool surfaces_have_closed_part( const geode::BRep& brep,
        absl::Span< const geode::uuid > surfaces,
        const absl::flat_hash_set< geode::uuid >& block_lines )
    {
        const absl::flat_hash_set< geode::uuid > surface_set{ surfaces.begin(),
            surfaces.end() };
        absl::flat_hash_set< geode::uuid > visited;
        for( const auto& surface_id : surfaces )
        {
            if( !visited.insert( surface_id ).second )
            {
                continue;
            }
            bool touches_block_boundary{ false };
            std::queue< geode::uuid > to_visit;
            to_visit.push( surface_id );
            while( !to_visit.empty() )
            {
                const auto& surface = brep.surface( to_visit.front() );
                to_visit.pop();
                for( const auto& line : brep.boundaries( surface ) )
                {
                    if( block_lines.contains( line.id() ) )
                    {
                        touches_block_boundary = true;
                        continue;
                    }
                    for( const auto& other_surface : brep.incidences( line ) )
                    {
                        if( surface_set.contains( other_surface.id() )
                            && visited.insert( other_surface.id() ).second )
                        {
                            to_visit.push( other_surface.id() );
                        }
                    }
                }
            }
            if( !touches_block_boundary )
            {
                return true;
            }
        }
        return false;
    }

    std::vector< geode::uuid > internal_surfaces_splitting_block(
        const geode::BRep& brep, const geode::Block3D& block )
    {
        if( brep.nb_internal_surfaces( block ) == 0 )
        {
            return {};
        }
        const auto block_lines = block_boundary_lines( brep, block );
        const auto surfaces =
            internal_surfaces_without_free_border( brep, block, block_lines );
        if( surfaces.empty() )
        {
            return {};
        }
        absl::flat_hash_set< geode::uuid > cut_lines;
        for( const auto& surface_id : surfaces )
        {
            for( const auto& line :
                brep.boundaries( brep.surface( surface_id ) ) )
            {
                if( block_lines.contains( line.id() ) )
                {
                    cut_lines.insert( line.id() );
                }
            }
        }
        if( nb_block_boundary_parts( brep, block, cut_lines )
                > nb_block_boundary_parts( brep, block, {} )
            || surfaces_have_closed_part( brep, surfaces, block_lines ) )
        {
            return surfaces;
        }
        return {};
    }
} // namespace

namespace geode
{
    index_t BRepBlocksTopologyInspectionResult::nb_issues() const
    {
        return some_blocks_not_meshed.nb_issues()
               + wrong_block_boundary_surface.nb_issues()
               + blocks_not_linked_to_a_unique_vertex.nb_issues()
               + blocks_with_not_closed_boundary_surfaces.nb_issues()
               + blocks_split_by_internal_surfaces.nb_issues()
               + model_boundaries_dont_form_a_closed_surface.nb_issues()
               + unique_vertices_part_of_two_blocks_and_no_boundary_surface
                     .nb_issues()
               + unique_vertices_with_incorrect_block_cmvs_count.nb_issues()
               + unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks
                     .nb_issues()
               + unique_vertices_linked_to_a_single_and_invalid_surface
                     .nb_issues()
               + unique_vertex_linked_to_multiple_invalid_surfaces.nb_issues();
    }

    std::string BRepBlocksTopologyInspectionResult::string() const
    {
        std::string message;
        if( some_blocks_not_meshed.nb_issues() != 0 )
        {
            absl::StrAppend( &message, some_blocks_not_meshed.string(), "\n" );
        }
        if( wrong_block_boundary_surface.nb_issues() != 0 )
        {
            absl::StrAppend( &message, wrong_block_boundary_surface.string() );
        }
        if( blocks_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, blocks_not_linked_to_a_unique_vertex.string() );
        }
        if( blocks_with_not_closed_boundary_surfaces.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, blocks_with_not_closed_boundary_surfaces.string() );
        }
        if( blocks_split_by_internal_surfaces.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, blocks_split_by_internal_surfaces.string() );
        }
        if( model_boundaries_dont_form_a_closed_surface.nb_issues() != 0 )
        {
            absl::StrAppend( &message,
                model_boundaries_dont_form_a_closed_surface.string() );
        }
        if( unique_vertices_part_of_two_blocks_and_no_boundary_surface
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_part_of_two_blocks_and_no_boundary_surface
                    .string(),
                "\n" );
        }
        if( unique_vertices_with_incorrect_block_cmvs_count.nb_issues() != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_with_incorrect_block_cmvs_count.string(),
                "\n" );
        }
        if( unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks
                    .string(),
                "\n" );
        }
        if( unique_vertices_linked_to_a_single_and_invalid_surface.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_single_and_invalid_surface.string(),
                "\n" );
        }
        if( unique_vertex_linked_to_multiple_invalid_surfaces.nb_issues() != 0 )
        {
            absl::StrAppend( &message,
                unique_vertex_linked_to_multiple_invalid_surfaces.string(),
                "\n" );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "no issues with Blocks topology \n";
    }

    std::string BRepBlocksTopologyInspectionResult::inspection_type() const
    {
        return "Blocks topology inspection";
    }

    BRepBlocksTopology::BRepBlocksTopology( const BRep& brep ) : brep_( brep )
    {
    }

    bool BRepBlocksTopology::brep_blocks_topology_is_valid(
        index_t unique_vertex_index ) const
    {
        return !( unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(
                      unique_vertex_index )
                  || unique_vertex_block_cmvs_count_is_incorrect(
                      unique_vertex_index ) );
    }

    bool BRepBlocksTopology::block_is_meshed( const Block3D& block ) const
    {
        const auto& block_mesh = block.mesh();
        return block_mesh.nb_vertices() != 0 && block_mesh.nb_polyhedra() != 0;
    }

    bool BRepBlocksTopology::block_vertices_are_associated_to_unique_vertices(
        const Block3D& block ) const
    {
        return internal::
            model_component_vertices_are_associated_to_unique_vertices(
                brep_, block.component_id(), block.mesh() );
    }

    std::optional< std::string > BRepBlocksTopology::
        unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(
            index_t unique_vertex_index ) const
    {
        const auto block_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Block3D::component_type_static() );
        if( block_uuids.size() != 2 )
        {
            return std::nullopt;
        }
        for( const auto& block_uuid : block_uuids )
        {
            if( !brep_.block( block_uuid ).is_active() )
            {
                return std::nullopt;
            }
        }
        for( const auto& surface_cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( surface_cmv.component_id.type
                != Surface3D::component_type_static() )
            {
                continue;
            }
            if( brep_.Relationships::is_boundary(
                    surface_cmv.component_id.id, block_uuids[0] )
                && brep_.Relationships::is_boundary(
                    surface_cmv.component_id.id, block_uuids[1] ) )
            {
                return std::nullopt;
            }
            for( const auto& line_cmv :
                brep_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( line_cmv.component_id.type
                    != Line3D::component_type_static() )
                {
                    continue;
                }
                if( brep_.Relationships::is_boundary(
                        line_cmv.component_id.id, surface_cmv.component_id.id )
                    && ( brep_.Relationships::is_boundary(
                             surface_cmv.component_id.id, block_uuids[0] )
                         || brep_.Relationships::is_boundary(
                             surface_cmv.component_id.id, block_uuids[1] ) ) )
                {
                    return std::nullopt;
                }
            }
        }
        return absl::StrCat( "unique vertex ", unique_vertex_index,
            " is part of two Blocks, but not of a Surface boundary to the "
            "two Blocks, nor of a Line boundary to one of the Blocks incident "
            "Surfaces." );
    }

    std::optional< std::string >
        BRepBlocksTopology::unique_vertex_block_cmvs_count_is_incorrect(
            index_t unique_vertex_index ) const
    {
        const auto block_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Block3D::component_type_static() );
        const auto component_cmvs =
            internal::vertex_cmvs_by_component( brep_, unique_vertex_index );
        for( const auto& block_uuid : block_uuids )
        {
            if( !brep_.block( block_uuid ).is_active() )
            {
                continue;
            }
            if( auto error_message = internal::wrong_nb_expected_block_cmvs(
                    brep_, unique_vertex_index, block_uuid, component_cmvs ) )
            {
                return error_message;
            }
        }
        return std::nullopt;
    }

    std::optional< std::string > BRepBlocksTopology::
        vertex_is_part_of_surface_with_wrong_relationships_to_block(
            index_t unique_vertex_index,
            absl::Span< const uuid > not_boundary_surfaces,
            absl::Span< const uuid > dangling_surface ) const
    {
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type != Surface3D::component_type_static()
                || !brep_.surface( cmv.component_id.id ).is_active() )
            {
                continue;
            }
            if( brep_.nb_incidences( cmv.component_id.id ) >= 1
                && absl::c_contains(
                    not_boundary_surfaces, cmv.component_id.id ) )
            {
                return absl::StrCat( "unique vertex with index ",
                    unique_vertex_index, " is part of Surface ",
                    brep_.surface( cmv.component_id.id )
                        .name()
                        .value_or( cmv.component_id.id.string() ),
                    " (", cmv.component_id.id.string(),
                    "), which should not be boundary of any Block." );
            }
            if( brep_.nb_embeddings( cmv.component_id.id ) >= 1
                && brep_.nb_incidences( cmv.component_id.id ) < 1
                && !absl::c_contains(
                    not_boundary_surfaces, cmv.component_id.id )
                && absl::c_contains( dangling_surface, cmv.component_id.id ) )
            {
                return absl::StrCat( "unique vertex ", unique_vertex_index,
                    " is part of Surface ",
                    brep_.surface( cmv.component_id.id )
                        .name()
                        .value_or( cmv.component_id.id.string() ),
                    " (", cmv.component_id.id.string(),
                    "), which should not be embedded in any Block." );
            }
            if( brep_.nb_incidences( cmv.component_id.id ) < 1
                && brep_.nb_embeddings( cmv.component_id.id ) < 1
                && !absl::c_contains( dangling_surface, cmv.component_id.id ) )
            {
                return absl::StrCat( "unique vertex ", unique_vertex_index,
                    " is part of Surface ",
                    brep_.surface( cmv.component_id.id )
                        .name()
                        .value_or( cmv.component_id.id.string() ),
                    " (", cmv.component_id.id.string(),
                    "), which is not internal to "
                    "a Block while it should be." );
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepBlocksTopology::vertex_is_part_of_invalid_single_surface(
            index_t unique_vertex_index,
            absl::Span< const uuid > not_boundary_surfaces ) const
    {
        const auto surface_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Surface3D::component_type_static() );
        if( surface_uuids.size() != 1
            || !brep_.surface( surface_uuids[0] ).is_active() )
        {
            return std::nullopt;
        }
        const auto& surface_id = surface_uuids[0];
        const auto block_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Block3D::component_type_static() );
        if( absl::c_contains( not_boundary_surfaces, surface_id ) )
        {
            return std::nullopt;
        }
        if( block_uuids.size() > 2 )
        {
            return absl::StrCat( "unique vertex ", unique_vertex_index,
                " is part of only one Surface, but is part of more "
                "than two Blocks." );
        }
        if( brep_.nb_embeddings( surface_id ) > 0 )
        {
            if( internal::brep_blocks_are_meshed( brep_ ) )
            {
                if( block_uuids.size() != 1 )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of only one Surface, which is "
                        "embedded, but not part of only one Block." );
                }
                else if( !brep_.Relationships::is_internal(
                             surface_id, block_uuids[0] ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of only one Surface, which is "
                        "embedded, and one Block, but the Surface is "
                        "not internal to the Block." );
                }
            }
        }
        else
        {
            for( const auto& block_id : block_uuids )
            {
                if( !brep_.Relationships::is_boundary( surface_id, block_id ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of only one Surface",
                        brep_.surface( surface_id )
                            .name()
                            .value_or( surface_id.string() ),
                        " (", surface_id.string(),
                        ") which is not embedded, but not boundary "
                        "either of Block ",
                        brep_.block( block_id )
                            .name()
                            .value_or( block_id.string() ),
                        " (", block_id.string(), "), in which the vertex is." );
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepBlocksTopology::vertex_is_part_of_invalid_multiple_surfaces(
            index_t unique_vertex_index ) const
    {
        const auto line_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Line3D::component_type_static() );
        if( line_uuids.size() < 2 )
        {
            return std::nullopt;
        }
        const auto surface_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Surface3D::component_type_static() );
        if( surface_uuids.size() < 2 )
        {
            return std::nullopt;
        }
        std::vector< uuid > not_dangling_surfaces;
        for( const auto& surface_uuid : surface_uuids )
        {
            if( brep_.nb_incidences( surface_uuid ) < 1
                && brep_.nb_embeddings( surface_uuid ) < 1 )
            {
                continue;
            }
            not_dangling_surfaces.push_back( surface_uuid );
        }
        if( not_dangling_surfaces.size() < 2 )
        {
            return std::nullopt;
        }
        for( const auto& line_id : line_uuids )
        {
            bool line_has_relations_with_all_surfaces{ true };
            for( const auto& surface_id : not_dangling_surfaces )
            {
                if( !brep_.Relationships::is_internal( line_id, surface_id )
                    && !brep_.Relationships::is_boundary(
                        line_id, surface_id ) )
                {
                    line_has_relations_with_all_surfaces = false;
                    break;
                }
            }
            if( brep_.nb_embeddings( line_id ) < 1
                && !line_is_boundary_of_at_least_two_surfaces_or_one_embedding_surface(
                    brep_, line_id, not_dangling_surfaces )
                && line_has_relations_with_all_surfaces )
            {
                const auto cmv =
                    brep_.component_mesh_vertices( unique_vertex_index )[0];
                return absl::StrCat( "unique vertex ", unique_vertex_index,
                    " is part of multiple Surfaces and multiple "
                    "Lines, but Line ",
                    brep_.line( line_id ).name().value_or( line_id.string() ),
                    " (", line_id.string(),
                    ") is neither internal, nor a boundary of at "
                    "least two Surfaces or one embedding "
                    "Surface." );
            }
        }
        return std::nullopt;
    }

    BRepBlocksTopologyInspectionResult
        BRepBlocksTopology::inspect_blocks() const
    {
        BRepBlocksTopologyInspectionResult result;
        if( brep_.nb_active_blocks() == 0 )
        {
            return result;
        }
        const auto not_boundary_surfaces = find_not_boundary_surfaces( brep_ );
        const auto dangling_surfaces =
            find_dangling_surfaces( brep_, not_boundary_surfaces );
        std::vector< geode::uuid > blocks_not_meshed;
        std::vector< geode::uuid > meshed_blocks;
        for( const auto& block : brep_.active_blocks() )
        {
            if( !block_is_meshed( brep_.block( block.id() ) ) )
            {
                blocks_not_meshed.push_back( block.id() );
                continue;
            }
            meshed_blocks.push_back( block.id() );
        }
        if( !blocks_not_meshed.empty() && !meshed_blocks.empty() )
        {
            for( const auto& block_id : blocks_not_meshed )
            {
                result.some_blocks_not_meshed.add_issue( block_id,
                    absl::StrCat( "Block ",
                        brep_.block( block_id )
                            .name()
                            .value_or( block_id.string() ),
                        " (", block_id.string(), ") is not meshed." ) );
            }
        }
        for( const auto& meshed_block_id : meshed_blocks )
        {
            const auto& block = brep_.block( meshed_block_id );
            auto block_result = internal::
                model_component_vertices_not_associated_to_unique_vertices(
                    brep_, block.component_id(), block.mesh() );
            if( block_result.nb_issues() != 0 )
            {
                block_result.set_description( absl::StrCat( "Block ",
                    brep_.block( meshed_block_id )
                        .name()
                        .value_or( meshed_block_id.string() ),
                    " (", meshed_block_id.string(), ")" ) );
                result.blocks_not_linked_to_a_unique_vertex.add_issues_to_map(
                    block.id(), std::move( block_result ) );
            }
        }
        if( brep_.nb_model_boundaries() != 0 )
        {
            if( !brep_boundaries_are_closed( brep_ ) )
            {
                result.model_boundaries_dont_form_a_closed_surface.add_issue(
                    0, "ModelBoundaries don't form a valid closed surface." );
            }
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            internal::add_unique_vertex_issue(
                result
                    .unique_vertices_part_of_two_blocks_and_no_boundary_surface,
                unique_vertex_id, [&] {
                    return unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(
                        unique_vertex_id );
                } );
            internal::add_unique_vertex_issue(
                result.unique_vertices_with_incorrect_block_cmvs_count,
                unique_vertex_id, [&] {
                    return unique_vertex_block_cmvs_count_is_incorrect(
                        unique_vertex_id );
                } );
            internal::add_unique_vertex_issue(
                result.unique_vertices_linked_to_a_single_and_invalid_surface,
                unique_vertex_id, [&] {
                    return vertex_is_part_of_invalid_single_surface(
                        unique_vertex_id, not_boundary_surfaces );
                } );
            internal::add_unique_vertex_issue(
                result
                    .unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks,
                unique_vertex_id, [&] {
                    return vertex_is_part_of_surface_with_wrong_relationships_to_block(
                        unique_vertex_id, not_boundary_surfaces,
                        dangling_surfaces );
                } );
            internal::add_unique_vertex_issue(
                result.unique_vertex_linked_to_multiple_invalid_surfaces,
                unique_vertex_id, [&] {
                    return vertex_is_part_of_invalid_multiple_surfaces(
                        unique_vertex_id );
                } );
        }
        for( const auto& block : brep_.active_blocks() )
        {
            if( !block_boundaries_are_closed( brep_, block ) )
            {
                result.blocks_with_not_closed_boundary_surfaces.add_issue(
                    block.id(),
                    absl::StrCat( "Block ",
                        block.name().value_or( block.id().string() ), " (",
                        block.id().string(),
                        ") boundaries are not closed or form several closed "
                        "volumes." ) );
            }
            const auto splitting_surfaces =
                internal_surfaces_splitting_block( brep_, block );
            if( !splitting_surfaces.empty() )
            {
                std::string message = absl::StrCat( "Block ",
                    block.name().value_or( block.id().string() ), " (",
                    block.id().string(),
                    ") is split into several volumes by its internal "
                    "Surfaces" );
                for( const auto& surface_id : splitting_surfaces )
                {
                    absl::StrAppend( &message, " ",
                        brep_.surface( surface_id )
                            .name()
                            .value_or( surface_id.string() ),
                        " (", surface_id.string(), ")" );
                }
                result.blocks_split_by_internal_surfaces.add_issue(
                    block.id(), absl::StrCat( message, "." ) );
            }
            for( const auto& surface : brep_.boundaries( block ) )
            {
                if( !surface.is_active() )
                {
                    continue;
                }
                if( surface_boundaries_are_not_linked_to_another_block_boundary(
                        surface, brep_, block ) )
                {
                    result.wrong_block_boundary_surface.add_issue( block.id(),
                        absl::StrCat( "Block ",
                            block.name().value_or( block.id().string() ), " (",
                            block.id().string(), ") boundary Surface ",
                            surface.name().value_or( surface.id().string() ),
                            " (", surface.id().string(),
                            ") has a boundary Line not incident to any "
                            "other Block boundary Surface. Either the surface "
                            "shouldn't be boundary, or there is a hole in the "
                            "block boundaries" ) );
                    break;
                }
            }
        }
        return result;
    }
} // namespace geode
