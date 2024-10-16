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

#include <geode/inspector/criterion/colocation/solid_colocation.hpp>
#include <geode/inspector/criterion/internal/colocation_impl.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    template < index_t dimension >
    class SolidMeshColocation< dimension >::Impl
        : public internal::ColocationImpl< dimension, SolidMesh< dimension > >
    {
    public:
        Impl( const SolidMesh< dimension >& mesh )
            : internal::ColocationImpl< dimension, SolidMesh< dimension > >(
                  mesh )
        {
        }
    };

    template < index_t dimension >
    SolidMeshColocation< dimension >::SolidMeshColocation(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshColocation< dimension >::~SolidMeshColocation() = default;

    template < index_t dimension >
    bool SolidMeshColocation< dimension >::mesh_has_colocated_points() const
    {
        return impl_->mesh_has_colocated_points();
    }

    template < index_t dimension >
    InspectionIssues< std::vector< index_t > >
        SolidMeshColocation< dimension >::colocated_points_groups() const
    {
        return impl_->colocated_points_groups();
    }

    template class opengeode_inspector_inspector_api SolidMeshColocation< 3 >;
} // namespace geode
