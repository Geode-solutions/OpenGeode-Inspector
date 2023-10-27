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

#include <geode/inspector/topology/brep_corners_topology.h>
#include <geode/inspector/topology/private/topology_helpers.h>

#include <geode/mesh/core/point_set.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    BRepCornersTopology::BRepCornersTopology( const BRep& brep ) : brep_{ brep }
    {
    }

    bool BRepCornersTopology::brep_corner_topology_is_valid(
        index_t unique_vertex_index ) const
    {
        const auto corners = brep_.component_mesh_vertices(
            unique_vertex_index, Corner3D::component_type_static() );
        if( corners.empty() )
        {
            return true;
        }
        if( corners.size() != 1 )
        {
            return false;
        }
        const auto& corner_uuid = corners[0].component_id.id();
        if( brep_.nb_embeddings( corner_uuid ) > 1 )
        {
            return false;
        }
        if( brep_.nb_embeddings( corner_uuid ) != 1 )
        {
            if( brep_.nb_incidences( corner_uuid ) < 1 )
            {
                return false;
            }
        }
        else if( brep_.nb_incidences( corner_uuid ) > 1 )
        {
            return false;
        }
        if( corner_is_part_of_line_but_not_boundary( unique_vertex_index ) )
        {
            return false;
        }
        return true;
    }

    absl::optional< std::string >
        BRepCornersTopology::unique_vertex_has_multiple_corners(
            index_t unique_vertex_index ) const
    {
        if( brep_
                .component_mesh_vertices(
                    unique_vertex_index, Corner3D::component_type_static() )
                .size()
            > 1 )
        {
            return "Unique vertex with index "
                   + absl::StrCat( unique_vertex_index )
                   + " is part of several corners.";
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        BRepCornersTopology::corner_has_multiple_embeddings(
            index_t unique_vertex_index ) const
    {
        const auto corners = brep_.component_mesh_vertices(
            unique_vertex_index, Corner3D::component_type_static() );
        if( !corners.empty()
            && brep_.nb_embeddings( corners[0].component_id.id() ) > 1 )
        {
            return "Unique vertex with index "
                   + absl::StrCat( unique_vertex_index )
                   + " is associated to corner with uuid '"
                   + corners[0].component_id.id().string()
                   + "', which has several embeddings.";
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        BRepCornersTopology::corner_is_not_internal_nor_boundary(
            index_t unique_vertex_index ) const
    {
        const auto corners = brep_.component_mesh_vertices(
            unique_vertex_index, Corner3D::component_type_static() );
        if( !corners.empty()
            && brep_.nb_embeddings( corners[0].component_id.id() ) < 1
            && brep_.nb_incidences( corners[0].component_id.id() ) < 1 )
        {
            return "Unique vertex with index "
                   + absl::StrCat( unique_vertex_index )
                   + " is associated to corner with uuid '"
                   + corners[0].component_id.id().string()
                   + "', which is neither internal nor boundary.";
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        BRepCornersTopology::corner_is_part_of_line_but_not_boundary(
            index_t unique_vertex_index ) const
    {
        for( const auto cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() != Corner3D::component_type_static() )
            {
                continue;
            }
            const auto& corner_uuid = cmv.component_id.id();
            for( const auto& line : brep_.component_mesh_vertices(
                     unique_vertex_index, Line3D::component_type_static() ) )
            {
                if( brep_.Relationships::is_boundary(
                        corner_uuid, line.component_id.id() ) )
                {
                    continue;
                }
                if( brep_.Relationships::is_internal(
                        corner_uuid, line.component_id.id() ) )
                {
                    index_t line_vertex_count{ 0 };
                    for( const auto cmv2 :
                        brep_.component_mesh_vertices( unique_vertex_index ) )
                    {
                        if( cmv2.component_id.id() == corner_uuid )
                        {
                            line_vertex_count++;
                        }
                    }
                    if( line_vertex_count != 2 )
                    {
                        return "Unique vertex with index "
                               + absl::StrCat( unique_vertex_index )
                               + " is associated with corner with uuid '"
                               + corner_uuid.string()
                               + "', which is internal to line with uuid '"
                               + line.component_id.id().string()
                               + "', so line should be closed and have two "
                                 "different vertices on unique vertex, but has "
                               + absl::StrCat( line_vertex_count )
                               + " vertices on it instead.";
                    }
                    continue;
                }
                return "Unique vertex with index "
                       + absl::StrCat( unique_vertex_index )
                       + " is associated with corner with uuid '"
                       + corner_uuid.string() + "', part of line with uuid '"
                       + line.component_id.id().string()
                       + "', but is neither boundary nor internal of it.";
            }
        }
        return absl::nullopt;
    }

    BRepCornersTopologyInspectionResult
        BRepCornersTopology::inspect_corners_topology() const
    {
        BRepCornersTopologyInspectionResult result;
        for( const auto& corner : brep_.corners() )
        {
            if( brep_.corner( corner.id() ).mesh().nb_vertices() == 0 )
            {
                result.corners_not_meshed.add_problem( corner.id(),
                    "Corner " + corner.id().string() + " is not meshed." );
                continue;
            }
            auto corner_result = detail::
                brep_component_vertices_not_associated_to_unique_vertices(
                    brep_, corner.component_id(), corner.mesh() );
            result.corners_not_linked_to_a_unique_vertex.emplace_back(
                corner.id(),
                "Corner " + corner.id().string()
                    + " has mesh vertices not linked to a unique vertex." );
            result.corners_not_linked_to_a_unique_vertex.back()
                .second.problems = std::move( corner_result.first );
            result.corners_not_linked_to_a_unique_vertex.back()
                .second.messages = std::move( corner_result.second );
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
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
