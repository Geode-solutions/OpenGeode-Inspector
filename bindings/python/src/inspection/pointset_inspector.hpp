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

#include <geode/mesh/core/point_set.hpp>

#include <geode/inspector/inspection/pointset_inspector.hpp>

#include <geode/inspector/bindings/python/inspection_helpers.hpp>

namespace geode
{
    template < index_t dimension >
    void do_define_point_set_inspector( pybind11::module& module )
    {
        using PointSet = PointSet< dimension >;
        using PointSetInspector = PointSetInspector< dimension >;
        const auto name = absl::StrCat( "PointSetInspector", dimension, "D" );
        pybind11::class_< PointSetInspector, PointSetColocation< dimension > >(
            module, name.c_str(),
            "Complete inspection of a PointSet: the colocation of its "
            "vertices (PointSetColocation, whose methods it also has)" )
            .def( pybind11::init< const PointSet& >(), pybind11::arg( "mesh" ),
                pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of this mesh. The inspector keeps the "
                "mesh alive" )
            .def( "inspect_point_set", &PointSetInspector::inspect_point_set,
                ReleaseGil(),
                "Return the PointSetInspectionResult: the groups of "
                "colocated vertices" );

        const auto inspect_function_name =
            absl::StrCat( "inspect_point_set", dimension, "D" );
        module.def(
            inspect_function_name.c_str(),
            []( const PointSet& point_set ) {
                const PointSetInspector inspector{ point_set };
                return inspector.inspect_point_set();
            },
            pybind11::arg( "point_set" ), ReleaseGil(),
            "Inspect the PointSet with a PointSetInspector and return the "
            "PointSetInspectionResult" );
    }

    void define_point_set_inspector( pybind11::module& module )
    {
        auto result = define_inspection_result< PointSetInspectionResult >(
            module, "PointSetInspectionResult",
            "Result of the inspection of a PointSet" );
        INSPECTION_RESULT_MEMBER( result, PointSetInspectionResult,
            colocated_points_groups,
            "InspectionIssuesVectorIndex: the groups of colocated vertices" );

        do_define_point_set_inspector< 2 >( module );
        do_define_point_set_inspector< 3 >( module );
    }
} // namespace geode
