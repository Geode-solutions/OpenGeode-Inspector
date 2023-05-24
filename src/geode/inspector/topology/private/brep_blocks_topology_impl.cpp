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

#include <geode/inspector/topology/private/brep_blocks_topology_impl.h>

#include <absl/container/flat_hash_set.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

#include <geode/inspector/topology/private/topology_helpers.h>

namespace geode
{
    namespace detail
    {
        BRepBlocksTopologyImpl::BRepBlocksTopologyImpl(
            const BRep& brep, bool verbose )
            : brep_( brep ), verbose_( verbose )
        {
        }

        BRepBlocksTopologyImpl::BRepBlocksTopologyImpl( const BRep& brep )
            : BRepBlocksTopologyImpl( brep, false )
        {
        }

        bool BRepBlocksTopologyImpl::brep_vertex_blocks_topology_is_valid(
            index_t unique_vertex_index ) const
        {
            const auto block_cmvs = brep_.component_mesh_vertices(
                unique_vertex_index, Block3D::component_type_static() );
            const auto block_uuids = components_uuids( block_cmvs );
            if( block_uuids.size() == 1 )
            {
                const auto line_cmvs = brep_.component_mesh_vertices(
                    unique_vertex_index, Line3D::component_type_static() );
                const auto surface_cmvs = brep_.component_mesh_vertices(
                    unique_vertex_index, Surface3D::component_type_static() );
                absl::flat_hash_set< uuid > internal_surfaces;
                for( const auto& surface_cmv : surface_cmvs )
                {
                    if( brep_.is_internal(
                            brep_.surface( surface_cmv.component_id.id() ),
                            brep_.block( block_uuids.front() ) ) )
                    {
                        if( !internal_surfaces
                                 .emplace( surface_cmv.component_id.id() )
                                 .second
                            && line_cmvs.empty() )
                        {
                            if( verbose_ )
                            {
                                Logger::info( "Unique vertex with index ",
                                    unique_vertex_index,
                                    " is part of one block (",
                                    block_uuids.front().string(),
                                    "), and an internal surface (",
                                    surface_cmv.component_id.id().string(),
                                    ") but no line, with several component "
                                    "mesh vertices along the surface." );
                            }
                            return false;
                        }
                    }
                }
                if( line_cmvs.empty() )
                {
                    if( line_cmvs.empty() && internal_surfaces.size() == 1
                        && block_cmvs.size() != 2 )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index, " is part of one block (",
                                block_uuids.front().string(),
                                "), and one internal surface (",
                                internal_surfaces.begin()->string(),
                                ") but no line, with ", block_cmvs.size(),
                                " component mesh vertices in the block (should "
                                "be 2)." );
                        }
                        return false;
                    }
                    return true;
                }
                if( brep_.has_component_mesh_vertices( unique_vertex_index,
                        Corner3D::component_type_static() ) )
                {
                    // Skip since it requires to implement a local radial sort
                    // to count number of block components
                    return true;
                }
                absl::flat_hash_set< uuid > incident_surfaces_of_free_lines;
                bool inside_free_line{ false };
                for( const auto& line_cmv : line_cmvs )
                {
                    const auto& line = brep_.line( line_cmv.component_id.id() );
                    if( brep_.nb_incidences( line_cmv.component_id.id() ) == 1
                        && brep_.nb_embedding_surfaces( line ) == 0 )
                    {
                        inside_free_line = true;
                        for( const auto& line_incidence :
                            brep_.incidences( line ) )
                        {
                            incident_surfaces_of_free_lines.emplace(
                                line_incidence.id() );
                        }
                    }
                }
                if( !inside_free_line )
                {
                    // Skip since it requires to implement a local radial sort
                    // to count number of block components
                    return true;
                }
                if( block_cmvs.size() != 1 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index, " is part of one block (",
                            block_uuids.front().string(),
                            "), one internal surfaces, and on a free line, "
                            "with ",
                            block_cmvs.size(),
                            " component mesh vertices in the block (should "
                            "be 1)." );
                    }
                    return false;
                }
                return true;
            }
            if( block_uuids.size() == 2 )
            {
                for( const auto& surface :
                    brep_.component_mesh_vertices( unique_vertex_index,
                        Surface3D::component_type_static() ) )
                {
                    if( brep_.Relationships::is_boundary(
                            surface.component_id.id(), block_uuids[0] )
                        && brep_.Relationships::is_boundary(
                            surface.component_id.id(), block_uuids[1] ) )
                    {
                        return true;
                    }
                    for( const auto& line :
                        brep_.component_mesh_vertices( unique_vertex_index,
                            Line3D::component_type_static() ) )
                    {
                        if( brep_.Relationships::is_boundary(
                                line.component_id.id(),
                                surface.component_id.id() )
                            && ( brep_.Relationships::is_boundary(
                                     surface.component_id.id(), block_uuids[0] )
                                 || brep_.Relationships::is_boundary(
                                     surface.component_id.id(),
                                     block_uuids[1] ) ) )
                        {
                            return true;
                        }
                    }
                }
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of two blocks, but not of a surface boundary "
                        "to the two blocks, nor of a line boundary to one "
                        "of the blocks incident surfaces." );
                }
                return false;
            }
            return true;
        }
    } // namespace detail
} // namespace geode
