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
#include <string>

#include <geode/mesh/core/point_set.h>

#include <geode/inspector/pointset_inspector.h>

namespace geode
{
    template < index_t dimension >
    void do_define_pointset_inspector( pybind11::module& module )
    {
        using PointSet = PointSet< dimension >;
        using PointSetInspector = PointSetInspector< dimension >;
        const auto name = absl::StrCat( "PointSetInspector", dimension, "D" );
        pybind11::class_< PointSetInspector, PointSetColocation< dimension > >(
            module, name.c_str() )
            .def( pybind11::init< const PointSet& >() );
    }
    void define_pointset_inspector( pybind11::module& module )
    {
        pybind11::class_< PointSetInspectionResult >(
            module, "PointSetInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "colocated_points_groups",
                &PointSetInspectionResult::colocated_points_groups )
            .def( "string", &PointSetInspectionResult::string );

        do_define_pointset_inspector< 2 >( module );
        do_define_pointset_inspector< 3 >( module );
    }
} // namespace geode
