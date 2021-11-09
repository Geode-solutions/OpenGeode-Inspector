/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/inspector/criterion/colocalization/solid_colocalization.h>
#include <geode/inspector/criterion/detail/colocalization_impl.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    template < index_t dimension >
    class SolidMeshColocalization< dimension >::Impl
        : public detail::ColocalizationImpl< SolidMesh< dimension > >
    {
    public:
        Impl( const SolidMesh< dimension >& mesh )
            : detail::DegenerationImpl< SolidMesh< dimension > >( mesh )
        {
        }
    };

    template < index_t dimension >
    SolidMeshColocalization< dimension >::SolidMeshColocalization(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshColocalization< dimension >::~SolidMeshColocalization()
    {
    }

    template < index_t dimension >
    bool SolidMeshColocalization< dimension >::mesh_has_colocalized_points()
        const
    {
        return impl_->mesh_has_colocalized_points( mesh );
    }

    template < index_t dimension >
    index_t SolidMeshColocalization< dimension >::nb_colocalized_points() const
    {
        return impl_->nb_colocalized_points( mesh );
    }

    template < index_t dimension >
    const std::vector< std::vector< index_t > >
        SolidMeshColocalization< dimension >::colocalized_points_groups() const
    {
        return impl_->colocalized_points_groups( mesh );
    }

    template class opengeode_inspector_inspector_api
        SolidMeshColocalization< 2 >;
    template class opengeode_inspector_inspector_api
        SolidMeshColocalization< 3 >;
} // namespace geode
