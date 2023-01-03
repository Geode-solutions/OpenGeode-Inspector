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

#include <geode/mesh/core/surface_mesh.h>

#include <geode/inspector/criterion/colocation/surface_colocation.h>

#define PYTHON_SURFACE_COLOCATION( dimension )                                 \
    const auto name##dimension =                                               \
        "SurfaceMeshColocation" + std::to_string( dimension ) + "D";           \
    pybind11::class_< SurfaceMeshColocation##dimension##D >(                   \
        module, name##dimension.c_str() )                                      \
        .def( pybind11::init< const SurfaceMesh< dimension >& >() )            \
        .def( pybind11::init< const SurfaceMesh< dimension >&, bool >() )      \
        .def( "mesh_has_colocated_points",                                     \
            &SurfaceMeshColocation##dimension##D::mesh_has_colocated_points )  \
        .def( "nb_colocated_points",                                           \
            &SurfaceMeshColocation##dimension##D::nb_colocated_points )        \
        .def( "colocated_points_groups",                                       \
            &SurfaceMeshColocation##dimension##D::colocated_points_groups )

namespace geode
{
    void define_surface_colocation( pybind11::module& module )
    {
        PYTHON_SURFACE_COLOCATION( 2 );
        PYTHON_SURFACE_COLOCATION( 3 );
    }
} // namespace geode
