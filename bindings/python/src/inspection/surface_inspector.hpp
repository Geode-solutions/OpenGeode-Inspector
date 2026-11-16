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

#include <string>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/inspector/inspection/surface_inspector.hpp>

#include <geode/inspector/bindings/python/inspection_helpers.hpp>

namespace geode
{
    template < index_t dimension >
    void do_define_surface_inspector( pybind11::module& module )
    {
        using SurfaceMesh = SurfaceMesh< dimension >;
        using SurfaceMeshInspector = SurfaceMeshInspector< dimension >;
        const auto name =
            absl::StrCat( "SurfaceMeshInspector", dimension, "D" );
        pybind11::class_< SurfaceMeshInspector,
            SurfaceMeshAdjacency< dimension >,
            SurfaceMeshColocation< dimension >,
            SurfaceMeshDegeneration< dimension >,
            SurfaceMeshEdgeManifold< dimension >,
            SurfaceMeshVertexManifold< dimension >,
            SurfaceMeshIntersections< dimension >,
            SurfaceMeshNegativeElements< dimension >,
            SurfaceMeshIsolation< dimension > >( module, name.c_str(),
            "Complete inspection of a SurfaceMesh: adjacency, colocation, "
            "degeneration, manifold property, intersections, negative "
            "polygons and isolation (SurfaceMeshAdjacency, "
            "SurfaceMeshColocation, ... whose methods it also has). The "
            "edges of the mesh are enabled by the inspections returning "
            "edge indices, and disabled again when the inspector is "
            "destroyed (if they were not enabled)" )
            .def( pybind11::init< const SurfaceMesh& >(),
                pybind11::arg( "mesh" ), pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of this mesh. The inspector keeps the "
                "mesh alive" )
            .def( "inspect_surface", &SurfaceMeshInspector::inspect_surface,
                ReleaseGil(),
                "Return the SurfaceInspectionResult: the result of every "
                "inspection of the mesh" );

        const auto inspect_function_name =
            absl::StrCat( "inspect_surface", dimension, "D" );
        module.def(
            inspect_function_name.c_str(),
            []( const SurfaceMesh& surface ) {
                const SurfaceMeshInspector inspector{ surface };
                return inspector.inspect_surface();
            },
            pybind11::arg( "surface" ), ReleaseGil(),
            "Inspect the SurfaceMesh with a SurfaceMeshInspector and return "
            "the SurfaceInspectionResult" );
    }

    void define_surface_inspector( pybind11::module& module )
    {
        auto result = define_inspection_result< SurfaceInspectionResult >(
            module, "SurfaceInspectionResult",
            "Result of the inspection of a SurfaceMesh" );
        INSPECTION_RESULT_MEMBER( result, SurfaceInspectionResult,
            polygon_edges_with_wrong_adjacency,
            "InspectionIssuesPolygonEdge: the polygon edges with a wrong "
            "adjacency" );
        INSPECTION_RESULT_MEMBER( result, SurfaceInspectionResult,
            colocated_points_groups,
            "InspectionIssuesVectorIndex: the groups of colocated vertices" );
        INSPECTION_RESULT_MEMBER( result, SurfaceInspectionResult,
            degenerated_edges,
            "InspectionIssuesIndex: the indices of the degenerated edges" );
        INSPECTION_RESULT_MEMBER( result, SurfaceInspectionResult,
            degenerated_polygons,
            "InspectionIssuesIndex: the indices of the degenerated "
            "polygons" );
        INSPECTION_RESULT_MEMBER( result, SurfaceInspectionResult,
            non_manifold_edges,
            "InspectionIssuesArrayIndex2: the non manifold edges (pairs of "
            "vertex indices)" );
        INSPECTION_RESULT_MEMBER( result, SurfaceInspectionResult,
            non_manifold_vertices,
            "InspectionIssuesIndex: the indices of the non manifold "
            "vertices" );
        INSPECTION_RESULT_MEMBER( result, SurfaceInspectionResult,
            intersecting_elements,
            "InspectionIssuesPairIndex: the pairs of intersecting polygons" );
        INSPECTION_RESULT_MEMBER( result, SurfaceInspectionResult,
            negative_polygons,
            "InspectionIssuesIndex: the indices of the polygons with a "
            "negative area (always empty for a 3D mesh)" );
        INSPECTION_RESULT_MEMBER( result, SurfaceInspectionResult,
            isolated_vertices,
            "InspectionIssuesIndex: the indices of the vertices that are in "
            "no polygon" );

        do_define_surface_inspector< 2 >( module );
        do_define_surface_inspector< 3 >( module );
    }
} // namespace geode
