/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/inspector/topology/brep_blocks_topology.hpp>

#include <optional>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/graph_builder.hpp>
#include <geode/mesh/builder/polygonal_surface_builder.hpp>
#include <geode/mesh/builder/vertex_set_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/graph.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>

#include <geode/model/helpers/component_mesh_edges.hpp>
#include <geode/model/helpers/ray_tracing.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/model_boundaries.hpp>
#include <geode/model/mixin/core/model_boundary.hpp>
#include <geode/model/mixin/core/relationships.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/topology/internal/topology_helpers.hpp>

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

    std::vector< geode::uuid > block_boundary_surfaces(
        const geode::BRep& brep, const geode::Block3D& block )
    {
        std::vector< geode::uuid > block_boundary_uuids;
        block_boundary_uuids.reserve( brep.nb_boundaries( block.id() ) );
        for( const auto& boundary_surface : brep.boundaries( block ) )
        {
            block_boundary_uuids.push_back( boundary_surface.id() );
        }
        return block_boundary_uuids;
    }

    bool is_line_incident_to_another_block_boundary_surface(
        const geode::Line3D& line,
        const geode::BRep& brep,
        absl::Span< const geode::uuid > block_boundary_uuids,
        const geode::uuid& boundary_surface_id )
    {
        for( const auto& incident_surface : brep.incidences( line ) )
        {
            if( incident_surface.id() == boundary_surface_id )
            {
                continue;
            }
            if( absl::c_find( block_boundary_uuids, incident_surface.id() )
                != block_boundary_uuids.end() )
            {
                return true;
            }
        }
        return false;
    }

    bool surface_should_not_be_boundary_to_block( const geode::uuid& bsurf_uuid,
        const geode::BRep& brep,
        const std::vector< geode::uuid >& block_boundary_uuids )
    {
        const auto& surface = brep.surface( bsurf_uuid );
        for( const auto& line : brep.boundaries( surface ) )
        {
            if( is_line_incident_to_another_block_boundary_surface(
                    line, brep, block_boundary_uuids, bsurf_uuid ) )
            {
                continue;
            }
            return true;
        }
        return false;
    }

    template < typename Condition >
    geode::index_t count_cmvs(
        absl::Span< const geode::ComponentMeshVertex > cmvs,
        const Condition& condition )
    {
        geode::index_t counter{ 0 };
        for( const auto& cmv : cmvs )
        {
            if( condition( cmv ) )
            {
                counter++;
            }
        }
        return counter;
    }

    std::vector< geode::uuid > find_not_boundary_surfaces(
        const geode::BRep& brep )
    {
        std::vector< geode::uuid > not_boundaries_surfaces;
        auto graph = geode::Graph::create();
        auto graph_builder = geode::GraphBuilder::create( *graph );
        geode::GenericMapping< geode::uuid, geode::index_t >
            surface_uuids_to_graph_edges;
        geode::BijectiveMapping< geode::uuid, geode::index_t >
            line_uuids_to_graph_vertices;
        for( const auto& line : brep.lines() )
        {
            line_uuids_to_graph_vertices.map(
                line.id(), graph_builder->create_vertex() );
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
                    if( const auto existing_edge = graph->edge_from_vertices(
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
                        graph_builder->create_edge(
                            boundary_line_vertex, line_vertex ) );
                }
            }
        }
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

    bool verify_blocks_boundaries(
        const geode::BRep& brep, const geode::Block3D& block )
    {
        auto polygonal_surface = geode::PolygonalSurface3D::create();
        auto polygonal_surface_builder =
            geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );
        polygonal_surface->enable_edges();
        geode::BijectiveMapping< geode::index_t, geode::index_t >
            unique_vertices_to_polygonal_surface_vertices;
        for( const auto& boundary : brep.boundaries( block ) )
        {
            const auto& surface_mesh = boundary.mesh();
            for( const auto polygon :
                geode::Range{ surface_mesh.nb_polygons() } )
            {
                auto polygon_vertices =
                    surface_mesh.polygon_vertices( polygon );
                for( auto& polygon_vertex : polygon_vertices )
                {
                    const auto unique_vertex = brep.unique_vertex(
                        { boundary.component_id(), polygon_vertex } );
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
        return polygonal_surface->nb_vertices()
                   - polygonal_surface->edges().nb_edges()
                   + polygonal_surface->nb_polygons()
               == 2;
    }

    bool are_brep_boundaries_closed( const geode::BRep& brep )
    {
        auto polygonal_surface = geode::PolygonalSurface3D::create();
        auto polygonal_surface_builder =
            geode::PolygonalSurfaceBuilder3D::create( *polygonal_surface );
        polygonal_surface->enable_edges();
        geode::BijectiveMapping< geode::index_t, geode::index_t >
            unique_vertices_to_polygonal_surface_vertices;
        for( const auto& model_boundary : brep.model_boundaries() )
        {
            for( const auto& boundary :
                brep.model_boundary_items( model_boundary ) )
            {
                const auto& surface_mesh = boundary.mesh();
                for( const auto polygon :
                    geode::Range{ surface_mesh.nb_polygons() } )
                {
                    auto polygon_vertices =
                        surface_mesh.polygon_vertices( polygon );
                    for( auto& polygon_vertex : polygon_vertices )
                    {
                        const auto unique_vertex = brep.unique_vertex(
                            { boundary.component_id(), polygon_vertex } );
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
                    polygonal_surface_builder->create_polygon(
                        polygon_vertices );
                }
            }
        }
        polygonal_surface_builder->compute_polygon_adjacencies();
        return polygonal_surface->nb_vertices()
                   - polygonal_surface->edges().nb_edges()
                   + polygonal_surface->nb_polygons()
               == 2;
    }
} // namespace

namespace geode
{
    index_t BRepBlocksTopologyInspectionResult::nb_issues() const
    {
        return wrong_block_boundary_surface.nb_issues()
               + model_boundaries_dont_form_a_closed_surface.nb_issues()
               + some_blocks_not_meshed.nb_issues()
               + blocks_not_linked_to_a_unique_vertex.nb_issues()
               + unique_vertices_part_of_two_blocks_and_no_boundary_surface
                     .nb_issues()
               + unique_vertices_with_incorrect_block_cmvs_count.nb_issues()
               + unique_vertices_linked_to_a_single_and_invalid_line.nb_issues()
               + unique_vertices_linked_to_not_internal_nor_boundary_line
                     .nb_issues()
               + unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks
                     .nb_issues()
               + unique_vertices_linked_to_a_single_and_invalid_surface
                     .nb_issues()
               + unique_vertex_linked_to_multiple_invalid_surfaces.nb_issues();
    }

    std::string BRepBlocksTopologyInspectionResult::string() const
    {
        std::string message;
        if( wrong_block_boundary_surface.nb_issues() != 0 )
        {
            absl::StrAppend( &message, wrong_block_boundary_surface.string() );
        }
        if( model_boundaries_dont_form_a_closed_surface.nb_issues() != 0 )
        {
            absl::StrAppend( &message,
                model_boundaries_dont_form_a_closed_surface.string() );
        }
        if( some_blocks_not_meshed.nb_issues() != 0 )
        {
            absl::StrAppend( &message, some_blocks_not_meshed.string(), "\n" );
        }
        if( blocks_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, blocks_not_linked_to_a_unique_vertex.string() );
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
        if( unique_vertices_linked_to_a_single_and_invalid_surface.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_with_incorrect_block_cmvs_count.string(),
                "\n" );
        }
        if( unique_vertices_linked_to_a_single_and_invalid_line.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_single_and_invalid_line.string(),
                "\n" );
        }
        if( unique_vertices_linked_to_not_internal_nor_boundary_line.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_not_internal_nor_boundary_line
                    .string(),
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
        return "No issues with blocks topology \n";
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
        for( const auto& surface_cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( surface_cmv.component_id.type()
                != Surface3D::component_type_static() )
            {
                continue;
            }
            if( brep_.Relationships::is_boundary(
                    surface_cmv.component_id.id(), block_uuids[0] )
                && brep_.Relationships::is_boundary(
                    surface_cmv.component_id.id(), block_uuids[1] ) )
            {
                return std::nullopt;
            }
            for( const auto& line_cmv :
                brep_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( line_cmv.component_id.type()
                    != Line3D::component_type_static() )
                {
                    continue;
                }
                if( brep_.Relationships::is_boundary(
                        line_cmv.component_id.id(),
                        surface_cmv.component_id.id() )
                    && ( brep_.Relationships::is_boundary(
                             surface_cmv.component_id.id(), block_uuids[0] )
                         || brep_.Relationships::is_boundary(
                             surface_cmv.component_id.id(), block_uuids[1] ) ) )
                {
                    return std::nullopt;
                }
            }
        }
        return absl::StrCat( "Unique vertex with index ", unique_vertex_index,
            " is part of two blocks, but not of a surface boundary to the "
            "two "
            "blocks, nor of a line boundary to one of the blocks incident "
            "surfaces." );
    }

    std::optional< std::string >
        BRepBlocksTopology::unique_vertex_block_cmvs_count_is_incorrect(
            index_t unique_vertex_index ) const
    {
        const auto block_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Block3D::component_type_static() );

        std::vector< ComponentMeshVertex > block_cmvs;
        std::vector< ComponentMeshVertex > surface_cmvs;
        std::vector< ComponentMeshVertex > line_cmvs;
        std::vector< ComponentMeshVertex > corner_cmvs;
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Block3D::component_type_static() )
            {
                block_cmvs.push_back( cmv );
            }
            if( cmv.component_id.type() == Surface3D::component_type_static() )
            {
                surface_cmvs.push_back( cmv );
            }
            if( cmv.component_id.type() == Line3D::component_type_static() )
            {
                line_cmvs.push_back( cmv );
            }
            if( cmv.component_id.type() == Corner3D::component_type_static() )
            {
                corner_cmvs.push_back( cmv );
            }
        }
        for( const auto& block_uuid : block_uuids )
        {
            const auto nb_block_cmvs = count_cmvs(
                block_cmvs, [&block_uuid]( const ComponentMeshVertex& cmv ) {
                    return cmv.component_id.id() == block_uuid;
                } );

            const auto nb_internal_surface_cmvs = count_cmvs( surface_cmvs,
                [&block_uuid, this]( const ComponentMeshVertex& cmv ) {
                    return this->brep_.is_internal(
                        brep_.surface( cmv.component_id.id() ),
                        brep_.block( block_uuid ) );
                } );

            const auto nb_boundary_surface_cmvs = count_cmvs( surface_cmvs,
                [&block_uuid, this]( const ComponentMeshVertex& cmv ) {
                    return this->brep_.is_boundary(
                        brep_.surface( cmv.component_id.id() ),
                        brep_.block( block_uuid ) );
                } );
            const auto nb_boundary_line_cmvs = count_cmvs( line_cmvs,
                [&block_uuid, this]( const ComponentMeshVertex& cmv ) {
                    for( const auto& block_boundary :
                        this->brep_.boundaries( brep_.block( block_uuid ) ) )
                    {
                        for( const auto& surface_boundary :
                            this->brep_.boundaries( block_boundary ) )
                        {
                            if( surface_boundary.id() == cmv.component_id.id() )
                            {
                                return true;
                            }
                        }
                        for( const auto& surface_internal :
                            this->brep_.internal_lines( block_boundary ) )
                        {
                            if( surface_internal.id() == cmv.component_id.id() )
                            {
                                return true;
                            }
                        }
                    }
                    return false;
                } );
            const auto nb_free_line_cmvs = count_cmvs( line_cmvs,
                [&block_uuid, this]( const ComponentMeshVertex& cmv ) {
                    return this->brep_.nb_incidences( cmv.component_id.id() )
                               == 1
                           && this->brep_.nb_embedding_surfaces(
                                  brep_.line( cmv.component_id.id() ) )
                                  == 0;
                } );
            if( corner_cmvs.size() == 1 && nb_internal_surface_cmvs == 0 )
            {
                if( nb_boundary_line_cmvs == 1 )
                {
                    if( nb_block_cmvs != 1 )
                    {
                        return absl::StrCat( "Unique vertex with index ",
                            unique_vertex_index, " is part of block ",
                            block_uuid.string(),
                            " and exactly one corner and one line but "
                            "has ",
                            nb_block_cmvs,
                            " block component mesh vertices (should be "
                            "1)." );
                    }
                    continue;
                }

                const auto predicted_nb_block_cmvs = nb_boundary_surface_cmvs
                                                     + corner_cmvs.size()
                                                     - nb_boundary_line_cmvs;
                if( nb_block_cmvs != predicted_nb_block_cmvs )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index, " is part of the block ",
                        block_uuid.string(),
                        ", and of a corner, and of no internal line, ",
                        "and of ", nb_boundary_surface_cmvs,
                        " boundary surface(s), and of ", nb_boundary_line_cmvs,
                        " line(s) on block boundaries, with ", nb_block_cmvs,
                        " block component mesh vertices (should be ",
                        predicted_nb_block_cmvs, ")." );
                }
                continue;
            }

            if( nb_internal_surface_cmvs == 0 )
            {
                const auto predicted_nb_block_cmvs =
                    nb_boundary_line_cmvs == 0 ? 1
                                               : nb_boundary_surface_cmvs / 2;
                if( nb_block_cmvs != predicted_nb_block_cmvs )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index, " is part of the block ",
                        block_uuid.string(),
                        " and none of its internal surfaces but has ",
                        nb_block_cmvs,
                        " block component mesh vertices (should be ",
                        predicted_nb_block_cmvs, ")." );
                }
                continue;
            }
            auto predicted_nb_block_cmvs =
                nb_internal_surface_cmvs < nb_free_line_cmvs + 1
                    ? static_cast< index_t >( 1 )
                    : nb_internal_surface_cmvs - nb_free_line_cmvs;
            if( nb_internal_surface_cmvs - nb_free_line_cmvs == 1 )
            {
                predicted_nb_block_cmvs++;
            }
            if( nb_boundary_surface_cmvs > 1 && corner_cmvs.empty() )
            {
                predicted_nb_block_cmvs += ( nb_boundary_surface_cmvs - 2 ) / 2;
            }
            if( nb_block_cmvs != predicted_nb_block_cmvs )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of the block ",
                    block_uuid.string(), ", has ", nb_internal_surface_cmvs,
                    " internal surface(s) component mesh vertices (CMVs), "
                    "has ",
                    nb_boundary_surface_cmvs,
                    " boundary surface(s) CMVs, and has ", nb_free_line_cmvs,
                    " free line(s) CMVs, with ", nb_block_cmvs,
                    " block CMVs (should be ", predicted_nb_block_cmvs, ")." );
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepBlocksTopology::vertex_is_part_of_invalid_single_line(
            index_t unique_vertex_index ) const
    {
        const auto line_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Line3D::component_type_static() );
        if( line_uuids.size() != 1 )
        {
            return std::nullopt;
        }
        const auto& line_id = line_uuids[0];
        const auto surface_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Surface3D::component_type_static() );
        const auto block_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Block3D::component_type_static() );
        if( brep_.nb_embedding_surfaces( brep_.line( line_id ) ) < 1
            && brep_.nb_incidences( line_id ) < 1 )
        {
            return std::nullopt;
        }
        if( surface_uuids.size() == 1 )
        {
            if( !brep_.Relationships::is_internal( line_id, surface_uuids[0] )
                && !( brep_.Relationships::nb_embeddings( surface_uuids[0] ) > 0
                      || brep_.Relationships::is_boundary(
                          line_id, surface_uuids[0] ) ) )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index,
                    " is part of only one line, with uuid '", line_id.string(),
                    "', and only one surface, with uuid '",
                    surface_uuids[0].string(),
                    "', but the line is neither embedded in the "
                    "surface, nor boundary of the surface while the "
                    "surface is embedded in a block." );
            }
        }
        else if( surface_uuids.empty() )
        {
            if( !internal::brep_blocks_are_meshed( brep_ ) )
            {
                return std::nullopt;
            }
            if( block_uuids.size() != 1 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index,
                    " is part of only one line, with uuid '", line_id.string(),
                    "', no surfaces, but is part of ", block_uuids.size(),
                    " blocks, instead of one." );
            }
            if( !brep_.Relationships::is_internal( line_id, block_uuids[0] ) )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index,
                    " is part of only one line, with uuid '", line_id.string(),
                    "', no surfaces, one block, but the line is not "
                    "internal to the block." );
            }
        }
        else
        {
            for( const auto& surface_id : surface_uuids )
            {
                if( !brep_.Relationships::is_boundary( line_id, surface_id )
                    && !brep_.Relationships::is_internal(
                        line_id, surface_id ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one line, with uuid '",
                        line_id.string(),
                        "', and multiple surfaces, but the line is "
                        "neither internal nor boundary of surface with "
                        "uuid '",
                        surface_id.string(), "', in which the vertex is." );
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string > BRepBlocksTopology::
        vertex_is_part_of_line_with_wrong_relationships_to_surface(
            index_t unique_vertex_index ) const
    {
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() != Line3D::component_type_static() )
            {
                continue;
            }
            const auto cme = geode::component_mesh_edges(
                brep_, brep_.line( cmv.component_id.id() ), 0 );
            if( brep_.nb_incidences( cmv.component_id.id() ) >= 1 )
            {
                for( const auto& incident_surface :
                    brep_.incidences( brep_.line( cmv.component_id.id() ) ) )
                {
                    if( !cme.surface_edges.contains( incident_surface.id() ) )
                    {
                        return absl::StrCat( "Unique vertex with index ",
                            unique_vertex_index, " is part of line with uuid '",
                            cmv.component_id.id().string(),
                            "', which should be boundary of "
                            "surface with uuid '",
                            incident_surface.id().string() );
                    }
                    const auto& surface_edges =
                        cme.surface_edges.at( incident_surface.id() );
                    if( surface_edges.size() != 1 )
                    {
                        return absl::StrCat( "Unique vertex with index ",
                            unique_vertex_index, " is part of line with uuid '",
                            cmv.component_id.id().string(),
                            "', which should not be boundary of "
                            "surface with uuid '",
                            incident_surface.id().string() );
                    }
                }
            }
            if( brep_.nb_embedding_surfaces(
                    brep_.line( cmv.component_id.id() ) )
                >= 1 )
            {
                for( const auto& embedding_surface : brep_.embedding_surfaces(
                         brep_.line( cmv.component_id.id() ) ) )
                {
                    if( !cme.surface_edges.contains( embedding_surface.id() ) )
                    {
                        return absl::StrCat( "Unique vertex with index ",
                            unique_vertex_index, " is part of line with uuid '",
                            cmv.component_id.id().string(),
                            "', which should not be embedded in "
                            "surface with uuid '",
                            embedding_surface.id().string() );
                    }
                    const auto& surface_edges =
                        cme.surface_edges.at( embedding_surface.id() );
                    if( surface_edges.size() <= 1 )
                    {
                        return absl::StrCat( "Unique vertex with index ",
                            unique_vertex_index, " is part of line with uuid '",
                            cmv.component_id.id().string(),
                            "', which should not be embedded in "
                            "surface with uuid '",
                            embedding_surface.id().string() );
                    }
                }
            }
            if( brep_.nb_incidences( cmv.component_id.id() ) < 1
                && brep_.nb_embedding_surfaces(
                       brep_.line( cmv.component_id.id() ) )
                       < 1 )
            {
                for( const auto& surface : brep_.surfaces() )
                {
                    if( cme.surface_edges.contains( surface.id() ) )
                    {
                        return absl::StrCat( "Unique vertex with index ",
                            unique_vertex_index, " is part of line with uuid '",
                            cmv.component_id.id().string(),
                            "', which should be internal or boundary to "
                            "surface with uuid '",
                            surface.id().string() );
                    }
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string > BRepBlocksTopology::
        vertex_is_part_of_surface_with_wrong_relationships_to_block(
            index_t unique_vertex_index,
            absl::Span< const uuid > not_boundary_surfaces ) const
    {
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() != Surface3D::component_type_static() )
            {
                continue;
            }
            if( brep_.nb_incidences( cmv.component_id.id() ) >= 1
                && absl::c_contains(
                    not_boundary_surfaces, cmv.component_id.id() ) )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of surface with uuid '",
                    cmv.component_id.id().string(),
                    "', which should not be boundary of any block." );
            }
            if( brep_.nb_embeddings( cmv.component_id.id() ) >= 1
                && brep_.nb_incidences( cmv.component_id.id() ) < 1
                && !absl::c_contains(
                    not_boundary_surfaces, cmv.component_id.id() ) )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of surface with uuid '",
                    cmv.component_id.id().string(),
                    "', which should not be embedded in any block." );
            }
            if( brep_.nb_embeddings( cmv.component_id.id() ) < 1
                && brep_.nb_incidences( cmv.component_id.id() ) < 1
                && !absl::c_contains(
                    not_boundary_surfaces, cmv.component_id.id() ) )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of surface with uuid '",
                    cmv.component_id.id().string(),
                    "', which should be boundary of a block." );
            }
            if( brep_.nb_incidences( cmv.component_id.id() ) < 1
                && brep_.nb_embeddings( cmv.component_id.id() ) < 1 )
            {
                const auto& surface_mesh =
                    brep_.surface( cmv.component_id.id() ).mesh();
                const auto polygon_barycenter =
                    surface_mesh.polygon_barycenter( 0 );
                bool is_dangling{ true };
                for( const auto& block : brep_.blocks() )
                {
                    if( geode::is_point_inside_block(
                            brep_, block, polygon_barycenter ) )
                    {
                        is_dangling = false;
                        break;
                    }
                }
                if( !is_dangling )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index, " is part of surface with uuid '",
                        cmv.component_id.id().string(),
                        "', which is not internal to "
                        "a block while it should be." );
                }
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
        if( surface_uuids.size() != 1 )
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
            return absl::StrCat( "Unique vertex with index ",
                unique_vertex_index,
                " is part of only one surface, but is part of more "
                "than two blocks." );
        }
        if( brep_.nb_embeddings( surface_id ) > 0 )
        {
            if( internal::brep_blocks_are_meshed( brep_ ) )
            {
                if( block_uuids.size() != 1 )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one surface, which is "
                        "embedded, but not part of only one block." );
                }
                else if( !brep_.Relationships::is_internal(
                             surface_id, block_uuids[0] ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one surface, which is "
                        "embedded, and one block, but the surface is "
                        "not internal to the block." );
                }
            }
        }
        else
        {
            for( const auto& block_id : block_uuids )
            {
                if( !brep_.Relationships::is_boundary( surface_id, block_id ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one surface, with uuid'",
                        surface_id.string(),
                        "' which is not embedded, but not boundary "
                        "either of block with uuid '",
                        block_id.string(), "', in which the vertex is." );
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
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index,
                    " is part of multiple surfaces and multiple "
                    "lines, but line with uuid'",
                    line_id.string(),
                    "' is neither internal, nor a boundary of at "
                    "least two surfaces or one embedding "
                    "surface." );
            }
        }
        return std::nullopt;
    }

    BRepBlocksTopologyInspectionResult
        BRepBlocksTopology::inspect_blocks() const
    {
        BRepBlocksTopologyInspectionResult result;
        std::vector< geode::uuid > meshed_blocks;
        const auto not_boundary_surfaces = find_not_boundary_surfaces( brep_ );
        std::vector< geode::uuid > boundary_uuids;
        for( const auto& block : brep_.blocks() )
        {
            for( const auto& boundary : brep_.boundaries( block ) )
            {
                if( absl::c_contains( boundary_uuids, boundary.id() ) )
                {
                    continue;
                }
                boundary_uuids.push_back( boundary.id() );
            }
        }
        for( const auto& boundary : brep_.model_boundaries() )
        {
            if( boundary.component_type_static()
                != Surface3D::component_type_static() )
            {
                continue;
            }
            if( absl::c_contains( boundary_uuids, boundary.id() ) )
            {
                continue;
            }
            boundary_uuids.push_back( boundary.id() );
        }
        for( const auto& block : brep_.blocks() )
        {
            if( brep_.block( block.id() ).mesh().nb_vertices() != 0 )
            {
                meshed_blocks.push_back( block.id() );
            }
        }
        if( meshed_blocks.size() != brep_.nb_blocks()
            || meshed_blocks.size() == 0 )
        {
            for( const auto& meshed_block_id : meshed_blocks )
            {
                result.some_blocks_not_meshed.add_issue( meshed_block_id,
                    absl::StrCat( "Block ", meshed_block_id.string(),
                        " is not meshed." ) );
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
                block_result.set_description(
                    absl::StrCat( "Block ", meshed_block_id.string() ) );
                result.blocks_not_linked_to_a_unique_vertex.add_issues_to_map(
                    block.id(), std::move( block_result ) );
            }
            if( verify_blocks_boundaries( brep_, block ) )
            {
                result.blocks_with_not_closed_boundary_surfaces.add_issue(
                    block.id(), absl::StrCat( "Block ", block.id().string(),
                                    " boundaries are valid." ) );
            }
        }
        if( brep_.nb_model_boundaries() != 0 )
        {
            if( !are_brep_boundaries_closed( brep_ ) )
            {
                result.model_boundaries_dont_form_a_closed_surface.add_issue(
                    0, absl::StrCat(
                           "Brep boundaries are not topologically valid." ) );
            }
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( const auto problem_message =
                    unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_part_of_two_blocks_and_no_boundary_surface
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    unique_vertex_block_cmvs_count_is_incorrect(
                        unique_vertex_id ) )
            {
                result.unique_vertices_with_incorrect_block_cmvs_count
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    vertex_is_part_of_invalid_single_surface(
                        unique_vertex_id, not_boundary_surfaces ) )
            {
                result.unique_vertices_linked_to_a_single_and_invalid_surface
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    vertex_is_part_of_line_with_wrong_relationships_to_surface(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_not_internal_nor_boundary_line
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    vertex_is_part_of_invalid_single_line( unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_a_single_and_invalid_line
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    vertex_is_part_of_surface_with_wrong_relationships_to_block(
                        unique_vertex_id, not_boundary_surfaces ) )
            {
                result
                    .unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    vertex_is_part_of_invalid_multiple_surfaces(
                        unique_vertex_id ) )
            {
                result.unique_vertex_linked_to_multiple_invalid_surfaces
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
        }
        for( const auto& block : brep_.blocks() )
        {
            const auto block_boundary_uuids =
                block_boundary_surfaces( brep_, block );
            for( const auto& bsurf_uuid : block_boundary_uuids )
            {
                if( surface_should_not_be_boundary_to_block(
                        bsurf_uuid, brep_, block_boundary_uuids ) )
                {
                    result.wrong_block_boundary_surface.add_issue( bsurf_uuid,
                        absl::StrCat( "Surface ", bsurf_uuid.string(),
                            " should not be boundary of Block ",
                            block.id().string(),
                            " : it has a boundary line not incident to any "
                            "other block "
                            "boundary surface." ) );
                }
            }
        }
        return result;
    }
} // namespace geode
