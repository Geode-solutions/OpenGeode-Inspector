/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/core/point_set.h>

#include <geode/inspector/pointset_inspector.h>

#define PYTHON_POINTSET_INSPECTOR( dimension )                                 \
    const auto name##dimension =                                               \
        "PointSetInspector" + std::to_string( dimension ) + "D";               \
    pybind11::class_< PointSetInspector##dimension##D,                         \
        PointSetColocation##dimension##D >( module, name##dimension.c_str() )  \
        .def( pybind11::init< const PointSet< dimension >& >() )               \
        .def( pybind11::init< const PointSet< dimension >&, bool >() )

namespace geode
{
    void define_pointset_inspector( pybind11::module& module )
    {
        PYTHON_POINTSET_INSPECTOR( 2 );
        PYTHON_POINTSET_INSPECTOR( 3 );
    }
} // namespace geode
