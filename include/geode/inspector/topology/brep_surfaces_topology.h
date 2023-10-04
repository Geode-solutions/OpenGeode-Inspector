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
         * Class for inspecting the topology of a BRep model surfaces through
         * their unique vertices
         */
        class BRepSurfacesTopologyImpl
        {
        public:
            BRepSurfacesTopologyImpl( const BRep& brep );

            BRepSurfacesTopologyImpl( const BRep& brep, bool verbose );

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
            bool brep_vertex_surfaces_topology_is_valid(
                index_t unique_vertex_index ) const;

            bool vertex_is_part_of_not_boundary_nor_internal_surface(
                const index_t unique_vertex_index ) const;

            bool vertex_is_part_of_surface_with_invalid_internal_topology(
                const index_t unique_vertex_index ) const;

            bool vertex_is_part_of_invalid_unique_surface(
                index_t unique_vertex_index ) const;

            bool vertex_is_part_of_invalid_multiple_surfaces(
                index_t unique_vertex_index ) const;

            bool vertex_is_part_of_line_and_not_on_surface_border(
                index_t unique_vertex_index ) const;

        private:
            const BRep& brep_;
            bool verbose_;
        };
    } // namespace detail
} // namespace geode