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
         * Class for inspecting the topology of a BRep model blocks through
         * their unique vertices
         */
        class BRepBlocksTopologyImpl
        {
        public:
            BRepBlocksTopologyImpl( const BRep& brep );

            BRepBlocksTopologyImpl( const BRep& brep, bool verbose );

            /*!
             * Checks if the brep unique vertices are parts of valid blocks,
             * i.e. verify:
             * If the vertex is part of multiple blocks, either it is part of
             * exactly 2 blocks (and at least one surface which is boundary to
             * the 2 blocks), or it is part of more than to blocks (and it is
             * either a corner, or not a corner but part of only one line).
             */
            bool brep_vertex_blocks_topology_is_valid(
                index_t unique_vertex_index ) const;

        private:
            const BRep& brep_;
            bool verbose_;
        };
    } // namespace detail
} // namespace geode