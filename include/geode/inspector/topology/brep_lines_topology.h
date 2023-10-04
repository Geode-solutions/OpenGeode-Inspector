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

#include <geode/inspector/common.h>

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    namespace detail
    {
        /*!
         * Class for inspecting the topology of a BRep model lines through their
         * unique vertices
         */
        class BRepLinesTopologyImpl
        {
        public:
            BRepLinesTopologyImpl( const BRep& brep );

            BRepLinesTopologyImpl( const BRep& brep, bool verbose );

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
            bool brep_vertex_lines_topology_is_valid(
                index_t unique_vertex_index ) const;

            bool vertex_is_part_of_not_boundary_nor_internal_line(
                const index_t unique_vertex_index ) const;

            bool vertex_is_part_of_line_with_invalid_internal_topology(
                const index_t unique_vertex_index ) const;

            bool vertex_is_part_of_invalid_unique_line(
                index_t unique_vertex_index ) const;

            bool vertex_has_lines_but_is_not_corner(
                index_t unique_vertex_index ) const;

        private:
            const BRep& brep_;
            bool verbose_;
        };
    } // namespace detail
} // namespace geode