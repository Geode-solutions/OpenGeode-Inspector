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

#include <geode/inspector/criterion/section_meshes_inspector.hpp>

#include <async++.h>

namespace geode
{
    index_t SectionMeshesInspectionResult::nb_issues() const
    {
        return unique_vertices_colocation.nb_issues()
               + meshes_colocation.nb_issues() + meshes_adjacencies.nb_issues()
               + meshes_degenerations.nb_issues()
               + meshes_intersections.nb_issues()
               + meshes_non_manifolds.nb_issues()
               + meshes_negative_elements.nb_issues();
    }

    std::string SectionMeshesInspectionResult::string() const
    {
        return absl::StrCat( unique_vertices_colocation.string(),
            meshes_colocation.string(), meshes_adjacencies.string(),
            meshes_degenerations.string(), meshes_intersections.string(),
            meshes_non_manifolds.string(), meshes_negative_elements.string() );
    }

    std::string SectionMeshesInspectionResult::inspection_type() const
    {
        return "model meshes inspection";
    }

    SectionMeshesInspector::SectionMeshesInspector( const Section& section )
        : SectionUniqueVerticesColocation( section ),
          SectionComponentMeshesAdjacency( section ),
          SectionComponentMeshesColocation( section ),
          SectionComponentMeshesDegeneration( section ),
          SectionComponentMeshesManifold( section ),
          SectionComponentMeshesNegativeElements( section ),
          SectionMeshesIntersections( section )
    {
    }

    SectionMeshesInspectionResult
        SectionMeshesInspector::inspect_section_meshes() const
    {
        SectionMeshesInspectionResult result;
        async::parallel_invoke(
            [&result, this] {
                result.unique_vertices_colocation = inspect_unique_vertices();
            },
            [&result, this] {
                result.meshes_colocation = inspect_meshes_point_colocations();
            },
            [&result, this] {
                result.meshes_adjacencies =
                    inspect_section_meshes_adjacencies();
            },
            [&result, this] {
                result.meshes_degenerations = inspect_elements_degeneration();
            },
            [&result, this] {
                result.meshes_intersections = inspect_intersections();
            },
            [&result, this] {
                result.meshes_non_manifolds = inspect_section_manifold();
            },
            [&result, this] {
                result.meshes_negative_elements = inspect_negative_elements();
            } );
        return result;
    }
} // namespace geode
