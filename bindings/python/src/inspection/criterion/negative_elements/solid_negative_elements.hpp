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

#include <geode/inspector/inspection/criterion/negative_elements/solid_negative_elements.hpp>

#include <geode/inspector/bindings/python/inspection_helpers.hpp>

namespace geode
{
    template < index_t dimension >
    void do_define_solid_negative_elements( pybind11::module& module )
    {
        using SolidMesh = SolidMesh< dimension >;
        using SolidMeshNegativeElements =
            SolidMeshNegativeElements< dimension >;
        const auto name =
            absl::StrCat( "SolidMeshNegativeElements", dimension, "D" );
        pybind11::class_< SolidMeshNegativeElements >( module, name.c_str(),
            "Inspection of the negative polyhedra (with a negative volume) "
            "of a SolidMesh" )
            .def( pybind11::init< const SolidMesh& >(), pybind11::arg( "mesh" ),
                pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the negative polyhedra of this mesh. "
                "The inspector keeps the mesh alive" )
            .def( "mesh_has_negative_elements",
                &SolidMeshNegativeElements::mesh_has_negative_elements,
                ReleaseGil(),
                "Return True if a polyhedron has a negative volume" )
            .def( "negative_polyhedra",
                &SolidMeshNegativeElements::negative_polyhedra, ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "polyhedra with a negative volume" );
    }

    void define_solid_negative_elements( pybind11::module& module )
    {
        do_define_solid_negative_elements< 3 >( module );
    }
} // namespace geode
