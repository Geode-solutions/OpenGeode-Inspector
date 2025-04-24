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

#pragma once
#include <optional>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

#include <geode/basic/uuid.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Surface );
    ALIAS_3D( Surface );
    class BRep;
} // namespace geode

namespace geode
{
    struct
        opengeode_inspector_inspector_api BRepSurfacesTopologyInspectionResult
    {
        InspectionIssues< uuid > surfaces_not_meshed{
            "uuids of surface without mesh."
        };
        InspectionIssuesMap< index_t > surfaces_not_linked_to_a_unique_vertex{
            "Surfaces with mesh vertices not linked to a unique vertex"
        };
        InspectionIssues< index_t >
            unique_vertices_linked_to_not_internal_nor_boundary_surface{
                "Indices of unique vertices linked to Surface without boundary "
                "nor internal status."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_surface_with_invalid_embbedings{
                "Indices of unique vertices linked to a Surface with invalid "
                "internal topology."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_single_and_invalid_surface{
                "Indices of unique vertices linked to only one Surface and "
                "this single surface is invalid."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_several_and_invalid_surfaces{
                "Indices of unique vertices linked to several Surfaces in an "
                "invalid way."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border{
                "Indices of unique vertices linked to a Line but not linked to "
                "a Surface border."
            };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };
    /*!
     * Class for inspecting the topology of a BRep model surfaces through
     * their unique vertices
     */
    class opengeode_inspector_inspector_api BRepSurfacesTopology
    {
    public:
        explicit BRepSurfacesTopology( const BRep& brep );

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
        [[nodiscard]] bool brep_surfaces_topology_is_valid(
            index_t unique_vertex_index ) const;

        [[nodiscard]] bool surface_is_meshed( const Surface3D& surface ) const;

        [[nodiscard]] bool surface_vertices_are_associated_to_unique_vertices(
            const Surface3D& surface ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_not_internal_nor_boundary_surface(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_invalid_embedded_surface(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_invalid_single_surface(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_invalid_multiple_surfaces(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_line_and_not_on_surface_border(
                index_t unique_vertex_index ) const;

        [[nodiscard]] BRepSurfacesTopologyInspectionResult
            inspect_surfaces_topology() const;

    private:
        const BRep& brep_;
    };
} // namespace geode