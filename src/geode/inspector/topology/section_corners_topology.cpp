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

#include <geode/inspector/topology/private/topology_helpers.h>
#include <geode/inspector/topology/section_corners_topology.h>

#include <geode/mesh/core/point_set.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    std::string SectionCornersTopologyInspectionResult::string() const
    {
        auto message = absl::StrCat( corners_not_meshed.string(), "\n" );
        for( const auto& corner_uv_issue :
            corners_not_linked_to_a_unique_vertex )
        {
            absl::StrAppend( &message, corner_uv_issue.second.string(), "\n" );
        }
        absl::StrAppend( &message,
            unique_vertices_linked_to_multiple_corners.string(), "\n" );
        absl::StrAppend( &message,
            unique_vertices_linked_to_multiple_internals_corner.string(),
            "\n" );
        absl::StrAppend( &message,
            unique_vertices_linked_to_not_internal_nor_boundary_corner.string(),
            "\n" );
        absl::StrAppend( &message,
            unique_vertices_liked_to_not_boundary_line_corner.string(), "\n" );
        return message;
    }

    SectionCornersTopology::SectionCornersTopology( const Section& section )
        : section_( section )
    {
    }

    bool SectionCornersTopology::section_corner_topology_is_valid(
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

    absl::optional< std::string >
        SectionCornersTopology::unique_vertex_has_multiple_corners(
            index_t unique_vertex_index ) const
    {
        if( section_
                .component_mesh_vertices(
                    unique_vertex_index, Corner2D::component_type_static() )
                .size()
            > 1 )
        {
            return absl::StrCat( "Unique vertex with index ",
                unique_vertex_index, " is part of several corners." );
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        SectionCornersTopology::corner_has_multiple_embeddings(
            index_t unique_vertex_index ) const
    {
        const auto corners = section_.component_mesh_vertices(
            unique_vertex_index, Corner2D::component_type_static() );
        if( !corners.empty()
            && section_.nb_embeddings( corners[0].component_id.id() ) > 1 )
        {
            return absl::StrCat( "Unique vertex with index ",
                unique_vertex_index, " is associated to corner with uuid '",
                corners[0].component_id.id().string(),
                "', which has several embeddings." );
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        SectionCornersTopology::corner_is_not_internal_nor_boundary(
            index_t unique_vertex_index ) const
    {
        const auto corners = section_.component_mesh_vertices(
            unique_vertex_index, Corner2D::component_type_static() );
        if( !corners.empty()
            && section_.nb_embeddings( corners[0].component_id.id() ) < 1
            && section_.nb_incidences( corners[0].component_id.id() ) < 1 )
        {
            return absl::StrCat( "Unique vertex with index ",
                unique_vertex_index, " is associated to corner with uuid '",
                corners[0].component_id.id().string(),
                "', which is neither internal nor boundary." );
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        SectionCornersTopology::corner_is_part_of_line_but_not_boundary(
            index_t unique_vertex_index ) const
    {
        const auto corners = section_.component_mesh_vertices(
            unique_vertex_index, Corner2D::component_type_static() );
        if( !corners.empty() )
        {
            const auto& corner_uuid = corners[0].component_id.id();
            for( const auto& line : section_.component_mesh_vertices(
                     unique_vertex_index, Line2D::component_type_static() ) )
            {
                if( !section_.Relationships::is_boundary(
                        corner_uuid, line.component_id.id() ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is associated with corner with uuid '",
                        corner_uuid.string(), "', part of line with uuid '",
                        line.component_id.id().string(),
                        "', but is not a boundary of the line." );
                }
            }
        }
        return absl::nullopt;
    }

    SectionCornersTopologyInspectionResult
        SectionCornersTopology::inspect_corners_topology() const
    {
        SectionCornersTopologyInspectionResult result;
        for( const auto& corner : section_.corners() )
        {
            if( section_.corner( corner.id() ).mesh().nb_vertices() == 0 )
            {
                result.corners_not_meshed.add_problem( corner.id(),
                    "Corner " + corner.id().string() + " is not meshed." );
                continue;
            }
            auto corner_result = detail::
                section_component_vertices_are_associated_to_unique_vertices(
                    section_, corner.component_id(), corner.mesh() );
            corner_result.description =
                absl::StrCat( "Corner ", corner.id().string(),
                    " has mesh vertices not linked to a unique vertex." );
            result.corners_not_linked_to_a_unique_vertex.emplace_back(
                corner.id(), corner_result );
        }
        for( const auto unique_vertex_id :
            Range{ section_.nb_unique_vertices() } )
        {
            if( const auto problem_message =
                    unique_vertex_has_multiple_corners( unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_multiple_corners.add_problem(
                    unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    corner_has_multiple_embeddings( unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_multiple_internals_corner
                    .add_problem( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    corner_is_not_internal_nor_boundary( unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_not_internal_nor_boundary_corner
                    .add_problem( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    corner_is_part_of_line_but_not_boundary(
                        unique_vertex_id ) )
            {
                result.unique_vertices_liked_to_not_boundary_line_corner
                    .add_problem( unique_vertex_id, problem_message.value() );
            }
        }
        return result;
    }

} // namespace geode
