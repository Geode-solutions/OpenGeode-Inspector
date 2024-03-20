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

#include <geode/inspector/common.h>
#include <geode/inspector/criterion/adjacency/brep_meshes_adjacency.h>
#include <geode/inspector/criterion/colocation/component_meshes_colocation.h>
#include <geode/inspector/criterion/colocation/unique_vertices_colocation.h>
#include <geode/inspector/criterion/degeneration/component_meshes_degeneration.h>
#include <geode/inspector/criterion/intersections/model_intersections.h>
#include <geode/inspector/criterion/manifold/brep_meshes_manifold.h>

namespace geode
{
    struct opengeode_inspector_inspector_api BRepMeshesInspectionResult
    {
        UniqueVerticesInspectionResult unique_vertices_colocation;
        MeshesColocationInspectionResult meshes_colocation;
        BRepMeshesAdjacencyInspectionResult adjacencies;
        DegeneratedElementsInspectionResult degenerations;
        ElementsIntersectionsInspectionResult intersections;
        BRepMeshesManifoldInspectionResult manifolds;
        std::string string() const;
    };

    /*!
     * Class for inspecting a BRep model
     * @extends BRepMeshesInspector
     */
    class opengeode_inspector_inspector_api BRepMeshesInspector
        : public BRepUniqueVerticesColocation,
          public BRepComponentMeshesAdjacency,
          public BRepComponentMeshesColocation,
          public BRepComponentMeshesDegeneration,
          public BRepComponentMeshesManifold,
          public BRepMeshesIntersections
    {
        OPENGEODE_DISABLE_COPY( BRepMeshesInspector );

    public:
        BRepMeshesInspector( const BRep& brep );

        BRepMeshesInspectionResult inspect_brep_meshes() const;
    };
} // namespace geode