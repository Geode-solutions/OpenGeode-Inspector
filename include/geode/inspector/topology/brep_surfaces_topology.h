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

#pragma once
#include <absl/types/optional.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>

#include <geode/basic/uuid.h>

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    struct BRepSurfacesTopologyInspectionResult
    {
        InspectionIssues< uuid > surfaces_not_meshed{
            "uuids of surface without mesh."
        };
        std::vector< std::pair< uuid, InspectionIssues< index_t > > >
            surfaces_not_linked_to_a_unique_vertex;
        InspectionIssues< index_t >
            unique_vertices_linked_to_not_internal_nor_boundary_surface{
                "Indices of unique vertices linked to surface without boundary "
                "nor internal status."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_surface_with_invalid_embbedings{
                "Indices of unique vertices linked to a surface with invalid "
                "internal topology."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_single_and_invalid_surface{
                "Indices of unique vertices linked to only one surface and "
                "this single surface is invalid."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_several_and_invalid_surfaces{
                "Indices of unique vertices linked to several surfaces in an "
                "invalid way."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border{
                "Indices of unique vertices linked to a line but not linked to "
                "a "
                "surface border."
            };

        std::string string() const
        {
            std::string message{ "" };
            absl::StrAppend( &message, surfaces_not_meshed.string() );
            for( const auto& surface_uv_issue :
                surfaces_not_linked_to_a_unique_vertex )
            {
                absl::StrAppend( &message, surface_uv_issue.second.string() );
            }
            absl::StrAppend( &message,
                unique_vertices_linked_to_not_internal_nor_boundary_surface
                    .string() );
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_surface_with_invalid_embbedings
                    .string() );
            absl::StrAppend(
                &message, unique_vertices_linked_to_a_single_and_invalid_surface
                              .string() );
            absl::StrAppend(
                &message, unique_vertices_linked_to_several_and_invalid_surfaces
                              .string() );
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                    .string() );
            return message;
        }
    };
    /*!
     * Class for inspecting the topology of a BRep model surfaces through
     * their unique vertices
     */
    class opengeode_inspector_inspector_api BRepSurfacesTopology
    {
    public:
        BRepSurfacesTopology( const BRep& brep );

        /*!
         * Checks if the brep unique vertices are parts of valid surfaces,
         * i.e. verify:
         * Each surface is either internal or boundary.
         * Each internal surface is internal to only one object and is not
         * boundary.
         * If the vertex is part of only one surface, the vertex is part of
         * no more than 2 blocks, and the surface is either internal to a
         * block (and the vertex is in one and only one block) or a boundary
         * of all the blocks the vertex is in.
         * If the vertex is part of multiple surfaces, it is either part of
         * only one line (and no corner, and the line is boundary to all the
         * surfaces) or multiple lines (and all the lines are either
         * internal or boundary to at least 2 of the surfaces the vertex is
         * in).
         */
        bool brep_surfaces_topology_is_valid(
            index_t unique_vertex_index ) const;

        absl::optional< std::string >
            vertex_is_part_of_not_internal_nor_boundary_surface(
                index_t unique_vertex_index ) const;

        absl::optional< std::string >
            vertex_is_part_of_invalid_embedded_surface(
                index_t unique_vertex_index ) const;

        absl::optional< std::string > vertex_is_part_of_invalid_single_surface(
            index_t unique_vertex_index ) const;

        absl::optional< std::string >
            vertex_is_part_of_invalid_multiple_surfaces(
                index_t unique_vertex_index ) const;

        absl::optional< std::string >
            vertex_is_part_of_line_and_not_on_surface_border(
                index_t unique_vertex_index ) const;

        BRepSurfacesTopologyInspectionResult inspect_surfaces_topology() const;

    private:
        const BRep& brep_;
    };
} // namespace geode