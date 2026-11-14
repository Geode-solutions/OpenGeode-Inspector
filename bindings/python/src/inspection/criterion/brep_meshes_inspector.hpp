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
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/inspection/criterion/brep_meshes_inspector.hpp>

#include "../../inspection_helpers.hpp"

namespace geode
{
    void define_brep_meshes_inspector( pybind11::module& module )
    {
        auto result = define_inspection_result< BRepMeshesInspectionResult >(
            module, "BRepMeshesInspectionResult",
            "Result of the inspection of the meshes of a BRep: one "
            "result per criterion" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesInspectionResult,
            unique_vertices_colocation,
            "UniqueVerticesInspectionResult: the colocation of the unique "
            "vertices" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesInspectionResult,
            meshes_colocation,
            "MeshesColocationInspectionResult: the colocated vertices of the "
            "component meshes" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesInspectionResult,
            meshes_adjacencies,
            "BRepMeshesAdjacencyInspectionResult: the adjacencies of the "
            "component meshes" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesInspectionResult,
            meshes_degenerations,
            "BRepMeshesDegenerationInspectionResult: the degenerated "
            "elements of the component meshes" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesInspectionResult,
            meshes_intersections,
            "ElementsIntersectionsInspectionResult: the intersections "
            "between the component meshes" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesInspectionResult,
            meshes_non_manifolds,
            "BRepMeshesManifoldInspectionResult: the non manifold vertices, "
            "edges and facets" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesInspectionResult,
            meshes_negative_elements,
            "BRepMeshesNegativeElementsInspectionResult: the negative "
            "polyhedra of the Blocks" );

        pybind11::class_< BRepMeshesInspector, BRepUniqueVerticesColocation,
            BRepComponentMeshesAdjacency, BRepComponentMeshesColocation,
            BRepComponentMeshesDegeneration, BRepComponentMeshesManifold,
            BRepComponentMeshesNegativeElements, BRepMeshesIntersections >(
            module, "BRepMeshesInspector",
            "Inspection of the meshes of a BRep: the unique vertices "
            "colocation, and the adjacencies, colocation, degeneration, "
            "manifold property, negative elements and intersections of the "
            "component meshes. It also has the methods of each of these "
            "inspectors" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                pybind11::keep_alive< 1, 2 >(), ReleaseGil(),
                "Create the inspector of the meshes of this BRep. The "
                "inspector keeps the BRep alive" )
            .def( "inspect_brep_meshes",
                &BRepMeshesInspector::inspect_brep_meshes, ReleaseGil(),
                "Return the BRepMeshesInspectionResult: the result of every "
                "inspection of the meshes of the BRep" );
    }
} // namespace geode
