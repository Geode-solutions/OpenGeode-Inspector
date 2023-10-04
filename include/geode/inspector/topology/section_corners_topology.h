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
    class Section;
} // namespace geode

namespace geode
{
    namespace detail
    {
        class SectionCornersTopologyImpl
        {
        public:
            SectionCornersTopologyImpl( const Section& section );

            SectionCornersTopologyImpl( const Section& section, bool verbose );

            /*!
             * Checks if the section unique vertices are valid corners, i.e.
             * corners that verify:
             * Each unique_vertex can only be associated to one corner.
             * Each corner can only be internal to one object (surface or
             * block).
             * Each corner is a boundary of every line it is associated to.
             */
            bool section_corner_topology_is_valid(
                index_t unique_vertex_index ) const;

            bool unique_vertex_has_multiple_corners(
                index_t unique_vertex_index ) const;

            bool corner_has_multiple_embeddings(
                index_t unique_vertex_index ) const;

            bool corner_is_not_internal_nor_boundary(
                index_t unique_vertex_index ) const;

            bool corner_is_part_of_line_but_not_boundary(
                index_t unique_vertex_index ) const;

        private:
            const Section& section_;
            bool verbose_;
        };
    } // namespace detail
} // namespace geode
