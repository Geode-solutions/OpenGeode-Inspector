/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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
#include <string>

#include <geode/basic/uuid.hpp>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Corner );
    ALIAS_2D( Corner );
    class Section;
} // namespace geode

namespace geode
{
    struct
        opengeode_inspector_inspector_api SectionCornersTopologyInspectionResult
    {
        InspectionIssues< uuid > corners_not_meshed{
            "Corners without mesh (uuids listed)"
        };
        InspectionIssuesMap< index_t > corners_not_linked_to_a_unique_vertex{
            "Corners containing mesh vertices not linked to unique vertices"
        };
        InspectionIssues< index_t > unique_vertices_linked_to_multiple_corners{
            "unique vertices shared by several Corners"
        };
        InspectionIssues< index_t >
            unique_vertices_linked_to_multiple_internals_corner{
                "unique vertices linked to a Corner with several "
                "embeddings"
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_not_internal_nor_boundary_corner{
                "unique vertices linked to a Corner with neither "
                "boundary nor internal status"
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_not_boundary_line_corner{
                "unique vertices linked to a Line's Corner without boundary "
                "status"
            };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };

    class opengeode_inspector_inspector_api SectionCornersTopology
    {
    public:
        explicit SectionCornersTopology( const Section& section );

        /*!
         * Checks if the section unique vertices are valid corners, i.e.
         * corners that verify:
         * Each unique_vertex can only be associated to one corner.
         * Each corner can only be internal to one object (surface or
         * block).
         * Each corner is a boundary of every line it is associated to.
         */
        [[nodiscard]] bool section_corner_topology_is_valid(
            index_t unique_vertex_index ) const;

        [[nodiscard]] bool corner_is_meshed( const Corner2D& corner ) const;

        [[nodiscard]] bool corner_vertices_are_associated_to_unique_vertices(
            const Corner2D& corner ) const;

        [[nodiscard]] std::optional< std::string >
            unique_vertex_has_multiple_corners(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            corner_has_multiple_embeddings( index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            corner_is_not_internal_nor_boundary(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            corner_is_part_of_line_but_not_boundary(
                index_t unique_vertex_index ) const;

        [[nodiscard]] SectionCornersTopologyInspectionResult
            inspect_corners_topology() const;

    private:
        const Section& section_;
    };
} // namespace geode
