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

#include <geode/inspector/inspection/criterion/adjacency/section_meshes_adjacency.hpp>

#include <geode/inspector/bindings/python/inspection_helpers.hpp>

namespace geode
{
    void define_section_meshes_adjacency( pybind11::module& module )
    {
        auto result =
            define_inspection_result< SectionMeshesAdjacencyInspectionResult >(
                module, "SectionMeshesAdjacencyInspectionResult",
                "Result of the inspection of the adjacencies in the meshes of "
                "the Surfaces of a Section" );
        INSPECTION_RESULT_MEMBER( result,
            SectionMeshesAdjacencyInspectionResult,
            surfaces_edges_with_wrong_adjacencies,
            "InspectionIssuesMapPolygonEdge: the polygon edges with a wrong "
            "adjacency, by Surface" );

        pybind11::class_< SectionComponentMeshesAdjacency >( module,
            "SectionComponentMeshesAdjacency",
            "Inspection of the adjacencies between the polygons of the meshes "
            "of the Surfaces of a Section" )
            .def( pybind11::init< const Section& >(), pybind11::arg( "model" ),
                pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the adjacencies of the meshes of "
                "this Section. The inspector keeps the Section alive" )
            .def( "inspect_section_meshes_adjacencies",
                &SectionComponentMeshesAdjacency::
                    inspect_section_meshes_adjacencies,
                ReleaseGil(),
                "Return the SectionMeshesAdjacencyInspectionResult for the "
                "active Surfaces: the wrong adjacencies of their meshes (as "
                "SurfaceMeshAdjacency finds them) and the polygon edges on "
                "the border of a Surface that are not part of a Line of the "
                "model" );
    }
} // namespace geode
