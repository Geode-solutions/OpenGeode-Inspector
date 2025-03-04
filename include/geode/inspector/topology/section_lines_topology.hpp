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

#include <geode/basic/uuid.hpp>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    class Section;
} // namespace geode

namespace geode
{
    struct
        opengeode_inspector_inspector_api SectionLinesTopologyInspectionResult
    {
        InspectionIssues< uuid > lines_not_meshed{
            "uuids of Lines without mesh."
        };
        InspectionIssuesMap< index_t > lines_not_linked_to_a_unique_vertex{
            "Lines with mesh vertices not linked to a unique vertex"
        };
        InspectionIssues< index_t >
            unique_vertices_linked_to_not_internal_nor_boundary_line{
                "Indices of unique vertices linked to a Line without boundary "
                "nor internal status."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_line_with_invalid_embeddings{
                "Indices of unique vertices linked to a Line with invalid "
                "internal topology."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_single_and_invalid_line{
                "Indices of unique vertices linked to only one Line and this "
                "single Line is invalid."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner{
                "Indices of unique vertices linked to several Lines but not "
                "linked to a corner."
            };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };
    /*!
     * Class for inspecting the topology of a Section model lines through
     * their unique vertices
     */
    class opengeode_inspector_inspector_api SectionLinesTopology
    {
    public:
        explicit SectionLinesTopology( const Section& section );

        /*!
         * Checks if the section unique vertices are parts of valid lines,
         * i.e. verify:
         * Each line is either internal or boundary.
         * Each internal line is internal to only one object and is not
         * boundary.
         * If the vertex is part of only one line, the vertex is part of
         * no more than 2 surfaces, and the line is either internal to a
         * surface (and the vertex is in one and only one surface) or a
         * boundary of all the surfaces the vertex is in.
         * If the vertex is part of multiple lines, it is also a corner.
         */
        [[nodiscard]] bool section_lines_topology_is_valid(
            index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_not_internal_nor_boundary_line(
                const index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_invalid_embedded_line(
                const index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_invalid_single_line(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_has_lines_but_is_not_a_corner(
                index_t unique_vertex_index ) const;

        [[nodiscard]] SectionLinesTopologyInspectionResult
            inspect_lines_topology() const;

    private:
        const Section& section_;
    };
} // namespace geode