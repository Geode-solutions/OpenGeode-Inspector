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

#include <geode/inspector/edgedcurve_inspector.hpp>

#include <async++.h>

#include <geode/mesh/core/edged_curve.hpp>

namespace geode
{
    index_t EdgedCurveInspectionResult::nb_issues() const
    {
        return colocated_points_groups.nb_issues()
               + degenerated_edges.nb_issues();
    }

    std::string EdgedCurveInspectionResult::string() const
    {
        return absl::StrCat(
            colocated_points_groups.string(), degenerated_edges.string() );
    }

    std::string EdgedCurveInspectionResult::inspection_type() const
    {
        return "edged curve inspection";
    }

    template < index_t dimension >
    EdgedCurveInspector< dimension >::EdgedCurveInspector(
        const EdgedCurve< dimension >& mesh )
        : AddInspectors< EdgedCurve< dimension >,
              EdgedCurveColocation< dimension >,
              EdgedCurveDegeneration< dimension > >{ mesh }
    {
    }

    template < index_t dimension >
    EdgedCurveInspectionResult
        EdgedCurveInspector< dimension >::inspect_edged_curve() const
    {
        EdgedCurveInspectionResult result;
        async::parallel_invoke(
            [&result, this] {
                result.colocated_points_groups =
                    this->colocated_points_groups();
            },
            [&result, this] {
                result.degenerated_edges = this->degenerated_edges();
            } );
        return result;
    }

    template class opengeode_inspector_inspector_api EdgedCurveInspector< 2 >;
    template class opengeode_inspector_inspector_api EdgedCurveInspector< 3 >;
} // namespace geode
