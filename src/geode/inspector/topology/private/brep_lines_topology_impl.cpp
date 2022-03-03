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

#include <geode/inspector/topology/private/brep_lines_topology_impl.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    namespace detail
    {
        BRepLinesTopologyImpl::BRepLinesTopologyImpl( const BRep& brep )
            : brep_( brep )
        {
        }

        bool BRepLinesTopologyImpl::brep_vertex_lines_topology_is_valid(
            index_t unique_vertex_index ) const
        {
            const auto lines = brep_.mesh_component_vertices(
                unique_vertex_index, Line3D::component_type_static() );
            if( lines.empty() )
            {
                return true;
            }
            if( vertex_is_part_of_not_boundary_nor_internal_line(
                    unique_vertex_index )
                || vertex_is_part_of_line_with_invalid_internal_topology(
                    unique_vertex_index )
                || vertex_is_part_of_invalid_unique_line( unique_vertex_index )
                || vertex_has_lines_but_is_not_corner( unique_vertex_index ) )
            {
                return false;
            }
            return true;
        }

        bool BRepLinesTopologyImpl::
            vertex_is_part_of_not_boundary_nor_internal_line(
                const index_t unique_vertex_index ) const
        {
            for( const auto line : brep_.mesh_component_vertices(
                     unique_vertex_index, Line3D::component_type_static() ) )
            {
                if( brep_.nb_embeddings( line.component_id.id() ) < 1
                    && brep_.nb_incidences( line.component_id.id() ) < 1 )
                {
                    return true;
                }
            }
            return false;
        }

        bool BRepLinesTopologyImpl::
            vertex_is_part_of_line_with_invalid_internal_topology(
                const index_t unique_vertex_index ) const
        {
            for( const auto line : brep_.mesh_component_vertices(
                     unique_vertex_index, Line3D::component_type_static() ) )
            {
                if( brep_.nb_embeddings( line.component_id.id() ) < 1 )
                {
                    return false;
                }
                else if( brep_.nb_embeddings( line.component_id.id() ) > 1
                         || brep_.nb_incidences( line.component_id.id() ) > 0 )
                {
                    return true;
                }
            }
            return false;
        }

        bool BRepLinesTopologyImpl::vertex_is_part_of_invalid_unique_line(
            index_t unique_vertex_index ) const
        {
            const auto lines = brep_.mesh_component_vertices(
                unique_vertex_index, Line3D::component_type_static() );
            if( lines.size() != 1 )
            {
                return false;
            }
            const auto surfaces = brep_.mesh_component_vertices(
                unique_vertex_index, Surface3D::component_type_static() );
            const auto blocks = brep_.mesh_component_vertices(
                unique_vertex_index, Block3D::component_type_static() );
            if( surfaces.size() == 1 )
            {
                if( !brep_.Relationships::is_internal(
                        lines[0].component_id.id(),
                        surfaces[0].component_id.id() ) )
                {
                    return true;
                }
            }
            else if( blocks.size() == 1 )
            {
                if( !brep_.Relationships::is_internal(
                        lines[0].component_id.id(),
                        blocks[0].component_id.id() )
                    || !surfaces.empty() )
                {
                    return true;
                }
            }
            else if( surfaces.size() > 1 )
            {
                for( const auto& surface : surfaces )
                {
                    if( !brep_.Relationships::is_boundary(
                            lines[0].component_id.id(),
                            surface.component_id.id() ) )
                    {
                        return true;
                    }
                }
            }
            else
            {
                return true;
            }
            return false;
        }

        bool BRepLinesTopologyImpl::vertex_has_lines_but_is_not_corner(
            index_t unique_vertex_index ) const
        {
            return brep_.mesh_component_vertices( unique_vertex_index,
                            Line3D::component_type_static() )
                           .size()
                       > 1
                   && brep_
                          .mesh_component_vertices( unique_vertex_index,
                              Corner3D::component_type_static() )
                          .empty();
        }
    } // namespace detail
} // namespace geode
