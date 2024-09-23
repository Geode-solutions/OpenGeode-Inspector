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

#pragma once

#include <geode/inspector/common.hpp>
#include <geode/inspector/criterion/adjacency/section_meshes_adjacency.hpp>
#include <geode/inspector/criterion/colocation/component_meshes_colocation.hpp>
#include <geode/inspector/criterion/colocation/unique_vertices_colocation.hpp>
#include <geode/inspector/criterion/degeneration/section_meshes_degeneration.hpp>
#include <geode/inspector/criterion/intersections/model_intersections.hpp>
#include <geode/inspector/criterion/manifold/section_meshes_manifold.hpp>
#include <geode/inspector/criterion/negative_elements/section_meshes_negative_elements.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    struct opengeode_inspector_inspector_api SectionMeshesInspectionResult
    {
        UniqueVerticesInspectionResult unique_vertices_colocation;
        MeshesColocationInspectionResult meshes_colocation;
        SectionMeshesAdjacencyInspectionResult meshes_adjacencies;
        SectionMeshesDegenerationInspectionResult meshes_degenerations;
        ElementsIntersectionsInspectionResult meshes_intersections;
        SectionMeshesManifoldInspectionResult meshes_non_manifolds;
        SectionMeshesNegativeElementsInspectionResult meshes_negative_elements;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };
    /*!
     * Class for inspecting a Section model
     * @extends SectionMeshesInspector
     */
    class opengeode_inspector_inspector_api SectionMeshesInspector
        : public SectionUniqueVerticesColocation,
          public SectionComponentMeshesAdjacency,
          public SectionComponentMeshesColocation,
          public SectionComponentMeshesDegeneration,
          public SectionComponentMeshesManifold,
          public SectionComponentMeshesNegativeElements,
          public SectionMeshesIntersections
    {
        OPENGEODE_DISABLE_COPY( SectionMeshesInspector );

    public:
        explicit SectionMeshesInspector( const Section& section );

        [[nodiscard]] SectionMeshesInspectionResult
            inspect_section_meshes() const;
    };
} // namespace geode