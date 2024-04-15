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

#include <geode/basic/pimpl.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>
#include <geode/inspector/topology/section_corners_topology.h>
#include <geode/inspector/topology/section_lines_topology.h>
#include <geode/inspector/topology/section_surfaces_topology.h>

namespace geode
{
    class Section;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api SectionTopologyInspectionResult
    {
        SectionCornersTopologyInspectionResult corners;
        SectionLinesTopologyInspectionResult lines;
        SectionSurfacesTopologyInspectionResult surfaces;
        InspectionIssues< index_t > unique_vertices_not_linked_to_any_component{
            "Unique vertices not linked to any component"
        };

        std::string string() const;

        std::string inspection_type() const;
    };

    /*!
     * Class for inspecting the topology of a Section model corners
     */
    class opengeode_inspector_inspector_api SectionTopologyInspector
        : public SectionCornersTopology,
          public SectionLinesTopology,
          public SectionSurfacesTopology
    {
        OPENGEODE_DISABLE_COPY( SectionTopologyInspector );

    public:
        SectionTopologyInspector( const Section& section );
        ~SectionTopologyInspector();

        /*!
         * Checks if the section is topologically valid through the unique
         * vertices.
         */
        bool section_topology_is_valid() const;

        bool section_meshed_components_are_linked_to_unique_vertices() const;

        bool section_unique_vertices_are_linked_to_a_component_vertex() const;

        SectionTopologyInspectionResult inspect_section_topology() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode