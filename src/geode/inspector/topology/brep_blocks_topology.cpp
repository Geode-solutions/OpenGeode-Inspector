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

    absl::optional< std::string >
        BRepBlocksTopology::brep_vertex_blocks_topology_is_valid(
            index_t unique_vertex_index ) const
    {
        const auto block_cmvs = brep_.component_mesh_vertices(
            unique_vertex_index, Block3D::component_type_static() );
        const auto block_uuids = detail::components_uuids( block_cmvs );
        if( block_uuids.size() == 2 )
        {
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
                for( const auto& line :
                    brep_.component_mesh_vertices(
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
            return std::string( "Unique vertex with index " )
                   + std::to_string( unique_vertex_index )
                   + std::string(
                       " is part of two blocks, but not of a surface boundary "
                       "to the two blocks, nor of a line boundary to one "
                       "of the blocks incident surfaces." );
        }
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
                        return std::string( "Unique vertex with index " )
                               + std::to_string( unique_vertex_index )
                               + std::string( " is part of the block " )
                               + block_uuid.string()
                               + std::string( " and exactly one corner and one "
                                              "line but has " )
                               + std::to_string( nb_block_cmvs )
                               + std::string( " block component mesh vertices "
                                              "(should be 1)." );
                    }
                    continue;
                }

                const auto predicted_nb_block_cmvs = nb_boundary_surface_cmvs
                                                     - nb_boundary_line_cmvs
                                                     + corner_cmvs.size();
                if( nb_block_cmvs != predicted_nb_block_cmvs )
                {
                    return std::string( "Unique vertex with index " )
                           + std::to_string( unique_vertex_index )
                           + std::string( " is part of the block " )
                           + block_uuid.string()
                           + std::string(
                               ", and of a corner, and of no internal line, " )
                           + std::string( "and of " )
                           + std::to_string( nb_boundary_surface_cmvs )
                           + std::string( " boundary surface(s), and of " )
                           + std::to_string( nb_boundary_line_cmvs )
                           + std::string(
                               " line(s) on block boundaries, with " )
                           + std::to_string( nb_block_cmvs )
                           + std::string( " block component mesh vertices" )
                           + std::string( "(should be " )
                           + std::to_string( predicted_nb_block_cmvs )
                           + std::string( ")." );
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
                    return std::string( "Unique vertex with index " )
                           + std::to_string( unique_vertex_index )
                           + std::string( " is part of the block " )
                           + block_uuid.string()
                           + std::string( " and none of its internal "
                                          "surfaces but has " )
                           + std::to_string( nb_block_cmvs )
                           + std::string( " block component mesh vertices "
                                          "(should be " )
                           + std::to_string( predicted_nb_block_cmvs )
                           + std::string( ")." );
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
                return std::string( "Unique vertex with index " )
                       + std::to_string( unique_vertex_index )
                       + std::string( " is part of the block " )
                       + block_uuid.string() + std::string( ", of " )
                       + std::to_string( nb_internal_surface_cmvs )
                       + std::string( " internal surface(s), of " )
                       + std::to_string( nb_boundary_surface_cmvs )
                       + std::string( " boundary surface(s), and of " )
                       + std::to_string( nb_free_line_cmvs )
                       + std::string( " free line(s), with " )
                       + std::to_string( nb_block_cmvs )
                       + std::string(
                           " block component mesh vertices (should be " )
                       + std::to_string( predicted_nb_block_cmvs )
                       + std::string( ")." );
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
                    block.id().string()
                        + std::string( " is a block without mesh." ) );
            }

            auto block_result = detail::
                brep_component_vertices_not_associated_to_unique_vertices(
                    brep_, block.component_id(), block.mesh() );
            result.blocks_not_linked_to_unique_vertex.problems.insert(
                result.blocks_not_linked_to_unique_vertex.problems.end(),
                std::make_move_iterator( block_result.first.begin() ),
                std::make_move_iterator( block_result.first.end() ) );
            result.blocks_not_linked_to_unique_vertex.messages.insert(
                result.blocks_not_linked_to_unique_vertex.messages.end(),
                std::make_move_iterator( block_result.second.begin() ),
                std::make_move_iterator( block_result.second.end() ) );
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            const auto block_topology_is_valid =
                brep_vertex_blocks_topology_is_valid( unique_vertex_id );

            if( block_topology_is_valid )
            {
                result.part_of_invalid_blocks_unique_vertices.add_problem(
                    unique_vertex_id, block_topology_is_valid.value() );
            }
        }
        return result;
    }
} // namespace geode
