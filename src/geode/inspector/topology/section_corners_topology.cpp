/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <optional>

#include <geode/inspector/topology/internal/topology_helpers.hpp>
#include <geode/inspector/topology/section_corners_topology.hpp>

#include <geode/mesh/core/point_set.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    index_t SectionCornersTopologyInspectionResult::nb_issues() const
    {
        return corners_not_meshed.nb_issues()
               + corners_not_linked_to_a_unique_vertex.nb_issues()
               + unique_vertices_linked_to_multiple_corners.nb_issues()
               + unique_vertices_linked_to_multiple_internals_corner.nb_issues()
               + unique_vertices_linked_to_not_internal_nor_boundary_corner
                     .nb_issues()
               + unique_vertices_linked_to_not_boundary_line_corner.nb_issues();
    }

    std::string SectionCornersTopologyInspectionResult::string() const
    {
        std::string message;
        if( corners_not_meshed.nb_issues() != 0 )
        {
            absl::StrAppend( &message, corners_not_meshed.string() );
        }
        if( corners_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, corners_not_linked_to_a_unique_vertex.string() );
        }
        if( unique_vertices_linked_to_multiple_corners.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, unique_vertices_linked_to_multiple_corners.string() );
        }
        if( unique_vertices_linked_to_multiple_internals_corner.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_multiple_internals_corner.string() );
        }
        if( unique_vertices_linked_to_not_internal_nor_boundary_corner
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_not_internal_nor_boundary_corner
                    .string() );
        }
        if( unique_vertices_linked_to_not_boundary_line_corner.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_not_boundary_line_corner.string() );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "No issues with corners topology \n";
    }

    std::string SectionCornersTopologyInspectionResult::inspection_type() const
    {
        return "Corners topology inspection";
    }

    SectionCornersTopology::SectionCornersTopology( const Section& section )
        : section_( section )
    {
    }

    bool SectionCornersTopology::section_corner_topology_is_valid(
        index_t unique_vertex_index ) const
    {
        bool corner_found{ false };
        for( const auto& cmv :
            section_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() != Corner2D::component_type_static() )
            {
                continue;
            }
            if( corner_found )
            {
                return false;
            }
            corner_found = true;
            const auto& corner_uuid = cmv.component_id.id();
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
            for( const auto& line :
                section_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( line.component_id.type()
                    != Line2D::component_type_static() )
                {
                    continue;
                }
                if( !section_.Relationships::is_boundary(
                        corner_uuid, line.component_id.id() ) )
                {
                    return false;
                }
            }
        }
        return true;
    }

    bool SectionCornersTopology::corner_is_meshed(
        const Corner2D& corner ) const
    {
        return corner.mesh().nb_vertices() != 0;
    }

    bool SectionCornersTopology::
        corner_vertices_are_associated_to_unique_vertices(
            const Corner2D& corner ) const
    {
        return internal::
            model_component_vertices_are_associated_to_unique_vertices(
                section_, corner.component_id(), corner.mesh() );
    }

    std::optional< std::string >
        SectionCornersTopology::unique_vertex_has_multiple_corners(
            index_t unique_vertex_index ) const
    {
        bool corner_found{ false };
        for( const auto& cmv :
            section_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Corner2D::component_type_static() )
            {
                if( corner_found )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index, " is part of several corners." );
                }
                corner_found = true;
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        SectionCornersTopology::corner_has_multiple_embeddings(
            index_t unique_vertex_index ) const
    {
        for( const auto& cmv :
            section_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Corner2D::component_type_static()
                && section_.nb_embeddings( cmv.component_id.id() ) > 1 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is associated to corner with uuid '",
                    cmv.component_id.id().string(),
                    "', which has several embeddings." );
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        SectionCornersTopology::corner_is_not_internal_nor_boundary(
            index_t unique_vertex_index ) const
    {
        for( const auto& cmv :
            section_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Corner2D::component_type_static()
                && section_.nb_embeddings( cmv.component_id.id() ) < 1
                && section_.nb_incidences( cmv.component_id.id() ) < 1 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is associated to corner with uuid '",
                    cmv.component_id.id().string(),
                    "', which is neither internal nor boundary." );
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        SectionCornersTopology::corner_is_part_of_line_but_not_boundary(
            index_t unique_vertex_index ) const
    {
        for( const auto& cmv :
            section_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() != Corner2D::component_type_static() )
            {
                continue;
            }
            const auto& corner_uuid = cmv.component_id.id();
            for( const auto& line :
                section_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( line.component_id.type()
                    != Line2D::component_type_static() )
                {
                    continue;
                }
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
        return std::nullopt;
    }

    SectionCornersTopologyInspectionResult
        SectionCornersTopology::inspect_corners_topology() const
    {
        SectionCornersTopologyInspectionResult result;
        for( const auto& corner : section_.corners() )
        {
            if( !corner_is_meshed( section_.corner( corner.id() ) ) )
            {
                result.corners_not_meshed.add_issue( corner.id(),
                    "Corner " + corner.id().string() + " is not meshed." );
                continue;
            }
            auto corner_result = internal::
                model_component_vertices_not_associated_to_unique_vertices(
                    section_, corner.component_id(), corner.mesh() );
            if( corner_result.nb_issues() != 0 )
            {
                corner_result.set_description(
                    absl::StrCat( "Corner ", corner.id().string() ) );
                result.corners_not_linked_to_a_unique_vertex.add_issues_to_map(
                    corner.id(), std::move( corner_result ) );
            }
        }
        for( const auto unique_vertex_id :
            Range{ section_.nb_unique_vertices() } )
        {
            if( const auto problem_message =
                    unique_vertex_has_multiple_corners( unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_multiple_corners.add_issue(
                    unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    corner_has_multiple_embeddings( unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_multiple_internals_corner
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    corner_is_not_internal_nor_boundary( unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_not_internal_nor_boundary_corner
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    corner_is_part_of_line_but_not_boundary(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_not_boundary_line_corner
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
        }
        return result;
    }

} // namespace geode
