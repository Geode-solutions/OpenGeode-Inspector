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

#pragma once

#include <geode/inspector/common.hpp>
#include <geode/inspector/criterion/colocation/edgedcurve_colocation.hpp>
#include <geode/inspector/criterion/degeneration/edgedcurve_degeneration.hpp>
#include <geode/inspector/mixin/add_inspectors.hpp>

namespace geode
{
    struct opengeode_inspector_inspector_api EdgedCurveInspectionResult
    {
        InspectionIssues< std::vector< index_t > > colocated_points_groups{
            "Colocation of vertices not tested"
        };
        InspectionIssues< index_t > degenerated_edges{
            "Degeneration of edges not tested"
        };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };

    /*!
     * Class for inspecting an EdgedCurve
     * @extends EdgedCurveColocation
     * @extends EdgedCurveDegeneration
     */
    template < index_t dimension >
    class EdgedCurveInspector : public AddInspectors< EdgedCurve< dimension >,
                                    EdgedCurveColocation< dimension >,
                                    EdgedCurveDegeneration< dimension > >
    {
        OPENGEODE_DISABLE_COPY( EdgedCurveInspector );

    public:
        explicit EdgedCurveInspector( const EdgedCurve< dimension >& mesh );

        [[nodiscard]] EdgedCurveInspectionResult inspect_edged_curve() const;
    };
    ALIAS_2D_AND_3D( EdgedCurveInspector );
} // namespace geode