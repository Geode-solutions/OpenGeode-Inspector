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

#include <geode/inspector/criterion/adjacency/section_meshes_adjacency.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/internal/component_meshes_adjacency.hpp>

namespace geode
{
    class SectionComponentMeshesAdjacency::Impl
        : public internal::ComponentMeshesAdjacency< Section >
    {
    public:
        Impl( const Section& section )
            : internal::ComponentMeshesAdjacency< Section >( section )
        {
        }
    };

    index_t SectionMeshesAdjacencyInspectionResult::nb_issues() const
    {
        return surfaces_edges_with_wrong_adjacencies.nb_issues();
    }

    std::string SectionMeshesAdjacencyInspectionResult::string() const
    {
        if( surfaces_edges_with_wrong_adjacencies.nb_issues() != 0 )
        {
            return surfaces_edges_with_wrong_adjacencies.string();
        }
        return "No adjacency issues in model \n";
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

    SectionComponentMeshesAdjacency::~SectionComponentMeshesAdjacency() =
        default;

    SectionMeshesAdjacencyInspectionResult
        SectionComponentMeshesAdjacency::inspect_section_meshes_adjacencies()
            const
    {
        SectionMeshesAdjacencyInspectionResult result;
        impl_->add_surfaces_edges_with_wrong_adjacencies(
            result.surfaces_edges_with_wrong_adjacencies );
        return result;
    }
} // namespace geode
