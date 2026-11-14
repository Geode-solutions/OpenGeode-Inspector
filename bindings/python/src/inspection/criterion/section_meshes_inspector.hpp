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
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/inspection/criterion/section_meshes_inspector.hpp>

#include "../../inspection_helpers.hpp"

namespace geode
{
    void define_section_meshes_inspector( pybind11::module& module )
    {
        auto result = define_inspection_result< SectionMeshesInspectionResult >(
            module, "SectionMeshesInspectionResult",
            "Result of the inspection of the meshes of a Section: one "
            "result per criterion" );
        INSPECTION_RESULT_MEMBER( result, SectionMeshesInspectionResult,
            unique_vertices_colocation,
            "UniqueVerticesInspectionResult: the colocation of the unique "
            "vertices" );
        INSPECTION_RESULT_MEMBER( result, SectionMeshesInspectionResult,
            meshes_colocation,
            "MeshesColocationInspectionResult: the colocated vertices of the "
            "component meshes" );
        INSPECTION_RESULT_MEMBER( result, SectionMeshesInspectionResult,
            meshes_adjacencies,
            "SectionMeshesAdjacencyInspectionResult: the adjacencies of the "
            "component meshes" );
        INSPECTION_RESULT_MEMBER( result, SectionMeshesInspectionResult,
            meshes_degenerations,
            "SectionMeshesDegenerationInspectionResult: the degenerated "
            "elements of the component meshes" );
        INSPECTION_RESULT_MEMBER( result, SectionMeshesInspectionResult,
            meshes_intersections,
            "ElementsIntersectionsInspectionResult: the intersections "
            "between the component meshes" );
        INSPECTION_RESULT_MEMBER( result, SectionMeshesInspectionResult,
            meshes_non_manifolds,
            "SectionMeshesManifoldInspectionResult: the non manifold "
            "vertices and edges" );
        INSPECTION_RESULT_MEMBER( result, SectionMeshesInspectionResult,
            meshes_negative_elements,
            "SectionMeshesNegativeElementsInspectionResult: the negative "
            "polygons of the Surfaces" );

        pybind11::class_< SectionMeshesInspector,
            SectionUniqueVerticesColocation, SectionComponentMeshesAdjacency,
            SectionComponentMeshesColocation,
            SectionComponentMeshesDegeneration, SectionComponentMeshesManifold,
            SectionComponentMeshesNegativeElements,
            SectionMeshesIntersections >( module, "SectionMeshesInspector",
            "Inspection of the meshes of a Section: the unique vertices "
            "colocation, and the adjacencies, colocation, degeneration, "
            "manifold property, negative elements and intersections of the "
            "component meshes. It also has the methods of each of these "
            "inspectors" )
            .def( pybind11::init< const Section& >(),
                pybind11::arg( "section" ), pybind11::keep_alive< 1, 2 >(),
                ReleaseGil(),
                "Create the inspector of the meshes of this Section. The "
                "inspector keeps the Section alive" )
            .def( "inspect_section_meshes",
                &SectionMeshesInspector::inspect_section_meshes, ReleaseGil(),
                "Return the SectionMeshesInspectionResult: the result of "
                "every inspection of the meshes of the Section" );
    }
} // namespace geode
