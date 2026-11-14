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

#include <geode/inspector/inspection/criterion/adjacency/brep_meshes_adjacency.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    void define_brep_meshes_adjacency( pybind11::module& module )
    {
        auto result =
            define_inspection_result< BRepMeshesAdjacencyInspectionResult >(
                module, "BRepMeshesAdjacencyInspectionResult",
                "Result of the inspection of the adjacencies in the meshes of "
                "the Surfaces and Blocks of a BRep" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesAdjacencyInspectionResult,
            surfaces_edges_with_wrong_adjacencies,
            "InspectionIssuesMapPolygonEdge: the polygon edges with a wrong "
            "adjacency, by Surface" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesAdjacencyInspectionResult,
            blocks_facets_with_wrong_adjacencies,
            "InspectionIssuesMapPolyhedronFacet: the polyhedron facets with "
            "a wrong adjacency, by Block" );

        pybind11::class_< BRepComponentMeshesAdjacency >( module,
            "BRepComponentMeshesAdjacency",
            "Inspection of the adjacencies between the elements of the meshes "
            "of the Surfaces (polygon edges) and of the Blocks (polyhedron "
            "facets) of a BRep" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "model" ),
                pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the adjacencies of the meshes of "
                "this BRep. The inspector keeps the BRep alive" )
            .def( "inspect_brep_meshes_adjacencies",
                &BRepComponentMeshesAdjacency::inspect_brep_meshes_adjacencies,
                ReleaseGil(),
                "Return the BRepMeshesAdjacencyInspectionResult for the active "
                "Surfaces and Blocks: the wrong adjacencies of their meshes "
                "(as SurfaceMeshAdjacency and SolidMeshAdjacency find them), "
                "the polygon edges on the border of a Surface that are not "
                "part of a Line of the model, and the polyhedron facets on "
                "the border of a Block that are not a polygon of a Surface" );
    }
} // namespace geode
