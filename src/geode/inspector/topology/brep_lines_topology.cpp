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

#include <geode/inspector/topology/brep_lines_topology.hpp>

#include <optional>

#include <absl/algorithm/container.h>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/topology/internal/topology_helpers.hpp>

namespace geode
{
    index_t BRepLinesTopologyInspectionResult::nb_issues() const
    {
        return lines_not_meshed.nb_issues()
               + lines_not_linked_to_a_unique_vertex.nb_issues()
               + unique_vertices_linked_to_a_line_with_invalid_embeddings
                     .nb_issues()
               + unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                     .nb_issues();
    }

    std::string BRepLinesTopologyInspectionResult::string() const
    {
        std::string message;
        if( lines_not_meshed.nb_issues() != 0 )
        {
            absl::StrAppend( &message, lines_not_meshed.string() );
        }
        if( lines_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, lines_not_linked_to_a_unique_vertex.string() );
        }
        if( unique_vertices_linked_to_a_line_with_invalid_embeddings.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_line_with_invalid_embeddings
                    .string() );
        }
        if( unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                    .string() );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "No issues with lines topology \n";
    }

    std::string BRepLinesTopologyInspectionResult::inspection_type() const
    {
        return "Lines topology inspection";
    }

    BRepLinesTopology::BRepLinesTopology( const BRep& brep ) : brep_( brep ) {}

    bool BRepLinesTopology::brep_lines_topology_is_valid(
        index_t unique_vertex_index ) const
    {
        bool part_of_line{ false };
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Line3D::component_type_static() )
            {
                part_of_line = true;
                break;
            }
        }
        if( !part_of_line )
        {
            return true;
        }
        if( vertex_is_part_of_invalid_embedded_line( unique_vertex_index )
            || vertex_has_lines_but_is_not_a_corner( unique_vertex_index ) )
        {
            return false;
        }
        return true;
    }

    bool BRepLinesTopology::line_is_meshed( const Line3D& line ) const
    {
        const auto& line_mesh = line.mesh();
        return line_mesh.nb_vertices() != 0 && line_mesh.nb_edges() != 0;
    }

    bool BRepLinesTopology::line_vertices_are_associated_to_unique_vertices(
        const Line3D& line ) const
    {
        return internal::
            model_component_vertices_are_associated_to_unique_vertices(
                brep_, line.component_id(), line.mesh() );
    }

    std::optional< std::string >
        BRepLinesTopology::vertex_is_part_of_invalid_embedded_line(
            index_t unique_vertex_index ) const
    {
        for( const auto& line_cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( line_cmv.component_id.type()
                != Line3D::component_type_static() )
            {
                continue;
            }
            const auto line_id = line_cmv.component_id.id();
            for( const auto& embedding : brep_.embeddings( line_id ) )
            {
                if( brep_.Relationships::is_boundary(
                        line_id, embedding.id() ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index, " is part of line with uuid '",
                        line_id.string(),
                        "', which is both boundary and embedded in "
                        "surface with uuid '",
                        embedding.id().string() + "'." );
                }
                if( embedding.type() == Block3D::component_type_static()
                    && !internal::brep_blocks_are_meshed( brep_ ) )
                {
                    continue;
                }
                if( !absl::c_any_of(
                        brep_.component_mesh_vertices( unique_vertex_index ),
                        [&embedding]( const ComponentMeshVertex& cmv ) {
                            return cmv.component_id == embedding;
                        } ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index, " is part of line with uuid '",
                        line_id.string(),
                        "', which is embedded in surface with uuid '",
                        embedding.id().string(),
                        "', but the unique vertex is not linked to the "
                        "surface mesh vertices." );
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepLinesTopology::vertex_has_lines_but_is_not_a_corner(
            index_t unique_vertex_index ) const
    {
        index_t nb_lines{ 0 };
        bool is_a_corner{ false };
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Corner3D::component_type_static() )
            {
                is_a_corner = true;
            }
            else if( cmv.component_id.type()
                     == Line3D::component_type_static() )
            {
                nb_lines++;
            }
        }
        if( nb_lines > 1 && !is_a_corner )
        {
            return absl::StrCat( "Unique vertex with index ",
                unique_vertex_index,
                " is part of multiple lines but is not a corner." );
        }
        return std::nullopt;
    }

    BRepLinesTopologyInspectionResult
        BRepLinesTopology::inspect_lines_topology() const
    {
        BRepLinesTopologyInspectionResult result;
        for( const auto& line : brep_.lines() )
        {
            if( !line_is_meshed( brep_.line( line.id() ) ) )
            {
                result.lines_not_meshed.add_issue(
                    line.id(), absl::StrCat( line.id().string(),
                                   " is a line without mesh." ) );
            }

            auto line_result = internal::
                model_component_vertices_not_associated_to_unique_vertices(
                    brep_, line.component_id(), line.mesh() );
            if( line_result.nb_issues() != 0 )
            {
                line_result.set_description(
                    absl::StrCat( "Line ", line.id().string() ) );
                result.lines_not_linked_to_a_unique_vertex.add_issues_to_map(
                    line.id(), std::move( line_result ) );
            }
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( const auto invalid_internal_topology =
                    vertex_is_part_of_invalid_embedded_line(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_a_line_with_invalid_embeddings
                    .add_issue(
                        unique_vertex_id, invalid_internal_topology.value() );
            }
            if( const auto lines_but_is_not_corner =
                    vertex_has_lines_but_is_not_a_corner( unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                    .add_issue(
                        unique_vertex_id, lines_but_is_not_corner.value() );
            }
        }
        return result;
    }
} // namespace geode
