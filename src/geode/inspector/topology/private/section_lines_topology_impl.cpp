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

#include <geode/inspector/topology/private/section_lines_topology_impl.h>

#include <absl/algorithm/container.h>

#include <geode/basic/logger.h>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/topology/private/topology_helpers.h>

namespace geode
{
    namespace detail
    {
        SectionLinesTopologyImpl::SectionLinesTopologyImpl(
            const Section& section, bool verbose )
            : section_( section ), verbose_( verbose )
        {
        }
        SectionLinesTopologyImpl::SectionLinesTopologyImpl(
            const Section& section )
            : SectionLinesTopologyImpl( section, false )
        {
        }

        bool SectionLinesTopologyImpl::section_vertex_lines_topology_is_valid(
            index_t unique_vertex_index ) const
        {
            const auto lines = section_.component_mesh_vertices(
                unique_vertex_index, Line2D::component_type_static() );
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

        bool SectionLinesTopologyImpl::
            vertex_is_part_of_not_boundary_nor_internal_line(
                const index_t unique_vertex_index ) const
        {
            for( const auto& line : section_.component_mesh_vertices(
                     unique_vertex_index, Line2D::component_type_static() ) )
            {
                if( section_.nb_embeddings( line.component_id.id() ) < 1
                    && section_.nb_incidences( line.component_id.id() ) < 1 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index, " is part of line with uuid '",
                            line.component_id.id().string(),
                            "', which has no embeddings and no incidences." );
                    }
                    return true;
                }
            }
            return false;
        }

        bool SectionLinesTopologyImpl::
            vertex_is_part_of_line_with_invalid_internal_topology(
                const index_t unique_vertex_index ) const
        {
            for( const auto& line : section_.component_mesh_vertices(
                     unique_vertex_index, Line2D::component_type_static() ) )
            {
                if( section_.nb_embeddings( line.component_id.id() ) < 1 )
                {
                    return false;
                }
                else if( section_.nb_embeddings( line.component_id.id() ) > 1 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index, " is part of line with uuid '",
                            line.component_id.id().string(),
                            "', which has multiple embeddings." );
                    }
                    return true;
                }
                else if( section_.nb_incidences( line.component_id.id() ) > 0 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index, " is part of line with uuid '",
                            line.component_id.id().string(),
                            "', which has both an embedding and "
                            "incidence(s)." );
                    }
                    return true;
                }
                for( const auto& embedding :
                    section_.embeddings( line.component_id.id() ) )
                {
                    if( section_surfaces_are_meshed( section_ )
                        && !absl::c_any_of(
                            section_.component_mesh_vertices(
                                unique_vertex_index,
                                Surface2D::component_type_static() ),
                            [&embedding]( const ComponentMeshVertex& cmv ) {
                                return cmv.component_id.id() == embedding.id();
                            } ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of line with uuid '",
                                line.component_id.string(),
                                "', which is embedded in surface with uuid '",
                                embedding.id().string(),
                                "', but the unique vertex is not linked to the "
                                "surface mesh vertices." );
                        }
                        return true;
                    }
                }
            }
            return false;
        }

        bool SectionLinesTopologyImpl::vertex_is_part_of_invalid_unique_line(
            index_t unique_vertex_index ) const
        {
            const auto line_uuids =
                components_uuids( section_.component_mesh_vertices(
                    unique_vertex_index, Line2D::component_type_static() ) );
            if( line_uuids.size() != 1 )
            {
                return false;
            }
            const auto& line_id = line_uuids[0];
            const auto surface_uuids =
                components_uuids( section_.component_mesh_vertices(
                    unique_vertex_index, Surface2D::component_type_static() ) );
            if( surface_uuids.size() > 2 )
            {
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one line, with uuid '",
                        line_id.string(),
                        "', but part of more than two surfaces." );
                }
                return true;
            }
            if( section_.nb_embeddings( line_id ) > 0 )
            {
                if( detail::section_surfaces_are_meshed( section_ )
                    && ( surface_uuids.size() != 1
                         || !section_.Relationships::is_internal(
                             line_id, surface_uuids[0] ) ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of only one line, with uuid '",
                            line_id.string(),
                            "', which has embeddings, but there are more than "
                            "one meshed surface associated to the vertex, or "
                            "the line is not internal to the meshed surface "
                            "associated to the vertex." );
                    }
                    return true;
                }
            }
            else
            {
                for( const auto& surface_id : surface_uuids )
                {
                    if( !section_.Relationships::is_boundary(
                            line_id, surface_id ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of line with uuid '",
                                line_id.string(),
                                "', and mutiple surfaces, but the line is not "
                                "boundary of associated surface with uuid '",
                                surface_id.string(), "'." );
                        }
                        return true;
                    }
                }
            }
            return false;
        }

        bool SectionLinesTopologyImpl::vertex_has_lines_but_is_not_corner(
            index_t unique_vertex_index ) const
        {
            if( section_.component_mesh_vertices( unique_vertex_index,
                            Line2D::component_type_static() )
                        .size()
                    > 1
                && section_
                       .component_mesh_vertices( unique_vertex_index,
                           Corner2D::component_type_static() )
                       .empty() )
            {
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is associated to multiple lines but no corner." );
                }
                return true;
            }
            return false;
        }
    } // namespace detail
} // namespace geode
