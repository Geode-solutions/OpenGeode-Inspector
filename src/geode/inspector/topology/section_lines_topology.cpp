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

#include <geode/inspector/topology/section_lines_topology.h>

#include <absl/algorithm/container.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/topology/private/topology_helpers.h>

namespace geode
{
    SectionLinesTopology::SectionLinesTopology( const Section& section )
        : section_( section )
    {
    }

    bool SectionLinesTopology::section_lines_topology_is_valid(
        index_t unique_vertex_index ) const
    {
        const auto lines = section_.component_mesh_vertices(
            unique_vertex_index, Line2D::component_type_static() );
        if( lines.empty() )
        {
            return true;
        }
        if( vertex_is_part_of_not_internal_nor_boundary_line(
                unique_vertex_index )
            || vertex_is_part_of_invalid_embedded_line( unique_vertex_index )
            || vertex_is_part_of_invalid_single_line( unique_vertex_index )
            || vertex_has_lines_but_is_not_a_corner( unique_vertex_index ) )
        {
            return false;
        }
        return true;
    }

    absl::optional< std::string >
        SectionLinesTopology::vertex_is_part_of_not_internal_nor_boundary_line(
            const index_t unique_vertex_index ) const
    {
        for( const auto& line : section_.component_mesh_vertices(
                 unique_vertex_index, Line2D::component_type_static() ) )
        {
            if( section_.nb_embeddings( line.component_id.id() ) < 1
                && section_.nb_incidences( line.component_id.id() ) < 1 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of line with uuid '",
                    line.component_id.id().string(),
                    "', which is neither embedded nor incident." );
            }
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        SectionLinesTopology::vertex_is_part_of_invalid_embedded_line(
            const index_t unique_vertex_index ) const
    {
        for( const auto& line : section_.component_mesh_vertices(
                 unique_vertex_index, Line2D::component_type_static() ) )
        {
            if( section_.nb_embeddings( line.component_id.id() ) < 1 )
            {
                return absl::nullopt;
            }
            else if( section_.nb_embeddings( line.component_id.id() ) > 1 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of line with uuid '",
                    line.component_id.id().string(),
                    "', which has multiple embeddings." );
            }
            else if( section_.nb_incidences( line.component_id.id() ) > 0 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of line with uuid '",
                    line.component_id.id().string(),
                    "', which has both an embedding and "
                    "incidence(s)." );
            }
            for( const auto& embedding :
                section_.embeddings( line.component_id.id() ) )
            {
                if( detail::section_surfaces_are_meshed( section_ )
                    && !absl::c_any_of(
                        section_.component_mesh_vertices( unique_vertex_index,
                            Surface2D::component_type_static() ),
                        [&embedding]( const ComponentMeshVertex& cmv ) {
                            return cmv.component_id.id() == embedding.id();
                        } ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index, " is part of line with uuid '",
                        line.component_id.string(),
                        "', which is embedded in surface with uuid '",
                        embedding.id().string(),
                        "', but the unique vertex is not linked to the "
                        "surface mesh vertices." );
                }
            }
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        SectionLinesTopology::vertex_is_part_of_invalid_single_line(
            index_t unique_vertex_index ) const
    {
        const auto line_uuids =
            detail::components_uuids( section_.component_mesh_vertices(
                unique_vertex_index, Line2D::component_type_static() ) );
        if( line_uuids.size() != 1 )
        {
            return absl::nullopt;
        }
        const auto& line_id = line_uuids[0];
        const auto surface_uuids =
            detail::components_uuids( section_.component_mesh_vertices(
                unique_vertex_index, Surface2D::component_type_static() ) );
        if( surface_uuids.size() > 2 )
        {
            return absl::StrCat( "Unique vertex with index ",
                unique_vertex_index, " is part of only one line, with uuid '",
                line_id.string(), "', but part of more than two surfaces." );
        }
        if( section_.nb_embeddings( line_id ) > 0 )
        {
            if( detail::section_surfaces_are_meshed( section_ )
                && ( surface_uuids.size() != 1
                     || !section_.Relationships::is_internal(
                         line_id, surface_uuids[0] ) ) )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index,
                    " is part of only one line, with uuid '", line_id.string(),
                    "', which has embeddings, but there are more than "
                    "one meshed surface associated to the vertex, or "
                    "the line is not internal to the meshed surface "
                    "associated to the vertex." );
            }
        }
        else
        {
            for( const auto& surface_id : surface_uuids )
            {
                if( !section_.Relationships::is_boundary(
                        line_id, surface_id ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one line, with uuid '",
                        line_id.string(),
                        "', and mutiple surfaces, but the line is not "
                        "boundary of associated surface with uuid'",
                        surface_id.string(), "'." );
                }
            }
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        SectionLinesTopology::vertex_has_lines_but_is_not_a_corner(
            index_t unique_vertex_index ) const
    {
        if( section_.component_mesh_vertices(
                        unique_vertex_index, Line2D::component_type_static() )
                    .size()
                > 1
            && section_
                   .component_mesh_vertices(
                       unique_vertex_index, Corner2D::component_type_static() )
                   .empty() )
        {
            return absl::StrCat( "Unique vertex with index ",
                unique_vertex_index,
                " is part of multiple lines but is not a corner." );
        }
        return absl::nullopt;
    }

    SectionLinesInspectionResult
        SectionLinesTopology::inspect_lines_topology() const
    {
        SectionLinesInspectionResult result;
        for( const auto& line : section_.lines() )
        {
            if( section_.line( line.id() ).mesh().nb_vertices() == 0 )
            {
                result.lines_not_meshed.add_problem(
                    line.id(), absl::StrCat( line.id().string(),
                                   " is a line without mesh." ) );
            }
            auto line_result = detail::
                section_component_vertices_are_associated_to_unique_vertices(
                    section_, line.component_id(), line.mesh() );
            result.lines_not_linked_to_a_unique_vertex.emplace_back( line.id(),
                absl::StrCat( "Line ", line.id().string(),
                    " has mesh vertices not linked to a unique vertex." ) );
            result.lines_not_linked_to_a_unique_vertex.back().second.problems =
                std::move( line_result.first );
            result.lines_not_linked_to_a_unique_vertex.back().second.messages =
                std::move( line_result.second );
        }
        for( const auto unique_vertex_id :
            Range{ section_.nb_unique_vertices() } )
        {
            if( const auto boundary_nor_internal_line =
                    vertex_is_part_of_not_internal_nor_boundary_line(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_not_internal_nor_boundary_line
                    .add_problem(
                        unique_vertex_id, boundary_nor_internal_line.value() );
            }
            if( const auto invalid_internal_topology =
                    vertex_is_part_of_invalid_embedded_line(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_a_line_with_invalid_embeddings
                    .add_problem(
                        unique_vertex_id, invalid_internal_topology.value() );
            }
            if( const auto invalid_unique_line =
                    vertex_is_part_of_invalid_single_line( unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_a_single_and_invalid_line
                    .add_problem(
                        unique_vertex_id, invalid_unique_line.value() );
            }
            if( const auto lines_but_is_not_corner =
                    vertex_has_lines_but_is_not_a_corner( unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                    .add_problem(
                        unique_vertex_id, lines_but_is_not_corner.value() );
            }
        }
        return result;
    }

} // namespace geode
