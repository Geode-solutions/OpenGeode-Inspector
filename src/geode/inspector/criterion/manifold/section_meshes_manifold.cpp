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

#include <geode/inspector/criterion/manifold/section_meshes_manifold.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/internal/component_meshes_manifold.hpp>

namespace geode
{
    index_t SectionMeshesManifoldInspectionResult::nb_issues() const
    {
        return meshes_non_manifold_vertices.nb_issues()
               + meshes_non_manifold_edges.nb_issues();
    }

    std::string SectionMeshesManifoldInspectionResult::string() const
    {
        std::string message;
        if( meshes_non_manifold_vertices.nb_issues() != 0 )
        {
            absl::StrAppend( &message, meshes_non_manifold_vertices.string() );
        }
        if( meshes_non_manifold_edges.nb_issues() != 0 )
        {
            absl::StrAppend( &message, meshes_non_manifold_edges.string() );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "No manifold issues in model \n";
    }

    std::string SectionMeshesManifoldInspectionResult::inspection_type() const
    {
        return "Manifold inspection";
    }

    class SectionComponentMeshesManifold::Impl
        : public internal::ComponentMeshesManifold< Section >
    {
    public:
        Impl( const Section& section )
            : internal::ComponentMeshesManifold< Section >( section )
        {
        }
    };

    SectionComponentMeshesManifold::SectionComponentMeshesManifold(
        const Section& model )
        : impl_{ model }
    {
    }

    SectionComponentMeshesManifold::~SectionComponentMeshesManifold() = default;

    SectionMeshesManifoldInspectionResult
        SectionComponentMeshesManifold::inspect_section_manifold() const
    {
        SectionMeshesManifoldInspectionResult result;
        impl_->add_surfaces_meshes_non_manifold_vertices(
            result.meshes_non_manifold_vertices );
        impl_->add_surfaces_meshes_non_manifold_edges(
            result.meshes_non_manifold_edges );
        return result;
    }
} // namespace geode
