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
        /*!
         * Class for inspecting the topology of a Section model surfaces through
         * its unique vertices
         */
        class SectionSurfacesTopologyImpl
        {
        public:
            SectionSurfacesTopologyImpl( const Section& section );

            SectionSurfacesTopologyImpl( const Section& section, bool verbose );

            /*!
             * Checks if the section unique vertices are parts of valid
             * surfaces, i.e. verify:
             * If a unique vertex is part of two surfaces, it is part of a least
             * one line which is boundary of the two blocks.
             */
            bool section_vertex_surfaces_topology_is_valid(
                index_t unique_vertex_index ) const;

            bool vertex_is_part_of_invalid_surfaces_topology(
                index_t unique_vertex_index ) const;

            bool vertex_is_part_of_line_and_not_on_surface_border(
                index_t unique_vertex_index ) const;

        private:
            const Section& section_;
            bool verbose_;
        };
    } // namespace detail
} // namespace geode