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
#include <string>

#include <geode/basic/uuid.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api BRepCornersTopologyInspectionResult
    {
        InspectionIssues< uuid > corners_not_meshed{
            "uuids of Corners without mesh."
        };
        InspectionIssuesMap< index_t > corners_not_linked_to_a_unique_vertex{
            "Corners with mesh vertices not linked to a unique vertex"
        };
        InspectionIssues< index_t > unique_vertices_linked_to_multiple_corners{
            "Indices of unique vertices that are part of several Corners."
        };
        InspectionIssues< index_t >
            unique_vertices_linked_to_multiple_internals_corner{
                "Indices of unique vertices linked to a Corner with several "
                "embeddings"
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_not_internal_nor_boundary_corner{
                "Indices of unique vertices linked to a Corner without "
                "boundary nor internal status."
            };
        InspectionIssues< index_t >
            unique_vertices_liked_to_not_boundary_line_corner{
                "Indices of unique vertices linked to a Corner on a line but "
                "without boundary status."
            };

        std::string string() const;

        std::string inspection_type() const;
    };

    class opengeode_inspector_inspector_api BRepCornersTopology
    {
    public:
        BRepCornersTopology( const BRep& brep );

        /*!
         * Checks if the brep unique vertices are valid corners, i.e.
         * corners that verify:
         * Each unique_vertex can only be associated to one corner.
         * Each corner can only be internal to one object (surface or
         * block).
         * Each corner is a boundary of every line it is associated to.
         */
        bool brep_corner_topology_is_valid( index_t unique_vertex_index ) const;

        std::optional< std::string > unique_vertex_has_multiple_corners(
            index_t unique_vertex_index ) const;

        std::optional< std::string > corner_has_multiple_embeddings(
            index_t unique_vertex_index ) const;

        std::optional< std::string > corner_is_not_internal_nor_boundary(
            index_t unique_vertex_index ) const;

        std::optional< std::string > corner_is_part_of_line_but_not_boundary(
            index_t unique_vertex_index ) const;

        BRepCornersTopologyInspectionResult inspect_corners_topology() const;

    private:
        const BRep& brep_;
    };
} // namespace geode
