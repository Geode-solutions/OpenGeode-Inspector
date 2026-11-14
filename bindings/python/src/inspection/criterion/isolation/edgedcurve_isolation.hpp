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

#include <geode/mesh/core/edged_curve.hpp>

#include <geode/inspector/inspection/criterion/isolation/edgedcurve_isolation.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    template < index_t dimension >
    void do_define_edged_curve_isolation( pybind11::module& module )
    {
        using EdgedCurve = EdgedCurve< dimension >;
        using EdgedCurveIsolation = EdgedCurveIsolation< dimension >;
        const auto name = absl::StrCat( "EdgedCurveIsolation", dimension, "D" );
        pybind11::class_< EdgedCurveIsolation >( module, name.c_str(),
            "Inspection of the isolated vertices of an EdgedCurve: the "
            "vertices that "
            "are in no edge" )
            .def( pybind11::init< const EdgedCurve& >(),
                pybind11::arg( "mesh" ), pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the isolated vertices of this mesh. "
                "The inspector keeps the mesh alive" )
            .def( "mesh_has_isolated_vertices",
                &EdgedCurveIsolation::mesh_has_isolated_vertices, ReleaseGil(),
                "Return True if a vertex is in no edge" )
            .def( "isolated_vertices", &EdgedCurveIsolation::isolated_vertices,
                ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "vertices that are in no edge" );
    }

    void define_edged_curve_isolation( pybind11::module& module )
    {
        do_define_edged_curve_isolation< 2 >( module );
        do_define_edged_curve_isolation< 3 >( module );
    }
} // namespace geode
