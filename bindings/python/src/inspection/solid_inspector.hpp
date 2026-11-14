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

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/inspector/inspection/solid_inspector.hpp>

#include "../inspection_helpers.hpp"

namespace geode
{
    template < index_t dimension >
    void do_define_solid_inspector( pybind11::module& module )
    {
        using SolidMesh = SolidMesh< dimension >;
        using SolidMeshInspector = SolidMeshInspector< dimension >;
        const auto name = absl::StrCat( "SolidMeshInspector", dimension, "D" );
        pybind11::class_< SolidMeshInspector, SolidMeshAdjacency< dimension >,
            SolidMeshColocation< dimension >,
            SolidMeshDegeneration< dimension >,
            SolidMeshVertexManifold< dimension >,
            SolidMeshEdgeManifold< dimension >,
            SolidMeshFacetManifold< dimension >,
            SolidMeshNegativeElements< dimension >,
            SolidMeshIsolation< dimension > >( module, name.c_str(),
            "Complete inspection of a SolidMesh: adjacency, colocation, "
            "degeneration, manifold property, negative polyhedra and "
            "isolation (SolidMeshAdjacency, SolidMeshColocation, ... whose "
            "methods it also has). The edges of the mesh are enabled by the "
            "inspections returning edge indices, and disabled again when "
            "the inspector is destroyed (if they were not enabled)" )
            .def( pybind11::init< const SolidMesh& >(), pybind11::arg( "mesh" ),
                "Create the inspector of this mesh. The inspector keeps the "
                "mesh alive" )
            .def( "inspect_solid", &SolidMeshInspector::inspect_solid,
                ReleaseGil(),
                "Return the SolidInspectionResult: the result of every "
                "inspection of the mesh" );

        const auto inspect_function_name =
            absl::StrCat( "inspect_solid", dimension, "D" );
        module.def(
            inspect_function_name.c_str(),
            []( const SolidMesh& solid ) {
                const SolidMeshInspector inspector{ solid };
                return inspector.inspect_solid();
            },
            pybind11::arg( "solid" ), ReleaseGil(),
            "Inspect the SolidMesh with a SolidMeshInspector and return the "
            "SolidInspectionResult" );
    }

    void define_solid_inspector( pybind11::module& module )
    {
        auto result = define_inspection_result< SolidInspectionResult >( module,
            "SolidInspectionResult",
            "Result of the inspection of a SolidMesh" );
        INSPECTION_RESULT_MEMBER( result, SolidInspectionResult,
            polyhedron_facets_with_wrong_adjacency,
            "InspectionIssuesPolyhedronFacet: the polyhedron facets with a "
            "wrong adjacency" );
        INSPECTION_RESULT_MEMBER( result, SolidInspectionResult,
            colocated_points_groups,
            "InspectionIssuesVectorIndex: the groups of colocated vertices" );
        INSPECTION_RESULT_MEMBER( result, SolidInspectionResult,
            degenerated_edges,
            "InspectionIssuesIndex: the indices of the degenerated edges" );
        INSPECTION_RESULT_MEMBER( result, SolidInspectionResult,
            degenerated_polyhedra,
            "InspectionIssuesIndex: the indices of the degenerated "
            "polyhedra" );
        INSPECTION_RESULT_MEMBER( result, SolidInspectionResult,
            non_manifold_vertices,
            "InspectionIssuesIndex: the indices of the non manifold "
            "vertices" );
        INSPECTION_RESULT_MEMBER( result, SolidInspectionResult,
            non_manifold_edges,
            "InspectionIssuesArrayIndex2: the non manifold edges (pairs of "
            "vertex indices)" );
        INSPECTION_RESULT_MEMBER( result, SolidInspectionResult,
            non_manifold_facets,
            "InspectionIssuesPolyhedronFacetVertices: the non manifold "
            "facets (lists of vertex indices)" );
        INSPECTION_RESULT_MEMBER( result, SolidInspectionResult,
            negative_polyhedra,
            "InspectionIssuesIndex: the indices of the polyhedra with a "
            "negative volume" );
        INSPECTION_RESULT_MEMBER( result, SolidInspectionResult,
            isolated_vertices,
            "InspectionIssuesIndex: the indices of the vertices that are in "
            "no polyhedron" );

        do_define_solid_inspector< 3 >( module );
    }
} // namespace geode
