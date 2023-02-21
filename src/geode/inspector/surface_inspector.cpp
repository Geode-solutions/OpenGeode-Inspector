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

#include <geode/inspector/surface_inspector.h>

#include <geode/mesh/core/triangulated_surface.h>

namespace geode
{
    template < index_t dimension >
    SurfaceMeshInspector< dimension >::SurfaceMeshInspector(
        const SurfaceMesh< dimension >& mesh )
        : AddInspectors< SurfaceMesh< dimension >,
            SurfaceMeshAdjacency< dimension >,
            SurfaceMeshColocation< dimension >,
            SurfaceMeshDegeneration< dimension >,
            SurfaceMeshEdgeManifold< dimension >,
            SurfaceMeshVertexManifold< dimension > >{ mesh }
    {
    }

    template < index_t dimension >
    SurfaceMeshInspector< dimension >::SurfaceMeshInspector(
        const SurfaceMesh< dimension >& mesh, bool verbose )
        : AddInspectors< SurfaceMesh< dimension >,
            SurfaceMeshAdjacency< dimension >,
            SurfaceMeshColocation< dimension >,
            SurfaceMeshDegeneration< dimension >,
            SurfaceMeshEdgeManifold< dimension >,
            SurfaceMeshVertexManifold< dimension > >{ mesh, verbose }
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceInspector< dimension >::TriangulatedSurfaceInspector(
        const TriangulatedSurface< dimension >& mesh )
        : SurfaceMeshInspector< dimension >{ mesh },
          AddInspectors< TriangulatedSurface< dimension >,
              TriangulatedSurfaceIntersections< dimension > >{ mesh }
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceInspector< dimension >::TriangulatedSurfaceInspector(
        const TriangulatedSurface< dimension >& mesh, bool verbose )
        : SurfaceMeshInspector< dimension >{ mesh, verbose },
          AddInspectors< TriangulatedSurface< dimension >,
              TriangulatedSurfaceIntersections< dimension > >{ mesh, verbose }
    {
    }

    template class opengeode_inspector_inspector_api SurfaceMeshInspector< 2 >;
    template class opengeode_inspector_inspector_api SurfaceMeshInspector< 3 >;

    template class opengeode_inspector_inspector_api
        TriangulatedSurfaceInspector< 2 >;
    template class opengeode_inspector_inspector_api
        TriangulatedSurfaceInspector< 3 >;
} // namespace geode
