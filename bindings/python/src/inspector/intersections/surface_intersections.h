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

#include <geode/mesh/core/triangulated_surface.h>

#include <geode/inspector/criterion/intersections/surface_intersections.h>

#define PYTHON_SURFACE_INTERSECTIONS( dimension )                              \
    const auto name##dimension = "TriangulatedSurfaceIntersections"            \
                                 + std::to_string( dimension ) + "D";          \
    pybind11::class_< TriangulatedSurfaceIntersections##dimension##D >(        \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init< const TriangulatedSurface< dimension >& >() )    \
        .def( pybind11::init< const TriangulatedSurface< dimension >&,         \
            bool >() )                                                         \
        .def( "mesh_has_self_intersections",                                   \
            &TriangulatedSurfaceIntersections##dimension##D::                  \
                mesh_has_self_intersections )                                  \
        .def( "nb_intersecting_elements_pair",                                 \
            &TriangulatedSurfaceIntersections##dimension##D::                  \
                nb_intersecting_elements_pair )                                \
        .def( "intersecting_elements",                                         \
            &TriangulatedSurfaceIntersections##dimension##D::                  \
                intersecting_elements )

namespace geode
{
    void define_surface_intersections( pybind11::module& module )
    {
        PYTHON_SURFACE_INTERSECTIONS( 2 );
        PYTHON_SURFACE_INTERSECTIONS( 3 );
    }
} // namespace geode
