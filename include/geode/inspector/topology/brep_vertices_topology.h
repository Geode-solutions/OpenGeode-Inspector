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
    class opengeode_inspector_inspector_api BRepVerticesTopology
    {
        OPENGEODE_DISABLE_COPY( BRepVerticesTopology );

    public:
        BRepVerticesTopology( const BRep& brep );
        ~BRepVerticesTopology();

        /*!
         * Checks if the brep has topologically valid unique vertices, i.e. has
         * vertices which verify:
         * Each unique_vertex can only be associated to one corner.
         * Each corner can only be internal to one object (surface or block).
         * Each corner is a boundary of every line it is associated to.
         */
        bool brep_vertices_topology_is_valid() const;

        std::vector< index_t > multiple_corners_unique_vertices() const;

        std::vector< index_t > multiple_internals_corner_vertices() const;

        std::vector< index_t >
            not_internal_nor_boundary_corner_vertices() const;

        std::vector< index_t > line_corners_without_boundary_status() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode