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

#include <geode/basic/pimpl.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>
#include <geode/inspector/topology/brep_blocks_topology.h>
#include <geode/inspector/topology/brep_corners_topology.h>
#include <geode/inspector/topology/brep_lines_topology.h>
#include <geode/inspector/topology/brep_surfaces_topology.h>

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    struct BRepTopologyInspectionResult
    {
        BRepCornersTopologyInspectionResult corners;
        BRepLinesTopologyInspectionResult lines;
        BRepSurfacesTopologyInspectionResult surfaces;
        BRepBlocksTopologyInspectionResult blocks;

        InspectionIssues< index_t > unique_vertices_not_linked_to_any_component{
            "Unique vertices not linked to any component"
        };

        std::string string() const
        {
            std::string message{ "" };
            absl::StrAppend( &message, corners.string() );
            absl::StrAppend( &message, lines.string() );
            absl::StrAppend( &message, surfaces.string() );
            absl::StrAppend( &message, blocks.string() );
            absl::StrAppend( &message,
                unique_vertices_not_linked_to_any_component.string() );
            return message;
        }
    };
    /*!
     * Class for inspecting the topology of a BRep model corners
     */
    class opengeode_inspector_inspector_api BRepTopologyInspector
        : public BRepCornersTopology,
          public BRepLinesTopology,
          public BRepSurfacesTopology,
          public BRepBlocksTopology
    {
        OPENGEODE_DISABLE_COPY( BRepTopologyInspector );

    public:
        BRepTopologyInspector( const BRep& brep );

        ~BRepTopologyInspector();

        /*!
         * Checks if the brep is topologically valid through the unique
         * vertices.
         */
        bool brep_topology_is_valid() const;

        bool brep_meshed_components_are_linked_to_unique_vertices() const;

        bool brep_unique_vertices_are_linked_to_a_component_vertex() const;

        BRepTopologyInspectionResult inspect_brep_topology() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode