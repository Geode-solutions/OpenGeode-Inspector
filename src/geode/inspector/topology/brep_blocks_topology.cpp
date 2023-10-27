/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/inspector/topology/brep_blocks_topology.h>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>
#include <geode/mesh/core/solid_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

#include <geode/inspector/topology/private/topology_helpers.h>

namespace
{
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
} // namespace

namespace geode
{
    BRepBlocksTopology::BRepBlocksTopology( const BRep& brep ) : brep_( brep )
    {
    }

    bool BRepBlocksTopology::brep_blocks_topology_is_valid(
        index_t unique_vertex_index ) const
    {
        return !( unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(
                      +unique_vertex_index )
                  || unique_vertex_block_cmvs_count_is_incorrect(
                      unique_vertex_index ) );
    }

    absl::optional< std::string > BRepBlocksTopology::
        unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(
            index_t unique_vertex_index ) const
    {
        const auto block_cmvs = brep_.component_mesh_vertices(
            unique_vertex_index, Block3D::component_type_static() );
        const auto block_uuids = detail::components_uuids( block_cmvs );
        if( block_uuids.size() != 2 )
        {
            return absl::nullopt;
        }
        for( const auto& surface : brep_.component_mesh_vertices(
                 unique_vertex_index, Surface3D::component_type_static() ) )
        {
            if( brep_.Relationships::is_boundary(
                    surface.component_id.id(), block_uuids[0] )
                && brep_.Relationships::is_boundary(
                    surface.component_id.id(), block_uuids[1] ) )
            {
                return absl::nullopt;
            }
            for( const auto& line : brep_.component_mesh_vertices(
                     unique_vertex_index, Line3D::component_type_static() ) )
            {
                if( brep_.Relationships::is_boundary(
                        line.component_id.id(), surface.component_id.id() )
                    && ( brep_.Relationships::is_boundary(
                             surface.component_id.id(), block_uuids[0] )
                         || brep_.Relationships::is_boundary(
                             surface.component_id.id(), block_uuids[1] ) ) )
                {
                    return absl::nullopt;
                }
            }
        }
        return "Unique vertex with index " + absl::StrCat( unique_vertex_index )
               + " is part of two blocks, but not of a surface boundary to the "
                 "two blocks, nor of a line boundary to one of the blocks "
                 "incident surfaces.";
    }

    absl::optional< std::string >
        BRepBlocksTopology::unique_vertex_block_cmvs_count_is_incorrect(
            index_t unique_vertex_index ) const
    {
        const auto block_cmvs = brep_.component_mesh_vertices(
            unique_vertex_index, Block3D::component_type_static() );
        const auto block_uuids = detail::components_uuids( block_cmvs );
        const auto corner_cmvs = brep_.component_mesh_vertices(
            unique_vertex_index, Corner3D::component_type_static() );
        const auto line_cmvs = brep_.component_mesh_vertices(
            unique_vertex_index, Line3D::component_type_static() );
        const auto surface_cmvs = brep_.component_mesh_vertices(
            unique_vertex_index, Surface3D::component_type_static() );
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
                        return "Unique vertex with index "
                               + absl::StrCat( unique_vertex_index )
                               + " is part of block " + block_uuid.string()
                               + " and exactly one corner and one line but has "
                               + absl::StrCat( nb_block_cmvs )
                               + " block component mesh vertices (should be "
                                 "1).";
                    }
                    continue;
                }

                const auto predicted_nb_block_cmvs = nb_boundary_surface_cmvs
                                                     - nb_boundary_line_cmvs
                                                     + corner_cmvs.size();
                if( nb_block_cmvs != predicted_nb_block_cmvs )
                {
                    return "Unique vertex with index "
                           + absl::StrCat( unique_vertex_index )
                           + " is part of the block " + block_uuid.string()
                           + ", and of a corner, and of no internal line, "
                           + "and of "
                           + absl::StrCat( nb_boundary_surface_cmvs )
                           + " boundary surface(s), and of "
                           + absl::StrCat( nb_boundary_line_cmvs )
                           + " line(s) on block boundaries, with "
                           + absl::StrCat( nb_block_cmvs )
                           + " block component mesh vertices" + "(should be "
                           + absl::StrCat( predicted_nb_block_cmvs ) + ").";
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
                    return "Unique vertex with index "
                           + absl::StrCat( unique_vertex_index )
                           + " is part of the block " + block_uuid.string()
                           + " and none of its internal surfaces but has "
                           + absl::StrCat( nb_block_cmvs )
                           + " block component mesh vertices (should be "
                           + absl::StrCat( predicted_nb_block_cmvs ) + ").";
                }
                continue;
            }
            auto predicted_nb_block_cmvs =
                std::max( static_cast< index_t >( 1 ),
                    nb_internal_surface_cmvs - nb_free_line_cmvs );
            if( nb_internal_surface_cmvs - nb_free_line_cmvs == 1 )
            {
                predicted_nb_block_cmvs++;
            }
            if( nb_boundary_surface_cmvs > 0 && corner_cmvs.empty() )
            {
                predicted_nb_block_cmvs += ( nb_boundary_surface_cmvs - 2 ) / 2;
            }
            if( nb_block_cmvs != predicted_nb_block_cmvs )
            {
                return "Unique vertex with index "
                       + absl::StrCat( unique_vertex_index )
                       + " is part of the block " + block_uuid.string()
                       + ", of " + absl::StrCat( nb_internal_surface_cmvs )
                       + " internal surface(s), of "
                       + absl::StrCat( nb_boundary_surface_cmvs )
                       + " boundary surface(s), and of "
                       + absl::StrCat( nb_free_line_cmvs )
                       + " free line(s), with " + absl::StrCat( nb_block_cmvs )
                       + " block component mesh vertices (should be "
                       + absl::StrCat( predicted_nb_block_cmvs ) + ").";
            }
        }
        return absl::nullopt;
    }

    BRepBlocksTopologyInspectionResult
        BRepBlocksTopology::inspect_blocks() const
    {
        BRepBlocksTopologyInspectionResult result;
        for( const auto& block : brep_.blocks() )
        {
            if( brep_.block( block.id() ).mesh().nb_vertices() == 0 )
            {
                result.blocks_not_meshed.add_problem( block.id(),
                    "Block " + block.id().string() + " is not meshed." );
                continue;
            }
            auto block_result = detail::
                brep_component_vertices_not_associated_to_unique_vertices(
                    brep_, block.component_id(), block.mesh() );
            result.blocks_not_linked_to_a_unique_vertex.emplace_back(
                block.id(),
                "Block " + block.id().string()
                    + " has mesh vertices not linked to a unique vertex." );
            result.blocks_not_linked_to_a_unique_vertex.back().second.problems =
                std::move( block_result.first );
            result.blocks_not_linked_to_a_unique_vertex.back().second.messages =
                std::move( block_result.second );
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( const auto problem_message =
                    unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_part_of_two_blocks_and_no_boundary_surface
                    .add_problem( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    unique_vertex_block_cmvs_count_is_incorrect(
                        unique_vertex_id ) )
            {
                result.unique_vertices_with_incorrect_block_cmvs_count
                    .add_problem( unique_vertex_id, problem_message.value() );
            }
        }
        return result;
    }
} // namespace geode
