/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
        opengeode_inspector_inspector_api SectionSurfacesTopologyInspectionResult
    {
        InspectionIssues< uuid > surfaces_not_meshed{
            "uuids of Surfaces without mesh."
        };
        InspectionIssuesMap< index_t > surfaces_not_linked_to_a_unique_vertex{
            "Surfaces with mesh vertices not linked to a unique vertex"
        };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_surface_with_invalid_embbedings{
                "Indices of unique vertices linked to a Surface with invalid "
                "internal topology."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border{
                "Indices of unique vertices linked to a Line but not linked to "
                "a Surface border."
            };

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };
    /*!
     * Class for inspecting the topology of a Section model surfaces through
     * its unique vertices
     */
    class opengeode_inspector_inspector_api SectionSurfacesTopology
    {
    public:
        explicit SectionSurfacesTopology( const Section& section );

        /*!
         * Checks if the section unique vertices are parts of valid
         * surfaces, i.e. verify:
         * If a unique vertex is part of two surfaces, it is part of a least
         * one line which is boundary of the two blocks.
         */
        [[nodiscard]] bool section_vertex_surfaces_topology_is_valid(
            index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_invalid_embedded_surface(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_line_and_not_on_surface_border(
                index_t unique_vertex_index ) const;

        [[nodiscard]] SectionSurfacesTopologyInspectionResult
            inspect_surfaces() const;

    private:
        const Section& section_;
    };
} // namespace geode