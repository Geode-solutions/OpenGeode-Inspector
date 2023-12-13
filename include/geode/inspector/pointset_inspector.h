/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/inspector/common.h>
#include <geode/inspector/criterion/colocation/pointset_colocation.h>
#include <geode/inspector/mixin/add_inspectors.h>

namespace geode
{
    struct PointSetInspectionResult
    {
        InspectionIssues< std::vector< index_t > > colocated_points_groups;
        std::string string() const
        {
            std::string message{ "" };
            absl::StrAppend( &message, colocated_points_groups.string() );
            return message;
        }
    };

    /*!
     * Class for inspecting a PointSet
     * @extends PointSetColocation
     */
    template < index_t dimension >
    class opengeode_inspector_inspector_api PointSetInspector
        : public AddInspectors< PointSet< dimension >,
              PointSetColocation< dimension > >
    {
        OPENGEODE_DISABLE_COPY( PointSetInspector );

    public:
        PointSetInspector( const PointSet< dimension >& mesh );

        PointSetInspectionResult inspect_pointset() const;
    };
    ALIAS_2D_AND_3D( PointSetInspector );
} // namespace geode