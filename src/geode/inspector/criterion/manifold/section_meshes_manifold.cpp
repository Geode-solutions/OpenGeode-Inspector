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

#include <geode/inspector/criterion/manifold/section_meshes_manifold.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/private/component_meshes_manifold.h>

namespace geode
{
    class SectionComponentMeshesManifold::Impl
        : public ComponentMeshesManifold< 2, Section >
    {
    public:
        Impl( const Section& section, bool verbose )
            : ComponentMeshesManifold< 2, Section >( section, verbose )
        {
        }
    };

    SectionComponentMeshesManifold::SectionComponentMeshesManifold(
        const Section& model )
        : impl_{ model, false }
    {
    }

    SectionComponentMeshesManifold::SectionComponentMeshesManifold(
        const Section& model, bool verbose )
        : impl_{ model, verbose }
    {
    }

    SectionComponentMeshesManifold::~SectionComponentMeshesManifold() {}

    std::vector< uuid >
        SectionComponentMeshesManifold::components_non_manifold_meshes() const
    {
        return impl_->surfaces_non_manifold_meshes();
    }

    absl::flat_hash_map< uuid, index_t > SectionComponentMeshesManifold::
        component_meshes_nb_non_manifold_vertices() const
    {
        return impl_->surfaces_meshes_nb_non_manifold_vertices();
    }

    absl::flat_hash_map< uuid, index_t >
        SectionComponentMeshesManifold::component_meshes_nb_non_manifold_edges()
            const
    {
        return impl_->surfaces_meshes_nb_non_manifold_edges();
    }

    absl::flat_hash_map< uuid, std::vector< index_t > >
        SectionComponentMeshesManifold::component_meshes_non_manifold_vertices()
            const
    {
        return impl_->surfaces_meshes_non_manifold_vertices();
    }

    absl::flat_hash_map< uuid, std::vector< std::array< index_t, 2 > > >
        SectionComponentMeshesManifold::component_meshes_non_manifold_edges()
            const
    {
        return impl_->surfaces_meshes_non_manifold_edges();
    }
} // namespace geode
