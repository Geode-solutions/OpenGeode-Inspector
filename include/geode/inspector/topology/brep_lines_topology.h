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

namespace geode
{
    struct ComponentMeshVertex;
    class BRep;
    struct uuid;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api BRepLinesTopologyInspectionResult
    {
        ProblemInspectionResult< uuid > lines_not_meshed{
            "uuids of lines without mesh."
        };
        std::vector< std::pair< uuid, ProblemInspectionResult< index_t > > >
            lines_not_linked_to_a_unique_vertex;
        ProblemInspectionResult< index_t >
            unique_vertices_linked_to_not_internal_nor_boundary_line{
                "Indexes of unique vertex linked to line without boundary "
                "nor internal status."
            };
        ProblemInspectionResult< index_t >
            unique_vertices_linked_to_a_line_with_invalid_embeddings{
                "Indexes of unique vertex linked to a line with invalid "
                "internal topology."
            };
        ProblemInspectionResult< index_t >
            unique_vertices_linked_to_a_single_and_invalid_line{
                "Indexes of unique vertex linked to only one line and this "
                "single line is invalid."
            };
        ProblemInspectionResult< index_t >
            unique_vertices_linked_to_a_line_but_not_linked_to_a_corner{
                "Indexes of unique vertex linked to a line but not kinked to a "
                "corner."
            };
    };
    /*!
     * Class for inspecting the topology of a BRep model lines through their
     * unique vertices
     */
    class opengeode_inspector_inspector_api BRepLinesTopology
    {
    public:
        BRepLinesTopology( const BRep& brep );

        /*!
         * Checks if the brep unique vertices are parts of valid lines, i.e.
         * verify:
         * Each line is either internal or boundary.
         * If a line is internal to an object, it cannot be incident to it.
         * If the vertex is part of only one line, the line is either
         * internal to a surface, internal to a block, or a boundary of
         * multiple surfaces.
         * If the vertex is part of multiple lines, it is also a corner.
         */
        bool brep_lines_topology_is_valid( index_t unique_vertex_index ) const;

        absl::optional< std::string >
            vertex_is_part_of_not_internal_nor_boundary_line(
                index_t unique_vertex_index ) const;

        absl::optional< std::string > vertex_is_part_of_invalid_embedded_line(
            index_t unique_vertex_index ) const;

        absl::optional< std::string > vertex_is_part_of_invalid_single_line(
            index_t unique_vertex_index ) const;

        absl::optional< std::string > vertex_has_lines_but_no_corner(
            index_t unique_vertex_index ) const;

        BRepLinesTopologyInspectionResult inspect_lines_topology() const;

    private:
        const BRep& brep_;
    };
} // namespace geode