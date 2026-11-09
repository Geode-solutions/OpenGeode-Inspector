/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/inspector/inspection/criterion/isolation/surface_isolation.hpp>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/inspector/inspection/criterion/internal/isolation_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class SurfaceMeshIsolation< dimension >::Impl
    {
    public:
        explicit Impl( const SurfaceMesh< dimension >& mesh ) : mesh_( mesh ) {}

        bool mesh_has_isolated_vertices() const
        {
            return internal::mesh_has_isolated_vertices( mesh_ );
        }

        InspectionIssues< index_t > isolated_vertices() const
        {
            return internal::isolated_vertices( mesh_ );
        }

    private:
        const SurfaceMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    SurfaceMeshIsolation< dimension >::SurfaceMeshIsolation(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SurfaceMeshIsolation< dimension >::~SurfaceMeshIsolation() = default;

    template < index_t dimension >
    bool SurfaceMeshIsolation< dimension >::mesh_has_isolated_vertices() const
    {
        return impl_->mesh_has_isolated_vertices();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SurfaceMeshIsolation< dimension >::isolated_vertices() const
    {
        return impl_->isolated_vertices();
    }

    template class opengeode_inspector_inspection_api SurfaceMeshIsolation< 2 >;
    template class opengeode_inspector_inspection_api SurfaceMeshIsolation< 3 >;
} // namespace geode
