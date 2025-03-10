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

#include <geode/inspector/criterion/colocation/pointset_colocation.hpp>
#include <geode/inspector/criterion/internal/colocation_impl.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/point_set.hpp>

namespace geode
{
    template < index_t dimension >
    class PointSetColocation< dimension >::Impl
        : public internal::ColocationImpl< dimension, PointSet< dimension > >
    {
    public:
        Impl( const PointSet< dimension >& mesh )
            : internal::ColocationImpl< dimension, PointSet< dimension > >(
                  mesh )
        {
        }
    };

    template < index_t dimension >
    PointSetColocation< dimension >::PointSetColocation(
        const PointSet< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    PointSetColocation< dimension >::~PointSetColocation() = default;

    template < index_t dimension >
    bool PointSetColocation< dimension >::mesh_has_colocated_points() const
    {
        return impl_->mesh_has_colocated_points();
    }

    template < index_t dimension >
    InspectionIssues< std::vector< index_t > >
        PointSetColocation< dimension >::colocated_points_groups() const
    {
        return impl_->colocated_points_groups();
    }

    template class opengeode_inspector_inspector_api PointSetColocation< 2 >;
    template class opengeode_inspector_inspector_api PointSetColocation< 3 >;
} // namespace geode
