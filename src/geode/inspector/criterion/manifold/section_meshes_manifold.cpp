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
        Impl( const Section& section )
            : ComponentMeshesManifold< 2, Section >( section )
        {
        }
    };

    SectionComponentMeshesManifold::SectionComponentMeshesManifold(
        const Section& model )
        : impl_{ model }
    {
    }

    std::string SectionMeshesManifoldInspectionResult::string() const
    {
        std::string message{ "" };
        for( const auto& vertices_issue : meshes_non_manifold_vertices )
        {
            absl::StrAppend( &message, vertices_issue.second.string(), "\n" );
        }
        for( const auto& edges_issue : meshes_non_manifold_edges )
        {
            absl::StrAppend( &message, edges_issue.second.string(), "\n" );
        }
        if( message == "" )
        {
            absl::StrAppend(
                &message, "No manifold issues in model component meshes" );
        }
        return message;
    }

    SectionComponentMeshesManifold::~SectionComponentMeshesManifold() {}

    SectionMeshesManifoldInspectionResult
        SectionComponentMeshesManifold::inspect_section_manifold() const
    {
        SectionMeshesManifoldInspectionResult result;
        result.meshes_non_manifold_vertices =
            impl_->surfaces_meshes_non_manifold_vertices();
        result.meshes_non_manifold_edges =
            impl_->surfaces_meshes_non_manifold_edges();
        return result;
    }
} // namespace geode
