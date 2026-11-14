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
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/inspector/inspection/criterion/intersections/surface_curve_intersections.hpp>

#include <geode/inspector/bindings/python/inspection_helpers.hpp>

namespace geode
{
    template < index_t dimension >
    void do_define_surface_curve_intersections( pybind11::module& module )
    {
        using TriangulatedSurface = TriangulatedSurface< dimension >;
        using EdgedCurve = EdgedCurve< dimension >;
        using SurfaceCurveIntersections =
            SurfaceCurveIntersections< dimension >;
        const auto name =
            absl::StrCat( "SurfaceCurveIntersections", dimension, "D" );
        pybind11::class_< SurfaceCurveIntersections >( module, name.c_str(),
            "Inspection of the intersections between the triangles of a "
            "TriangulatedSurface and the edges of an EdgedCurve" )
            .def( pybind11::init< const TriangulatedSurface&,
                      const EdgedCurve& >(),
                pybind11::arg( "surface" ), pybind11::arg( "curve" ),
                pybind11::keep_alive< 1, 2 >(), pybind11::keep_alive< 1, 3 >(),
                "Create the inspector of the intersections between this "
                "surface and this curve. The inspector keeps both meshes "
                "alive" )
            .def( "meshes_have_intersections",
                &SurfaceCurveIntersections::meshes_have_intersections,
                ReleaseGil(),
                "Return True if an edge of the curve intersects a triangle of "
                "the surface" )
            .def( "intersecting_elements",
                &SurfaceCurveIntersections::intersecting_elements, ReleaseGil(),
                "Return the InspectionIssuesPairIndex listing the pairs of "
                "intersecting elements: the first index of each pair is a "
                "triangle of the surface, the second one an edge of the "
                "curve" );
    }

    void define_surface_curve_intersections( pybind11::module& module )
    {
        do_define_surface_curve_intersections< 2 >( module );
        do_define_surface_curve_intersections< 3 >( module );
    }
} // namespace geode
