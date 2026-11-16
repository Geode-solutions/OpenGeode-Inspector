/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/inspector/inspection/criterion/isolation/solid_isolation.hpp>

#include <geode/inspector/bindings/python/inspection_helpers.hpp>

namespace geode
{
    template < index_t dimension >
    void do_define_solid_isolation( pybind11::module& module )
    {
        using SolidMesh = SolidMesh< dimension >;
        using SolidMeshIsolation = SolidMeshIsolation< dimension >;
        const auto name = absl::StrCat( "SolidMeshIsolation", dimension, "D" );
        pybind11::class_< SolidMeshIsolation >( module, name.c_str(),
            "Inspection of the isolated vertices of a SolidMesh: the vertices "
            "that "
            "are in no polyhedron" )
            .def( pybind11::init< const SolidMesh& >(), pybind11::arg( "mesh" ),
                pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the isolated vertices of this mesh. "
                "The inspector keeps the mesh alive" )
            .def( "mesh_has_isolated_vertices",
                &SolidMeshIsolation::mesh_has_isolated_vertices, ReleaseGil(),
                "Return True if a vertex is in no polyhedron" )
            .def( "isolated_vertices", &SolidMeshIsolation::isolated_vertices,
                ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "vertices that are in no polyhedron" );
    }

    void define_solid_isolation( pybind11::module& module )
    {
        do_define_solid_isolation< 3 >( module );
    }
} // namespace geode
