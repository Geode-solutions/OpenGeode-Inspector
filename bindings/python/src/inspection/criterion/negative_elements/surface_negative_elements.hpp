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

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/inspector/inspection/criterion/negative_elements/surface_negative_elements.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    template < index_t dimension >
    void do_define_surface_negative_elements( pybind11::module& module )
    {
        using SurfaceMesh = SurfaceMesh< dimension >;
        using SurfaceMeshNegativeElements =
            SurfaceMeshNegativeElements< dimension >;
        const auto name =
            absl::StrCat( "SurfaceMeshNegativeElements", dimension, "D" );
        pybind11::class_< SurfaceMeshNegativeElements >( module, name.c_str(),
            "Inspection of the negative polygons (with a negative area, "
            "clockwise polygons) of a SurfaceMesh. A polygon of a 3D mesh has "
            "no orientation: the inspection of a 3D mesh never finds an "
            "issue" )
            .def( pybind11::init< const SurfaceMesh& >(),
                pybind11::arg( "mesh" ), pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the negative polygons of this mesh. "
                "The inspector keeps the mesh alive" )
            .def( "mesh_has_negative_elements",
                &SurfaceMeshNegativeElements::mesh_has_negative_elements,
                ReleaseGil(),
                "Return True if a polygon has a negative area (always False "
                "for a 3D mesh)" )
            .def( "negative_polygons",
                &SurfaceMeshNegativeElements::negative_polygons, ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "polygons with a negative area (always empty for a 3D "
                "mesh)" );
    }

    void define_surface_negative_elements( pybind11::module& module )
    {
        do_define_surface_negative_elements< 2 >( module );
        do_define_surface_negative_elements< 3 >( module );
    }
} // namespace geode
