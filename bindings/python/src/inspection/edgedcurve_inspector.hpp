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

#include <string>

#include <geode/mesh/core/edged_curve.hpp>

#include <geode/inspector/inspection/edgedcurve_inspector.hpp>

#include "../inspection_helpers.hpp"

namespace geode
{
    template < index_t dimension >
    void do_define_edged_curve_inspector( pybind11::module& module )
    {
        using EdgedCurve = EdgedCurve< dimension >;
        using EdgedCurveInspector = EdgedCurveInspector< dimension >;
        const auto name = absl::StrCat( "EdgedCurveInspector", dimension, "D" );
        pybind11::class_< EdgedCurveInspector,
            EdgedCurveColocation< dimension >,
            EdgedCurveDegeneration< dimension >,
            EdgedCurveIsolation< dimension > >( module, name.c_str(),
            "Complete inspection of an EdgedCurve: the colocation, the "
            "degeneration and the isolation of its vertices and edges "
            "(EdgedCurveColocation, EdgedCurveDegeneration and "
            "EdgedCurveIsolation, whose methods it also has)" )
            .def( pybind11::init< const EdgedCurve& >(),
                pybind11::arg( "mesh" ),
                "Create the inspector of this mesh. The inspector keeps the "
                "mesh alive" )
            .def( "inspect_edged_curve",
                &EdgedCurveInspector::inspect_edged_curve, ReleaseGil(),
                "Return the EdgedCurveInspectionResult: the colocated "
                "vertices, the degenerated edges and the isolated vertices" );

        const auto inspect_function_name =
            absl::StrCat( "inspect_edged_curve", dimension, "D" );
        module.def(
            inspect_function_name.c_str(),
            []( const EdgedCurve& edged_curve ) {
                const EdgedCurveInspector inspector{ edged_curve };
                return inspector.inspect_edged_curve();
            },
            pybind11::arg( "edged_curve" ), ReleaseGil(),
            "Inspect the EdgedCurve with an EdgedCurveInspector and return "
            "the EdgedCurveInspectionResult" );
    }

    void define_edged_curve_inspector( pybind11::module& module )
    {
        auto result = define_inspection_result< EdgedCurveInspectionResult >(
            module, "EdgedCurveInspectionResult",
            "Result of the inspection of an EdgedCurve" );
        INSPECTION_RESULT_MEMBER( result, EdgedCurveInspectionResult,
            colocated_points_groups,
            "InspectionIssuesVectorIndex: the groups of colocated vertices" );
        INSPECTION_RESULT_MEMBER( result, EdgedCurveInspectionResult,
            degenerated_edges,
            "InspectionIssuesIndex: the indices of the degenerated edges" );
        INSPECTION_RESULT_MEMBER( result, EdgedCurveInspectionResult,
            isolated_vertices,
            "InspectionIssuesIndex: the indices of the vertices that are in "
            "no edge" );

        do_define_edged_curve_inspector< 2 >( module );
        do_define_edged_curve_inspector< 3 >( module );
    }
} // namespace geode
