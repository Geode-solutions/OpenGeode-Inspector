/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/inspector/criterion/degeneration/solid_degeneration.hpp>

namespace geode
{
    template < index_t dimension >
    void do_define_solid_degeneration( pybind11::module& module )
    {
        using SolidMesh = SolidMesh< dimension >;
        using SolidMeshDegeneration = SolidMeshDegeneration< dimension >;
        const auto name =
            absl::StrCat( "SolidMeshDegeneration", dimension, "D" );
        pybind11::class_< SolidMeshDegeneration >( module, name.c_str() )
            .def( pybind11::init< const SolidMesh& >() )
            .def( "is_mesh_degenerated",
                &SolidMeshDegeneration::is_mesh_degenerated )
            .def(
                "degenerated_edges", &SolidMeshDegeneration::degenerated_edges )
            .def( "degenerated_polyhedra",
                &SolidMeshDegeneration::degenerated_polyhedra );
    }
    void define_solid_degeneration( pybind11::module& module )
    {
        do_define_solid_degeneration< 3 >( module );
    }
} // namespace geode
