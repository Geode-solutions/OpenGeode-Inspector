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

#include <geode/inspector/inspection/criterion/degeneration/edgedcurve_degeneration.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    template < index_t dimension >
    void do_define_edged_curve_degeneration( pybind11::module& module )
    {
        using EdgedCurve = EdgedCurve< dimension >;
        using EdgedCurveDegeneration = EdgedCurveDegeneration< dimension >;
        const auto name =
            absl::StrCat( "EdgedCurveDegeneration", dimension, "D" );
        pybind11::class_< EdgedCurveDegeneration >( module, name.c_str(),
            "Inspection of the degenerated edges of an EdgedCurve" )
            .def( pybind11::init< const EdgedCurve& >(),
                pybind11::arg( "mesh" ),
                "Create the inspector of the degenerated edges of this mesh. "
                "The inspector keeps the mesh alive" )
            .def( "is_mesh_degenerated",
                &EdgedCurveDegeneration::is_mesh_degenerated, ReleaseGil(),
                "Return True if an edge is degenerated: its length is at "
                "most GLOBAL_EPSILON (1e-6)" )
            .def( "degenerated_edges",
                &EdgedCurveDegeneration::degenerated_edges, ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "degenerated edges (length at most GLOBAL_EPSILON (1e-6))" )
            .def( "small_edges", &EdgedCurveDegeneration::small_edges,
                pybind11::arg( "threshold" ), ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "edges whose length is at most threshold" );
    }

    void define_edged_curve_degeneration( pybind11::module& module )
    {
        do_define_edged_curve_degeneration< 2 >( module );
        do_define_edged_curve_degeneration< 3 >( module );
    }
} // namespace geode
