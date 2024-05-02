/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/mesh/core/edged_curve.h>

#include <geode/inspector/criterion/colocation/edgedcurve_colocation.h>

#define PYTHON_EDGEDCURVE_COLOCATION( dimension )

namespace geode
{
    template < index_t dimension >
    void do_define_edged_curve_colocation( pybind11::module& module )
    {
        using EdgedCurve = EdgedCurve< dimension >;
        using EdgedCurveColocation = EdgedCurveColocation< dimension >;

        const auto name =
            absl::StrCat( "EdgedCurveColocation", dimension, "D" );
        pybind11::class_< EdgedCurveColocation >( module, name.c_str() )
            .def( pybind11::init< const EdgedCurve& >() )
            .def( "mesh_has_colocated_points",
                &EdgedCurveColocation::mesh_has_colocated_points )
            .def( "colocated_points_groups",
                &EdgedCurveColocation::colocated_points_groups );
    }
    void define_edged_curve_colocation( pybind11::module& module )
    {
        do_define_edged_curve_colocation< 2 >( module );
        do_define_edged_curve_colocation< 3 >( module );
    }
} // namespace geode
