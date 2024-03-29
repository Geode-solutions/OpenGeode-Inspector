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

#include <geode/inspector/criterion/section_meshes_inspector.h>

namespace geode
{
    std::string SectionMeshesInspectionResult::string() const
    {
        return absl::StrCat( unique_vertices_colocation.string(), "\n",
            meshes_colocation.string(), "\n", adjacencies.string(), "\n",
            degenerations.string(), "\n", intersections.string(), "\n",
            manifolds.string(), "\n" );
    }

    SectionMeshesInspector::SectionMeshesInspector( const Section& section )
        : SectionUniqueVerticesColocation( section ),
          SectionComponentMeshesAdjacency( section ),
          SectionComponentMeshesColocation( section ),
          SectionComponentMeshesDegeneration( section ),
          SectionComponentMeshesManifold( section ),
          SectionMeshesIntersections( section )
    {
    }

    SectionMeshesInspectionResult
        SectionMeshesInspector::inspect_section_meshes() const
    {
        SectionMeshesInspectionResult result;
        result.unique_vertices_colocation = inspect_unique_vertices();
        result.meshes_colocation = inspect_meshes_point_colocations();
        result.adjacencies = inspect_section_meshes_adjacencies();
        result.degenerations = inspect_elements();
        result.intersections = inspect_intersections();
        result.manifolds = inspect_section_manifold();
        return result;
    }
} // namespace geode
