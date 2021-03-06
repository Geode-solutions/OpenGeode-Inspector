/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    /*!
     * Class for inspecting the topology of a BRep model corners
     */
    class opengeode_inspector_inspector_api BRepTopologyInspector
    {
        OPENGEODE_DISABLE_COPY( BRepTopologyInspector );

    public:
        BRepTopologyInspector( const BRep& brep );

        BRepTopologyInspector( const BRep& brep, bool verbose );

        ~BRepTopologyInspector();

        /*!
         * Checks if the brep is topologically valid through the unique
         * vertices.
         */
        bool brep_topology_is_valid() const;

        bool brep_meshed_components_are_linked_to_a_unique_vertex() const;

        index_t nb_corners_not_linked_to_a_unique_vertex() const;

        index_t nb_lines_meshed_but_not_linked_to_a_unique_vertex() const;

        index_t nb_surfaces_meshed_but_not_linked_to_a_unique_vertex() const;

        index_t nb_blocks_meshed_but_not_linked_to_a_unique_vertex() const;

        std::vector< index_t >
            invalid_components_topology_unique_vertices() const;

        std::vector< index_t > multiple_corners_unique_vertices() const;

        std::vector< index_t > multiple_internals_corner_vertices() const;

        std::vector< index_t >
            not_internal_nor_boundary_corner_vertices() const;

        std::vector< index_t > line_corners_without_boundary_status() const;

        std::vector< index_t >
            part_of_not_boundary_nor_internal_line_unique_vertices() const;

        std::vector< index_t >
            part_of_line_with_invalid_internal_topology_unique_vertices() const;

        std::vector< index_t >
            part_of_invalid_unique_line_unique_vertices() const;

        std::vector< index_t >
            part_of_lines_but_not_corner_unique_vertices() const;

        std::vector< index_t >
            part_of_not_boundary_nor_internal_surface_unique_vertices() const;

        std::vector< index_t >
            part_of_surface_with_invalid_internal_topology_unique_vertices()
                const;

        std::vector< index_t >
            part_of_invalid_unique_surface_unique_vertices() const;

        std::vector< index_t >
            part_of_invalid_multiple_surfaces_unique_vertices() const;

        std::vector< index_t >
            part_of_line_and_not_on_surface_border_unique_vertices() const;

        std::vector< index_t > part_of_invalid_blocks_unique_vertices() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode