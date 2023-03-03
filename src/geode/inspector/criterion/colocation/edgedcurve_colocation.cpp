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

#include <geode/inspector/criterion/colocation/edgedcurve_colocation.h>
#include <geode/inspector/criterion/private/colocation_impl.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/edged_curve.h>

namespace geode
{
    template < index_t dimension >
    class EdgedCurveColocation< dimension >::Impl
        : public detail::ColocationImpl< dimension, EdgedCurve< dimension > >
    {
    public:
        Impl( const EdgedCurve< dimension >& mesh, bool verbose )
            : detail::ColocationImpl< dimension, EdgedCurve< dimension > >(
                mesh, verbose )
        {
        }
    };

    template < index_t dimension >
    EdgedCurveColocation< dimension >::EdgedCurveColocation(
        const EdgedCurve< dimension >& mesh )
        : impl_( mesh, false )
    {
    }

    template < index_t dimension >
    EdgedCurveColocation< dimension >::EdgedCurveColocation(
        const EdgedCurve< dimension >& mesh, bool verbose )
        : impl_( mesh, verbose )
    {
    }

    template < index_t dimension >
    EdgedCurveColocation< dimension >::~EdgedCurveColocation()
    {
    }

    template < index_t dimension >
    bool EdgedCurveColocation< dimension >::mesh_has_colocated_points() const
    {
        return impl_->mesh_has_colocated_points();
    }

    template < index_t dimension >
    index_t EdgedCurveColocation< dimension >::nb_colocated_points() const
    {
        return impl_->nb_colocated_points();
    }

    template < index_t dimension >
    std::vector< std::vector< index_t > >
        EdgedCurveColocation< dimension >::colocated_points_groups() const
    {
        return impl_->colocated_points_groups();
    }

    template class opengeode_inspector_inspector_api EdgedCurveColocation< 2 >;
    template class opengeode_inspector_inspector_api EdgedCurveColocation< 3 >;
} // namespace geode
