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
#include <geode/inspector/criterion/adjacency/surface_adjacency.h>
#include <geode/inspector/criterion/colocation/surface_colocation.h>
#include <geode/inspector/criterion/degeneration/surface_degeneration.h>
#include <geode/inspector/criterion/intersections/surface_intersections.h>
#include <geode/inspector/criterion/manifold/surface_edge_manifold.h>
#include <geode/inspector/criterion/manifold/surface_vertex_manifold.h>
#include <geode/inspector/mixin/add_inspectors.h>

namespace geode
{
    /*!
     * Class for inspecting a SurfaceMesh
     * @extends SurfaceMeshAdjacency
     * @extends SurfaceMeshColocation
     * @extends SurfaceMeshDegeneration
     * @extends SurfaceMeshEdgeManifold
     * @extends SurfaceMeshVertexManifold
     */
    template < index_t dimension >
    class SurfaceMeshInspector : public AddInspectors< SurfaceMesh< dimension >,
                                     SurfaceMeshAdjacency< dimension >,
                                     SurfaceMeshColocation< dimension >,
                                     SurfaceMeshDegeneration< dimension >,
                                     SurfaceMeshEdgeManifold< dimension >,
                                     SurfaceMeshVertexManifold< dimension > >
    {
        OPENGEODE_DISABLE_COPY( SurfaceMeshInspector );

    public:
        SurfaceMeshInspector( const SurfaceMesh< dimension >& mesh );

        SurfaceMeshInspector(
            const SurfaceMesh< dimension >& mesh, bool verbose );
    };
    ALIAS_2D_AND_3D( SurfaceMeshInspector );

    /*!
     * Class for inspecting a TriangulatedSurface
     * @extends SurfaceMeshInspector
     * @extends TriangulatedSurfaceIntersections
     */
    template < index_t dimension >
    class TriangulatedSurfaceInspector
        : public SurfaceMeshInspector< dimension >,
          public AddInspectors< TriangulatedSurface< dimension >,
              TriangulatedSurfaceIntersections< dimension > >
    {
    public:
        TriangulatedSurfaceInspector(
            const TriangulatedSurface< dimension >& mesh );

        TriangulatedSurfaceInspector(
            const TriangulatedSurface< dimension >& mesh, bool verbose );
    };
    ALIAS_2D_AND_3D( TriangulatedSurfaceInspector );
} // namespace geode