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

#include <geode/inspector/topology/private/section_surfaces_topology_impl.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/topology/private/topology_helpers.h>

namespace geode
{
    namespace detail
    {
        SectionSurfacesTopologyImpl::SectionSurfacesTopologyImpl(
            const Section& section, bool verbose )
            : section_( section ), verbose_( verbose )
        {
        }
        SectionSurfacesTopologyImpl::SectionSurfacesTopologyImpl(
            const Section& section )
            : SectionSurfacesTopologyImpl( section, false )
        {
        }

        bool SectionSurfacesTopologyImpl::
            section_vertex_surfaces_topology_is_valid(
                index_t unique_vertex_index ) const
        {
            if( section_
                    .component_mesh_vertices( unique_vertex_index,
                        Surface2D::component_type_static() )
                    .empty() )
            {
                return true;
            }
            if( vertex_is_part_of_invalid_surfaces_topology(
                    unique_vertex_index )
                || vertex_is_part_of_line_and_not_on_surface_border(
                    unique_vertex_index ) )
            {
                return false;
            }
            return true;
        }

        bool SectionSurfacesTopologyImpl::
            vertex_is_part_of_invalid_surfaces_topology(
                index_t unique_vertex_index ) const
        {
            const auto surface_uuids =
                components_uuids( section_.component_mesh_vertices(
                    unique_vertex_index, Surface2D::component_type_static() ) );
            if( surface_uuids.size() == 2 )
            {
                for( const auto& line :
                    section_.component_mesh_vertices(
                        unique_vertex_index, Line2D::component_type_static() ) )
                {
                    if( section_.Relationships::is_boundary(
                            line.component_id.id(), surface_uuids[0] )
                        && section_.Relationships::is_boundary(
                            line.component_id.id(), surface_uuids[1] ) )
                    {
                        return false;
                    }
                }
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of two surfaces, but is associated to no "
                        "line boundary of the two surfaces." );
                }
                return true;
            }
            return false;
        }

        bool SectionSurfacesTopologyImpl::
            vertex_is_part_of_line_and_not_on_surface_border(
                index_t unique_vertex_index ) const
        {
            if( !detail::section_surfaces_are_meshed( section_ ) )
            {
                return false;
            }
            if( section_
                    .component_mesh_vertices(
                        unique_vertex_index, Line2D::component_type_static() )
                    .empty() )
            {
                return false;
            }
            for( const auto& surface_vertex : section_.component_mesh_vertices(
                     unique_vertex_index, Surface2D::component_type_static() ) )
            {
                if( !section_.surface( surface_vertex.component_id.id() )
                         .mesh()
                         .is_vertex_on_border( surface_vertex.vertex ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of a line and of surface with uuid '",
                            surface_vertex.component_id.id().string(),
                            "' but one of the associated vertex in the surface "
                            "mesh is not on the mesh border." );
                    }
                    return true;
                }
            }
            return false;
        }
    } // namespace detail
} // namespace geode
