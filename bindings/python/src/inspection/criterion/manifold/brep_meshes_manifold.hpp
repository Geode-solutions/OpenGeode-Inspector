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

#include <geode/inspector/inspection/criterion/manifold/brep_meshes_manifold.hpp>

#include "../../../inspection_helpers.hpp"
#include "../../information.hpp"

namespace geode
{
    void define_brep_meshes_manifold( pybind11::module& module )
    {
        auto edge = pybind11::class_< BRepNonManifoldEdge >( module,
            "BRepNonManifoldEdge",
            "A non manifold edge of a BRep: an edge between two unique "
            "vertices which is on several Surfaces without being on their "
            "border, or which is the only edge of a Line embedded in "
            "Surfaces, whose unique vertices are linked to no other mesh "
            "vertex" );
        edge.def( pybind11::init<>(),
                "Create an edge between the unique vertices 0 and 0, without "
                "component" )
            .def(
                pybind11::init( []( const std::array< index_t, 2 >& vertices,
                                    const std::vector< uuid >& component_ids ) {
                    return BRepNonManifoldEdge{ vertices, component_ids };
                } ),
                pybind11::arg( "edge" ), pybind11::arg( "component_ids" ),
                "Create the non manifold edge between the two unique vertices "
                "of edge, belonging to the components with these uuids" );
        def_readwrite_named( edge, "edge", &BRepNonManifoldEdge::edge,
            "The indices of the two unique vertices of the edge" );
        def_readwrite_named( edge, "component_ids",
            &BRepNonManifoldEdge::component_ids,
            "The uuids of the Surfaces (or of the Line) the edge is on" );

        auto facet = pybind11::class_< BRepNonManifoldFacet >( module,
            "BRepNonManifoldFacet",
            "A non manifold facet of a BRep: the unique vertices of a "
            "Surface with one polygon, embedded in Blocks, whose unique "
            "vertices are linked to no other mesh vertex" );
        facet
            .def( pybind11::init<>(),
                "Create a facet without unique vertex or component" )
            .def(
                pybind11::init( []( const PolygonVertices& vertices,
                                    const std::vector< uuid >& component_ids ) {
                    return BRepNonManifoldFacet{ vertices, component_ids };
                } ),
                pybind11::arg( "facet" ), pybind11::arg( "component_ids" ),
                "Create the non manifold facet made of these unique "
                "vertices, belonging to the components with these uuids" );
        def_readwrite_named( facet, "facet", &BRepNonManifoldFacet::facet,
            "The indices of the unique vertices of the facet" );
        def_readwrite_named( facet, "component_ids",
            &BRepNonManifoldFacet::component_ids,
            "The uuid of the Surface the facet is the polygon of" );

        do_define_information< BRepNonManifoldEdge >(
            module, "BRepNonManifoldEdge", "a BRepNonManifoldEdge" );
        do_define_information< BRepNonManifoldFacet >(
            module, "BRepNonManifoldFacet", "a BRepNonManifoldFacet" );

        auto result =
            define_inspection_result< BRepMeshesManifoldInspectionResult >(
                module, "BRepMeshesManifoldInspectionResult",
                "Result of the inspection of the manifold property of the "
                "meshes of the Surfaces and Blocks of a BRep, and of the BRep "
                "itself" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesManifoldInspectionResult,
            meshes_non_manifold_vertices,
            "InspectionIssuesMapIndex: the non manifold vertices of the "
            "meshes, by Surface and Block" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesManifoldInspectionResult,
            meshes_non_manifold_edges,
            "InspectionIssuesMapArrayIndex2: the non manifold edges of the "
            "meshes (pairs of vertex indices), by Surface and Block" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesManifoldInspectionResult,
            meshes_non_manifold_facets,
            "InspectionIssuesMapPolyhedronFacetVertices: the non manifold "
            "facets of the Block meshes (lists of vertex indices), by Block" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesManifoldInspectionResult,
            brep_non_manifold_edges,
            "InspectionIssuesBRepNonManifoldEdge: the non manifold edges "
            "of the BRep, between unique vertices" );
        INSPECTION_RESULT_MEMBER( result, BRepMeshesManifoldInspectionResult,
            brep_non_manifold_facets,
            "InspectionIssuesBRepNonManifoldFacet: the non manifold facets "
            "of the BRep, between unique vertices" );

        pybind11::class_< BRepComponentMeshesManifold >( module,
            "BRepComponentMeshesManifold",
            "Inspection of the manifold property of the meshes of the active "
            "Surfaces and Blocks of a BRep, and of the BRep itself" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the manifold property of this BRep. "
                "The inspector keeps the BRep alive" )
            .def( "inspect_brep_manifold",
                &BRepComponentMeshesManifold::inspect_brep_manifold,
                ReleaseGil(),
                "Return the BRepMeshesManifoldInspectionResult: the non "
                "manifold vertices, edges and facets of each Surface and "
                "Block mesh (as the mesh inspectors find them), and the non "
                "manifold edges and facets of the BRep" );
    }
} // namespace geode
