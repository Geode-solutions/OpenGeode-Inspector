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

#include <absl/types/span.h>

#include <geode/basic/pimpl.h>

#include <geode/inspector/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( TriangulatedSurface );
    FORWARD_DECLARATION_DIMENSION_CLASS( EdgedCurve );
} // namespace geode

namespace geode
{
    /*!
     * Class for inspecting the intersections of TriangulatedSurfacees
     */
    template < index_t dimension >
    class opengeode_inspector_inspector_api SurfaceCurveIntersections
    {
        OPENGEODE_DISABLE_COPY( SurfaceCurveIntersections );

    public:
        SurfaceCurveIntersections(
            const TriangulatedSurface< dimension >& surface,
            const EdgedCurve< dimension >& curve );

        SurfaceCurveIntersections( const TriangulatedSurface< dimension >& mesh,
            const EdgedCurve< dimension >& curve,
            bool verbose );

        ~SurfaceCurveIntersections();

        bool meshes_have_intersections() const;

        index_t nb_intersecting_elements_pair() const;

        /* Returns all pairs of intersecting triangles and edges.
         * First element of each pair is a triangle index in the
         * TriangulatedSurface, second element of each pair is an edge index in
         * the EdgedCurve.
         */
        std::vector< std::pair< index_t, index_t > >
            intersecting_elements() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_2D_AND_3D( SurfaceCurveIntersections );
} // namespace geode