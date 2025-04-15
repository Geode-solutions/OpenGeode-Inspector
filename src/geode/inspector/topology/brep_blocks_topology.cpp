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

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/graph_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/builder/vertex_set_builder.hpp>
#include <geode/mesh/core/graph.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/io/graph_output.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/relationships.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/topology/internal/topology_helpers.hpp>

namespace
{
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

    std::vector< geode::uuid > find_not_boundaries_surface(
        const geode::BRep& brep )
    {
        std::vector< geode::uuid > not_boundaries;
        auto graph = geode::Graph::create();
        auto graph_builder = geode::GraphBuilder::create( *graph );
        geode::BijectiveMapping< geode::uuid, geode::index_t >
            surface_uuids_to_graph_vertices;
        for( const auto& surface : brep.surfaces() )
        {
            if( !surface_uuids_to_graph_vertices.has_mapping_input(
                    surface.id() ) )
            {
                surface_uuids_to_graph_vertices.map(
                    surface.id(), graph_builder->create_vertex() );
            }
            const auto surface_vertex =
                surface_uuids_to_graph_vertices.in2out( surface.id() );
            for( const auto& boundary_line : brep.boundaries( surface ) )
            {
                for( const auto& incident_surface :
                    brep.incidences( boundary_line ) )
                {
                    if( incident_surface.id() == surface.id() )
                    {
                        continue;
                    }
                    if( !surface_uuids_to_graph_vertices.has_mapping_input(
                            incident_surface.id() ) )
                    {
                        surface_uuids_to_graph_vertices.map(
                            incident_surface.id(),
                            graph_builder->create_vertex() );
                    }
                    const auto& incident_surface_vertex =
                        surface_uuids_to_graph_vertices.in2out(
                            incident_surface.id() );
                    if( graph->edge_from_vertices(
                            incident_surface_vertex, surface_vertex ) )
                    {
                        continue;
                    }
                    graph_builder->create_edge(
                        incident_surface_vertex, surface_vertex );
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
                if( graph->edges_around_vertex( graph_vertex ).size() > 1 )
                {
                    continue;
                }
                const auto surface_id =
                    surface_uuids_to_graph_vertices.out2in( graph_vertex );
                if( graph->is_vertex_isolated( graph_vertex ) )
                {
                    not_boundaries.push_back( surface_id );
                    continue;
                }
                not_boundaries.push_back( surface_id );
                to_delete[graph->edges_around_vertex( graph_vertex )
                        .at( 0 )
                        .edge_id] = true;
            }
            if( !absl::c_contains( to_delete, true ) )
            {
                found_not_boundary_surface = false;
                continue;
            }
            graph_builder->delete_edges( to_delete );
            graph_builder->delete_isolated_vertices();
        }
        return not_boundaries;
    }

    std::vector< geode::uuid > find_not_boundaries_lines(
        const geode::BRep& brep )
    {
        std::vector< geode::uuid > not_boundaries;
        auto graph = geode::Graph::create();
        auto graph_builder = geode::GraphBuilder::create( *graph );
        geode::BijectiveMapping< geode::uuid, geode::index_t >
            line_uuids_to_graph_vertices;
        for( const auto& line : brep.lines() )
        {
            if( !line_uuids_to_graph_vertices.has_mapping_input( line.id() ) )
            {
                line_uuids_to_graph_vertices.map(
                    line.id(), graph_builder->create_vertex() );
            }
            const auto line_vertex =
                line_uuids_to_graph_vertices.in2out( line.id() );
            for( const auto& boundary_corner : brep.boundaries( line ) )
            {
                for( const auto& incident_line :
                    brep.incidences( boundary_corner ) )
                {
                    if( incident_line.id() == line.id() )
                    {
                        continue;
                    }
                    if( !line_uuids_to_graph_vertices.has_mapping_input(
                            incident_line.id() ) )
                    {
                        line_uuids_to_graph_vertices.map( incident_line.id(),
                            graph_builder->create_vertex() );
                    }
                    const auto& incident_line_vertex =
                        line_uuids_to_graph_vertices.in2out(
                            incident_line.id() );
                    if( graph->edge_from_vertices(
                            incident_line_vertex, line_vertex ) )
                    {
                        continue;
                    }
                    graph_builder->create_edge(
                        incident_line_vertex, line_vertex );
                }
            }
        }
        bool found_not_boundary_line{ true };
        while( found_not_boundary_line )
        {
            std::vector< bool > to_delete( graph->nb_edges(), false );
            for( const auto graph_vertex :
                geode::Range{ graph->nb_vertices() } )
            {
                if( graph->edges_around_vertex( graph_vertex ).size() > 1 )
                {
                    continue;
                }
                const auto line_id =
                    line_uuids_to_graph_vertices.out2in( graph_vertex );
                if( graph->is_vertex_isolated( graph_vertex ) )
                {
                    not_boundaries.push_back( line_id );
                    continue;
                }
                not_boundaries.push_back( line_id );
                to_delete[graph->edges_around_vertex( graph_vertex )
                        .at( 0 )
                        .edge_id] = true;
            }
            if( !absl::c_contains( to_delete, true ) )
            {
                found_not_boundary_line = false;
                continue;
            }
            graph_builder->delete_edges( to_delete );
            const auto vertex_old2new =
                graph_builder->delete_isolated_vertices();
            geode::BijectiveMapping< geode::uuid, geode::index_t >
                updated_mapping;
            for( const auto old_index : geode::Indices{ vertex_old2new } )
            {
                if( vertex_old2new.at( old_index ) == geode::NO_ID )
                {
                    continue;
                }
                const auto line_id =
                    line_uuids_to_graph_vertices.out2in( old_index );
                updated_mapping.map( line_id, vertex_old2new.at( old_index ) );
            }
            line_uuids_to_graph_vertices = updated_mapping;
        }
        return not_boundaries;
    }

    bool boundaries_of_block_form_a_closed_surface( const geode::BRep& brep,
        const geode::Block3D& block,
        absl::Span< const geode::uuid > not_boundaries_surfaces )
    {
        auto surface_mesh = geode::SurfaceMesh3D::create();
        auto builder = geode::SurfaceMeshBuilder3D::create( *surface_mesh );
        geode::BijectiveMapping< geode::index_t >
            unique_vertex_to_surface_vertex;
        for( const auto& boundary_surface : brep.boundaries( block ) )
        {
            const auto& boundary_surface_mesh = boundary_surface.mesh();
            for( const auto polygon :
                geode::Range{ boundary_surface_mesh.nb_polygons() } )
            {
                absl::FixedArray< geode::index_t > polygon_vertices(
                    boundary_surface_mesh.nb_polygon_vertices( polygon ) );
                for( const auto local_vertex :
                    geode::LRange{
                        boundary_surface_mesh.nb_polygon_vertices( polygon ) } )
                {
                    const auto boundary_surface_vertex =
                        boundary_surface_mesh.polygon_vertex(
                            { polygon, local_vertex } );
                    const auto unique_vertex =
                        brep.unique_vertex( { boundary_surface.component_id(),
                            boundary_surface_vertex } );
                    if( unique_vertex_to_surface_vertex.has_mapping_input(
                            unique_vertex ) )
                    {
                        polygon_vertices.at( local_vertex ) =
                            unique_vertex_to_surface_vertex.in2out(
                                unique_vertex );
                        continue;
                    }
                    const auto new_vertex =
                        builder->create_point( boundary_surface_mesh.point(
                            boundary_surface_vertex ) );
                    unique_vertex_to_surface_vertex.map(
                        unique_vertex, new_vertex );
                    polygon_vertices.at( local_vertex ) = new_vertex;
                }
                builder->create_polygon( polygon_vertices );
            }
        }
        builder->compute_polygon_adjacencies();
        const auto euler_characteristic = surface_mesh->nb_vertices()
                                          - surface_mesh->edges().nb_edges()
                                          + surface_mesh->nb_polygons();
        return euler_characteristic == 2;
    }

    bool are_model_boundaries_closed( const geode::BRep& brep,
        absl::Span< const geode::uuid > not_boundaries_surfaces )
    {
        auto surface_mesh = geode::SurfaceMesh3D::create();
        auto builder = geode::SurfaceMeshBuilder3D::create( *surface_mesh );
        geode::BijectiveMapping< geode::index_t >
            unique_vertex_to_surface_vertex;
        for( const auto& model_boundary : brep.model_boundaries() )
        {
            for( const auto& surface_in_model_boundary :
                brep.model_boundary_items( model_boundary ) )
            {
                const auto& boundary_surface_mesh =
                    surface_in_model_boundary.mesh();
                for( const auto polygon :
                    geode::Range{ boundary_surface_mesh.nb_polygons() } )
                {
                    absl::FixedArray< geode::index_t > polygon_vertices(
                        boundary_surface_mesh.nb_polygon_vertices( polygon ) );
                    for( const auto local_vertex : geode::LRange{
                             boundary_surface_mesh.nb_polygon_vertices(
                                 polygon ) } )
                    {
                        const auto boundary_surface_vertex =
                            boundary_surface_mesh.polygon_vertex(
                                { polygon, local_vertex } );
                        const auto unique_vertex = brep.unique_vertex(
                            { surface_in_model_boundary.component_id(),
                                boundary_surface_vertex } );
                        if( unique_vertex_to_surface_vertex.has_mapping_input(
                                unique_vertex ) )
                        {
                            polygon_vertices.at( local_vertex ) =
                                unique_vertex_to_surface_vertex.in2out(
                                    unique_vertex );
                            continue;
                        }
                        const auto new_vertex =
                            builder->create_point( boundary_surface_mesh.point(
                                boundary_surface_vertex ) );
                        unique_vertex_to_surface_vertex.map(
                            unique_vertex, new_vertex );
                        polygon_vertices.at( local_vertex ) = new_vertex;
                    }
                    builder->create_polygon( polygon_vertices );
                }
            }
        }
        builder->compute_polygon_adjacencies();
        const auto euler_characteristic = surface_mesh->nb_vertices()
                                          - surface_mesh->edges().nb_edges()
                                          + surface_mesh->nb_polygons();
        if( euler_characteristic != 2 )
        {
            return false;
        }
        return true;
    }

} // namespace

namespace geode
{
    index_t BRepBlocksTopologyInspectionResult::nb_issues() const
    {
        return wrong_block_boundary_surface.nb_issues()
               + some_blocks_not_meshed.nb_issues()
               + blocks_not_linked_to_a_unique_vertex.nb_issues()
               + unique_vertices_part_of_two_blocks_and_no_boundary_surface
                     .nb_issues()
               + unique_vertices_with_incorrect_block_cmvs_count.nb_issues()
               + unique_vertices_linked_to_a_single_and_invalid_line.nb_issues()
               + unique_vertices_linked_to_not_internal_nor_boundary_line
                     .nb_issues()
               + unique_vertices_linked_to_not_internal_nor_boundary_surface
                     .nb_issues()
               + unique_vertices_linked_to_a_single_and_invalid_surface
                     .nb_issues();
    }

    std::string BRepBlocksTopologyInspectionResult::string() const
    {
        std::string message;
        if( wrong_block_boundary_surface.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, wrong_block_boundary_surface.string(), "\n" );
        }
        if( some_blocks_not_meshed.nb_issues() != 0 )
        {
            absl::StrAppend( &message, some_blocks_not_meshed.string(), "\n" );
        }
        if( blocks_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, blocks_not_linked_to_a_unique_vertex.string(), "\n" );
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
        if( unique_vertices_linked_to_not_internal_nor_boundary_surface
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_not_internal_nor_boundary_surface
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
        const auto not_boundary_lines = find_not_boundaries_lines( brep_ );
        DEBUG( not_boundary_lines.size() );
        for( const auto& not_boundary_line : not_boundary_lines )
        {
            SDEBUG( not_boundary_line );
        }

        if( surface_uuids.size() == 1 )
        {
            if( !brep_.Relationships::is_internal( line_id, surface_uuids[0] )
                && !( brep_.Relationships::nb_embeddings( surface_uuids[0] ) > 0
                      && brep_.Relationships::is_boundary(
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

    std::optional< std::string >
        BRepBlocksTopology::vertex_is_part_of_not_internal_nor_boundary_line(
            index_t unique_vertex_index ) const
    {
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Line3D::component_type_static()
                && brep_.nb_embeddings( cmv.component_id.id() ) < 1
                && brep_.nb_incidences( cmv.component_id.id() ) < 1 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of line with uuid '",
                    cmv.component_id.id().string(),
                    "', which is neither embedded nor incident." );
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepBlocksTopology::vertex_is_part_of_not_internal_nor_boundary_surface(
            index_t unique_vertex_index ) const
    {
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Surface3D::component_type_static()
                && brep_.nb_embeddings( cmv.component_id.id() ) < 1
                && brep_.nb_incidences( cmv.component_id.id() ) < 1 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of surface with uuid '",
                    cmv.component_id.id().string(),
                    "', which is neither internal to nor a boundary of "
                    "a block." );
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepBlocksTopology::vertex_is_part_of_invalid_single_surface(
            index_t unique_vertex_index ) const
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

    BRepBlocksTopologyInspectionResult
        BRepBlocksTopology::inspect_blocks() const
    {
        BRepBlocksTopologyInspectionResult result;
        std::vector< geode::uuid > meshed_blocks;
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
        const auto not_boundaries_surfaces =
            find_not_boundaries_surface( brep_ );
        for( const auto& meshed_block_id : meshed_blocks )
        {
            const auto& block = brep_.block( meshed_block_id );
            auto block_result = internal::
                brep_component_vertices_not_associated_to_unique_vertices(
                    brep_, block.component_id(), block.mesh() );
            if( block_result.nb_issues() != 0 )
            {
                block_result.set_description(
                    absl::StrCat( "Block ", meshed_block_id.string() ) );
                result.blocks_not_linked_to_a_unique_vertex.add_issues_to_map(
                    block.id(), std::move( block_result ) );
            }
        }
        for( const auto& model_boundary : brep_.model_boundaries() )
        {
            for( const auto& surface_in_model_boundary :
                brep_.model_boundary_items( model_boundary ) )
            {
                if( absl::c_contains( not_boundaries_surfaces,
                        surface_in_model_boundary.id() ) )
                {
                    result.wrong_model_boundaries.add_issue(
                        surface_in_model_boundary.id(),
                        absl::StrCat( "Surface ",
                            surface_in_model_boundary.id().string(),
                            "should not be a model boundary " ) );
                }
            }
        }
        if( !are_model_boundaries_closed( brep_, not_boundaries_surfaces ) )
        {
            // result.add_issue(
            //      absl::StrCat( "Block ", block.id().string(),
            //                     " boundaries of block don't form a closed "
            //                     "surface" ) );
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
            if( const auto boundary_nor_internal_line =
                    vertex_is_part_of_not_internal_nor_boundary_line(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_not_internal_nor_boundary_line
                    .add_issue(
                        unique_vertex_id, boundary_nor_internal_line.value() );
            }
            if( const auto invalid_unique_line =
                    vertex_is_part_of_invalid_single_line( unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_a_single_and_invalid_line
                    .add_issue( unique_vertex_id, invalid_unique_line.value() );
            }
            if( const auto not_boundary_nor_internal_surface =
                    vertex_is_part_of_not_internal_nor_boundary_surface(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_not_internal_nor_boundary_surface
                    .add_issue( unique_vertex_id,
                        not_boundary_nor_internal_surface.value() );
            }
            if( const auto invalid_unique_surface =
                    vertex_is_part_of_invalid_single_surface(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_a_single_and_invalid_surface
                    .add_issue(
                        unique_vertex_id, invalid_unique_surface.value() );
            }
        }
        for( const auto& block : brep_.blocks() )
        {
            const auto block_boundary_uuids =
                block_boundary_surfaces( brep_, block );
            for( const auto& bsurf_uuid : block_boundary_uuids )
            {
                if( absl::c_contains( not_boundaries_surfaces, bsurf_uuid ) )
                {
                    result.wrong_block_boundary_surface.add_issue( bsurf_uuid,
                        absl::StrCat( "Surface ", bsurf_uuid.string(),
                            " should not be boundary of Block ",
                            block.id().string(),
                            " : it is not a boundary surface." ) );
                }
                if( surface_should_not_be_boundary_to_block(
                        bsurf_uuid, brep_, block_boundary_uuids ) )
                {
                    result.wrong_block_boundary_surface.add_issue( bsurf_uuid,
                        absl::StrCat( "Surface ", bsurf_uuid.string(),
                            " should not be boundary of Block ",
                            block.id().string(),
                            " : it has a boundary line not incident to "
                            "any "
                            "other block "
                            "boundary surface." ) );
                }
            }
            if( !boundaries_of_block_form_a_closed_surface(
                    brep_, block, not_boundaries_surfaces ) )
            {
                result.block_is_not_closed.add_issue(
                    block.id(), absl::StrCat( "Block ", block.id().string(),
                                    " boundaries of block don't form a closed "
                                    "surface" ) );
            }
        }
        return result;
    }
} // namespace geode
