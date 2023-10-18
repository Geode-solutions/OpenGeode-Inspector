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

#include <geode/basic/logger.h>

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
        else if( brep_.nb_embeddings( corner_uuid ) != 1 )
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
        for( const auto& line : brep_.component_mesh_vertices(
                 unique_vertex_index, Line3D::component_type_static() ) )
        {
            if( !brep_.Relationships::is_boundary(
                    corner_uuid, line.component_id.id() ) )
            {
                return false;
            }
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
            return std::string( "Unique vertex with index " )
                   + std::to_string( unique_vertex_index )
                   + std::string( " is part of several corners." );
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
            return std::string( "Unique vertex with index " )
                   + std::to_string( unique_vertex_index )
                   + std::string( " is associated to corner with uuid '" )
                   + corners[0].component_id.id().string()
                   + std::string( "', which has several embeddings." );
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
            std::string( "Unique vertex with index " )
                + std::to_string( unique_vertex_index )
                + std::string( " is associated to corner with uuid '" )
                + corners[0].component_id.id().string()
                + std::string( "', which is neither incident nor embedded." );
        }
        return absl::nullopt;
    }

    absl::optional< std::string >
        BRepCornersTopology::corner_is_part_of_line_but_not_boundary(
            index_t unique_vertex_index ) const
    {
        const auto corners = brep_.component_mesh_vertices(
            unique_vertex_index, Corner3D::component_type_static() );
        if( !corners.empty() )
        {
            const auto& corner_uuid = corners[0].component_id.id();
            for( const auto& line : brep_.component_mesh_vertices(
                     unique_vertex_index, Line3D::component_type_static() ) )
            {
                if( !brep_.Relationships::is_boundary(
                        corner_uuid, line.component_id.id() ) )
                {
                    std::string( "Unique vertex with index " )
                        + std::to_string( unique_vertex_index )
                        + std::string(
                            " is associated with corner with uuid '" )
                        + corner_uuid.string()
                        + std::string( "', part of line with uuid '" )
                        + line.component_id.id().string()
                        + std::string( "', but not boundary of it." );
                }
            }
        }
        return absl::nullopt;
    }

    BRepCornersTopologyInspectionResult
        BRepCornersTopology::inspect_corners() const
    {
        BRepCornersTopologyInspectionResult result;
        for( const auto& corner : brep_.corners() )
        {
            auto corner_result = detail::
                brep_component_vertices_not_associated_to_unique_vertices(
                    brep_, corner.component_id(), corner.mesh() );
            result.corners_not_linked_to_unique_vertex.problems.insert(
                result.corners_not_linked_to_unique_vertex.problems.end(),
                std::make_move_iterator( corner_result.first.begin() ),
                std::make_move_iterator( corner_result.first.end() ) );
            result.corners_not_linked_to_unique_vertex.messages.insert(
                result.corners_not_linked_to_unique_vertex.messages.end(),
                std::make_move_iterator( corner_result.second.begin() ),
                std::make_move_iterator( corner_result.second.end() ) );
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            const auto has_multiple_corners =
                unique_vertex_has_multiple_corners( unique_vertex_id );
            if( has_multiple_corners )
            {
                result.multiple_corners_unique_vertices.add_problem(
                    unique_vertex_id, has_multiple_corners.value() );
            }
            const auto has_multiple_embeddings =
                corner_has_multiple_embeddings( unique_vertex_id );
            if( has_multiple_embeddings )
            {
                result.multiple_internals_corner_vertices.add_problem(
                    unique_vertex_id, has_multiple_embeddings.value() );
            }
            const auto not_internal_nor_boundary =
                corner_is_not_internal_nor_boundary( unique_vertex_id );
            if( not_internal_nor_boundary )
            {
                result.not_internal_nor_boundary_corner_vertices.add_problem(
                    unique_vertex_id, not_internal_nor_boundary.value() );
            }
            const auto without_boundary_status =
                corner_is_part_of_line_but_not_boundary( unique_vertex_id );
            if( without_boundary_status )
            {
                result.line_corners_without_boundary_status.add_problem(
                    unique_vertex_id, without_boundary_status.value() );
            }
        }
        return result;
    }

} // namespace geode
