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

#include <geode/inspector/criterion/adjacency/section_meshes_adjacency.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/private/component_meshes_adjacency.h>

namespace geode
{
    class SectionComponentMeshesAdjacency::Impl
        : public ComponentMeshesAdjacency< 2, Section >
    {
    public:
        Impl( const Section& section )
            : ComponentMeshesAdjacency< 2, Section >( section )
        {
        }
    };

    std::string SectionMeshesAdjacencyInspectionResult::string() const
    {
        std::string message;
        for( const auto& surface_issue : surfaces_edges_with_wrong_adjacencies )
        {
            absl::StrAppend( &message, surface_issue.second.string(), "\n" );
        }
        if( surfaces_edges_with_wrong_adjacencies.empty() )
        {
            absl::StrAppend(
                &message, "No adjacency issues in model component meshes" );
        }
        return message;
    }

    std::string SectionMeshesAdjacencyInspectionResult::inspection_type() const
    {
        return "Adjacencies inspection";
    }

    SectionComponentMeshesAdjacency::SectionComponentMeshesAdjacency(
        const Section& model )
        : impl_{ model }
    {
    }

    SectionComponentMeshesAdjacency::~SectionComponentMeshesAdjacency() {}

    SectionMeshesAdjacencyInspectionResult
        SectionComponentMeshesAdjacency::inspect_section_meshes_adjacencies()
            const
    {
        SectionMeshesAdjacencyInspectionResult result;
        result.surfaces_edges_with_wrong_adjacencies =
            impl_->surfaces_edges_with_wrong_adjacencies();
        return result;
    }
} // namespace geode
