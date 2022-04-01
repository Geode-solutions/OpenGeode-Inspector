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

#include <geode/inspector/topology/private/section_surfaces_topology_impl.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    namespace detail
    {
        SectionSurfacesTopologyImpl::SectionSurfacesTopologyImpl(
            const Section& section )
            : section_( section )
        {
        }

        bool SectionSurfacesTopologyImpl::
            section_vertex_surfaces_topology_is_valid(
                index_t unique_vertex_index ) const
        {
            const auto surfaces = section_.mesh_component_vertices(
                unique_vertex_index, Surface2D::component_type_static() );
            if( surfaces.size() == 2 )
            {
                for( const auto line :
                    section_.mesh_component_vertices(
                        unique_vertex_index, Line2D::component_type_static() ) )
                {
                    if( section_.Relationships::is_boundary(
                            line.component_id.id(),
                            surfaces[0].component_id.id() )
                        && section_.Relationships::is_boundary(
                            line.component_id.id(),
                            surfaces[1].component_id.id() ) )
                    {
                        return true;
                    }
                }
                return false;
            }
            return true;
        }
    } // namespace detail
} // namespace geode
