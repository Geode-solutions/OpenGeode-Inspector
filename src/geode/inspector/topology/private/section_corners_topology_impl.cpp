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

#include <geode/inspector/topology/private/section_corners_topology_impl.h>

#include <geode/basic/logger.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    namespace detail
    {
        SectionCornersTopologyImpl::SectionCornersTopologyImpl(
            const Section& section, bool verbose )
            : section_( section ), verbose_( verbose )
        {
        }
        SectionCornersTopologyImpl::SectionCornersTopologyImpl(
            const Section& section )
            : SectionCornersTopologyImpl( section, false )
        {
        }

        bool SectionCornersTopologyImpl::section_corner_topology_is_valid(
            index_t unique_vertex_index ) const
        {
            const auto corners = section_.component_mesh_vertices(
                unique_vertex_index, Corner2D::component_type_static() );
            if( corners.empty() )
            {
                return true;
            }
            if( corners.size() != 1 )
            {
                return false;
            }
            const auto& corner_uuid = corners[0].component_id.id();
            if( section_.nb_embeddings( corner_uuid ) > 1 )
            {
                return false;
            }
            else if( section_.nb_embeddings( corner_uuid ) != 1 )
            {
                if( section_.nb_incidences( corner_uuid ) < 1 )
                {
                    return false;
                }
            }
            else if( section_.nb_incidences( corner_uuid ) > 1 )
            {
                return false;
            }
            for( const auto& line : section_.component_mesh_vertices(
                     unique_vertex_index, Line2D::component_type_static() ) )
            {
                if( !section_.Relationships::is_boundary(
                        corner_uuid, line.component_id.id() ) )
                {
                    return false;
                }
            }
            return true;
        }

        bool SectionCornersTopologyImpl::unique_vertex_has_multiple_corners(
            index_t unique_vertex_index ) const
        {
            if( section_
                    .component_mesh_vertices(
                        unique_vertex_index, Corner2D::component_type_static() )
                    .size()
                > 1 )
            {
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is associated to multiple corners." );
                }
                return true;
            }
            return false;
        }

        bool SectionCornersTopologyImpl::corner_has_multiple_embeddings(
            index_t unique_vertex_index ) const
        {
            const auto corners = section_.component_mesh_vertices(
                unique_vertex_index, Corner2D::component_type_static() );
            if( !corners.empty()
                && section_.nb_embeddings( corners[0].component_id.id() ) > 1 )
            {
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is associated to corner with uuid '",
                        corners[0].component_id.id().string(),
                        "', which has multiple embeddings." );
                }
                return true;
            }
            return false;
        }

        bool SectionCornersTopologyImpl::corner_is_not_internal_nor_boundary(
            index_t unique_vertex_index ) const
        {
            const auto corners = section_.component_mesh_vertices(
                unique_vertex_index, Corner2D::component_type_static() );
            if( !corners.empty()
                && section_.nb_embeddings( corners[0].component_id.id() ) < 1
                && section_.nb_incidences( corners[0].component_id.id() ) < 1 )
            {
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is associated to corner with uuid '",
                        corners[0].component_id.id().string(),
                        "', which is neither boundary nor internal." );
                }
                return true;
            }
            return false;
        }

        bool
            SectionCornersTopologyImpl::corner_is_part_of_line_but_not_boundary(
                index_t unique_vertex_index ) const
        {
            const auto corners = section_.component_mesh_vertices(
                unique_vertex_index, Corner2D::component_type_static() );
            if( !corners.empty() )
            {
                const auto& corner_uuid = corners[0].component_id.id();
                for( const auto& line :
                    section_.component_mesh_vertices(
                        unique_vertex_index, Line2D::component_type_static() ) )
                {
                    if( !section_.Relationships::is_boundary(
                            corner_uuid, line.component_id.id() ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is associated to corner with uuid '",
                                corner_uuid.string(), "' and line '",
                                line.component_id.id().string(),
                                "' but the corner is not boundary of the "
                                "line." );
                        }
                        return true;
                    }
                }
            }
            return false;
        }
    } // namespace detail
} // namespace geode
