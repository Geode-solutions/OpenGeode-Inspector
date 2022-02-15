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

#include <geode/inspector/criterion/degeneration/solid_degeneration.h>
#include <geode/inspector/criterion/private/degeneration_impl.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    template < index_t dimension >
    class SolidMeshDegeneration< dimension >::Impl
        : public detail::DegenerationImpl< SolidMesh< dimension > >
    {
    public:
        Impl( const SolidMesh< dimension >& mesh )
            : detail::DegenerationImpl< SolidMesh< dimension > >{ mesh }
        {
        }
    };

    template < index_t dimension >
    SolidMeshDegeneration< dimension >::SolidMeshDegeneration(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshDegeneration< dimension >::~SolidMeshDegeneration()
    {
    }

    template < index_t dimension >
    bool SolidMeshDegeneration< dimension >::is_mesh_degenerated() const
    {
        return impl_->is_mesh_degenerated();
    }

    template < index_t dimension >
    index_t SolidMeshDegeneration< dimension >::nb_degenerated_edges() const
    {
        return impl_->nb_degenerated_edges();
    }

    template < index_t dimension >
    std::vector< index_t >
        SolidMeshDegeneration< dimension >::degenerated_edges() const
    {
        return impl_->degenerated_edges();
    }

    template class opengeode_inspector_inspector_api SolidMeshDegeneration< 3 >;
} // namespace geode
