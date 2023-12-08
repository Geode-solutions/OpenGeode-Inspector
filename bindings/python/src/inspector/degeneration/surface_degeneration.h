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
#include <absl/strings/str_cat.h>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/inspector/criterion/degeneration/surface_degeneration.h>

namespace geode
{
    template < index_t dimension >
    void do_define_surface_degeneration( pybind11::module& module )
    {
        using SurfaceMesh = SurfaceMesh< dimension >;
        using SurfaceMeshDegeneration = SurfaceMeshDegeneration< dimension >;
        const auto name =
            absl::StrCat( "SurfaceMeshDegeneration", dimension, +"D" );
        pybind11::class_< SurfaceMeshDegeneration >( module, name.c_str() )
            .def( pybind11::init< const SurfaceMesh& >() )
            .def( "is_mesh_degenerated",
                &SurfaceMeshDegeneration::is_mesh_degenerated )
            .def( "degenerated_edges",
                &SurfaceMeshDegeneration::degenerated_edges );
    }
    void define_surface_degeneration( pybind11::module& module )
    {
        do_define_surface_degeneration< 2 >( module );
        do_define_surface_degeneration< 3 >( module );
    }
} // namespace geode
