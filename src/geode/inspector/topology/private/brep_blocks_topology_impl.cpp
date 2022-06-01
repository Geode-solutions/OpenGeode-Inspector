/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    std::vector< geode::uuid > brep_vertex_component_uuids(
        const geode::BRep& brep,
        geode::index_t unique_vertex_index,
        const geode::ComponentType& component_type )
    {
        const auto components =
            brep.mesh_component_vertices( unique_vertex_index, component_type );
        std::vector< geode::uuid > component_uuids;
        component_uuids.reserve( components.size() );
        for( const auto& mcv : components )
        {
            component_uuids.push_back( mcv.component_id.id() );
        }
        geode::sort_unique( component_uuids );
        return component_uuids;
    }
} // namespace

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
            const auto block_uuids = brep_vertex_component_uuids(
                brep_, unique_vertex_index, Block3D::component_type_static() );
            if( block_uuids.size() == 2 )
            {
                for( const auto surface :
                    brep_.mesh_component_vertices( unique_vertex_index,
                        Surface3D::component_type_static() ) )
                {
                    if( brep_.Relationships::is_boundary(
                            surface.component_id.id(), block_uuids[0] )
                        && brep_.Relationships::is_boundary(
                            surface.component_id.id(), block_uuids[1] ) )
                    {
                        return true;
                    }
                }
                for( const auto line :
                    brep_.mesh_component_vertices(
                        unique_vertex_index, Line3D::component_type_static() ) )
                {
                    for( const auto surface :
                        brep_.mesh_component_vertices( unique_vertex_index,
                            Surface3D::component_type_static() ) )
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
