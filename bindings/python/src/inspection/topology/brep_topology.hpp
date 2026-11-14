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

#include <geode/inspector/inspection/topology/brep_topology.hpp>

#include "../../inspection_helpers.hpp"

namespace geode
{
    void define_brep_topology_inspector( pybind11::module& module )
    {
        auto corners_result =
            define_inspection_result< BRepCornersTopologyInspectionResult >(
                module, "BRepCornersTopologyInspectionResult",
                "Result of the inspection of the topology of the Corners of a "
                "BRep, through the unique vertices" );
        INSPECTION_RESULT_MEMBER( corners_result,
            BRepCornersTopologyInspectionResult, corners_not_meshed,
            "InspectionIssuesUUID: the uuids of the Corners whose mesh has no "
            "vertex" );
        INSPECTION_RESULT_MEMBER( corners_result,
            BRepCornersTopologyInspectionResult,
            corners_not_linked_to_a_unique_vertex,
            "InspectionIssuesMapIndex: the vertices of the Corner meshes that "
            "are not linked to a unique vertex, by Corner" );
        INSPECTION_RESULT_MEMBER( corners_result,
            BRepCornersTopologyInspectionResult,
            unique_vertices_linked_to_multiple_corners,
            "InspectionIssuesIndex: the unique vertices linked to several "
            "Corners" );
        INSPECTION_RESULT_MEMBER( corners_result,
            BRepCornersTopologyInspectionResult,
            unique_vertices_linked_to_multiply_embedded_corner,
            "InspectionIssuesIndex: the unique vertices linked to a Corner "
            "embedded in several components including a Block" );
        INSPECTION_RESULT_MEMBER( corners_result,
            BRepCornersTopologyInspectionResult,
            unique_vertices_linked_to_not_internal_nor_boundary_corner,
            "InspectionIssuesIndex: the unique vertices linked to a Corner "
            "that is neither internal nor boundary" );
        INSPECTION_RESULT_MEMBER( corners_result,
            BRepCornersTopologyInspectionResult,
            unique_vertices_linked_to_not_boundary_line_corner,
            "InspectionIssuesIndex: the unique vertices linked to the Corner "
            "of a Line without being a boundary of this Line" );

        auto lines_result = define_inspection_result<
            BRepLinesTopologyInspectionResult >( module,
            "BRepLinesTopologyInspectionResult",
            "Result of the inspection of the topology of the Lines of a BRep, "
            "through the unique vertices" );
        INSPECTION_RESULT_MEMBER( lines_result,
            BRepLinesTopologyInspectionResult, lines_not_meshed,
            "InspectionIssuesUUID: the uuids of the Lines whose mesh has no "
            "vertex" );
        INSPECTION_RESULT_MEMBER( lines_result,
            BRepLinesTopologyInspectionResult, lines_not_embedded_nor_incident,
            "InspectionIssuesUUID: the uuids of the Lines that are neither "
            "embedded nor incident to another component" );
        INSPECTION_RESULT_MEMBER( lines_result,
            BRepLinesTopologyInspectionResult, lines_multiply_embedded,
            "InspectionIssuesUUID: the uuids of the Lines embedded in several "
            "components including a Block" );
        INSPECTION_RESULT_MEMBER( lines_result,
            BRepLinesTopologyInspectionResult,
            unique_vertices_linked_to_line_with_wrong_relationship_to_surface,
            "InspectionIssuesIndex: the unique vertices linked to a Line with "
            "an incorrect relationship to a Surface" );
        INSPECTION_RESULT_MEMBER( lines_result,
            BRepLinesTopologyInspectionResult,
            lines_not_linked_to_a_unique_vertex,
            "InspectionIssuesMapIndex: the vertices of the Line meshes that "
            "are not linked to a unique vertex, by Line" );
        INSPECTION_RESULT_MEMBER( lines_result,
            BRepLinesTopologyInspectionResult,
            unique_vertices_linked_to_a_line_with_invalid_embeddings,
            "InspectionIssuesIndex: the unique vertices linked to a Line with "
            "invalid internal topology" );
        INSPECTION_RESULT_MEMBER( lines_result,
            BRepLinesTopologyInspectionResult,
            unique_vertices_linked_to_a_single_and_invalid_line,
            "InspectionIssuesIndex: the unique vertices linked to a single "
            "Line which is invalid" );
        INSPECTION_RESULT_MEMBER( lines_result,
            BRepLinesTopologyInspectionResult,
            unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner,
            "InspectionIssuesIndex: the unique vertices linked to several "
            "Lines but not to a Corner" );
        INSPECTION_RESULT_MEMBER( lines_result,
            BRepLinesTopologyInspectionResult,
            line_edges_with_wrong_component_edges_around,
            "InspectionIssuesMapIndex: the edges of the Line meshes with "
            "incorrect surrounding component edges, by Line" );

        auto surfaces_result =
            define_inspection_result< BRepSurfacesTopologyInspectionResult >(
                module, "BRepSurfacesTopologyInspectionResult",
                "Result of the inspection of the topology of the Surfaces of a "
                "BRep, through the unique vertices" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            BRepSurfacesTopologyInspectionResult, surfaces_not_meshed,
            "InspectionIssuesUUID: the uuids of the Surfaces whose mesh has "
            "no vertex" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            BRepSurfacesTopologyInspectionResult, surfaces_multiply_embedded,
            "InspectionIssuesUUID: the uuids of the Surfaces embedded in "
            "several Blocks" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            BRepSurfacesTopologyInspectionResult,
            surfaces_not_linked_to_a_unique_vertex,
            "InspectionIssuesMapIndex: the vertices of the Surface meshes "
            "that are not linked to a unique vertex, by Surface" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            BRepSurfacesTopologyInspectionResult,
            unique_vertices_linked_to_a_surface_with_invalid_embbedings,
            "InspectionIssuesIndex: the unique vertices linked to a Surface "
            "with invalid internal topology" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            BRepSurfacesTopologyInspectionResult,
            unique_vertices_linked_to_several_and_invalid_surfaces,
            "InspectionIssuesIndex: the unique vertices improperly linked to "
            "several Surfaces" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            BRepSurfacesTopologyInspectionResult,
            unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border,
            "InspectionIssuesIndex: the unique vertices linked to a Line but "
            "not on the border of a Surface" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            BRepSurfacesTopologyInspectionResult,
            surface_polygons_with_wrong_component_facets_around,
            "InspectionIssuesMapIndex: the polygons of the Surface meshes "
            "with invalid surrounding component facets, by Surface" );

        auto blocks_result =
            define_inspection_result< BRepBlocksTopologyInspectionResult >(
                module, "BRepBlocksTopologyInspectionResult",
                "Result of the inspection of the topology of the Blocks of a "
                "BRep, through the unique vertices" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult, some_blocks_not_meshed,
            "InspectionIssuesUUID: the uuids of the Blocks whose mesh has no "
            "vertex" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult, wrong_block_boundary_surface,
            "InspectionIssuesUUID: the uuids of the Blocks with a boundary "
            "Surface not linked to another boundary Surface by a Line (the "
            "Surface should not be a boundary, or there is a hole in the "
            "boundaries of the Block)" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult,
            blocks_not_linked_to_a_unique_vertex,
            "InspectionIssuesMapIndex: the vertices of the Block meshes that "
            "are not linked to a unique vertex, by Block" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult,
            blocks_with_not_closed_boundary_surfaces,
            "InspectionIssuesUUID: the uuids of the Blocks with non closed "
            "boundary Surfaces" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult,
            blocks_split_by_internal_surfaces,
            "InspectionIssuesUUID: the uuids of the Blocks split into several "
            "volumes by internal Surfaces" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult,
            model_boundaries_dont_form_a_closed_surface,
            "InspectionIssuesIndex: the unique vertices where the "
            "ModelBoundaries do not form a closed surface" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult,
            unique_vertices_part_of_two_blocks_and_no_boundary_surface,
            "InspectionIssuesIndex: the unique vertices shared by two Blocks "
            "without a boundary Surface or an incident Line of a Block" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult,
            unique_vertices_with_incorrect_block_cmvs_count,
            "InspectionIssuesIndex: the unique vertices in a Block with an "
            "incorrect number of component mesh vertices" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult,
            unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks,
            "InspectionIssuesIndex: the unique vertices linked to Surfaces "
            "with neither boundary nor internal status" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult,
            unique_vertices_linked_to_a_single_and_invalid_surface,
            "InspectionIssuesIndex: the unique vertices linked to a single "
            "Surface which is invalid" );
        INSPECTION_RESULT_MEMBER( blocks_result,
            BRepBlocksTopologyInspectionResult,
            unique_vertex_linked_to_multiple_invalid_surfaces,
            "InspectionIssuesIndex: the unique vertices linked to several "
            "Surfaces in an invalid way" );

        auto topology_result = define_inspection_result<
            BRepTopologyInspectionResult >( module,
            "BRepTopologyInspectionResult",
            "Result of the inspection of the topology of a BRep: one result "
            "per type of component, and the issues of the links between the "
            "unique vertices and the component mesh vertices" );
        INSPECTION_RESULT_MEMBER( topology_result, BRepTopologyInspectionResult,
            corners,
            "BRepCornersTopologyInspectionResult: the topology of the "
            "Corners" );
        INSPECTION_RESULT_MEMBER( topology_result, BRepTopologyInspectionResult,
            lines,
            "BRepLinesTopologyInspectionResult: the topology of the Lines" );
        INSPECTION_RESULT_MEMBER( topology_result, BRepTopologyInspectionResult,
            surfaces,
            "BRepSurfacesTopologyInspectionResult: the topology of the "
            "Surfaces" );
        INSPECTION_RESULT_MEMBER( topology_result, BRepTopologyInspectionResult,
            blocks,
            "BRepBlocksTopologyInspectionResult: the topology of the Blocks" );
        INSPECTION_RESULT_MEMBER( topology_result, BRepTopologyInspectionResult,
            unique_vertices_not_linked_to_any_component,
            "InspectionIssuesIndex: the unique vertices not linked to any "
            "component mesh vertex" );
        INSPECTION_RESULT_MEMBER( topology_result, BRepTopologyInspectionResult,
            unique_vertices_linked_to_inexistant_cmv,
            "InspectionIssuesIndex: the unique vertices linked to a component "
            "mesh vertex that does not exist" );
        INSPECTION_RESULT_MEMBER( topology_result, BRepTopologyInspectionResult,
            unique_vertices_nonbijectively_linked_to_cmv,
            "InspectionIssuesIndex: the unique vertices linked to a component "
            "mesh vertex that is linked to another unique vertex" );

        pybind11::class_< BRepCornersTopology >( module, "BRepCornersTopology",
            "Inspection of the topology of the Corners of a BRep through the "
            "unique vertices: each unique vertex is linked to at most one "
            "Corner, a Corner is internal to at most one component and is a "
            "boundary of every Line it is linked to" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                "Create the inspector of the topology of the Corners of this "
                "BRep. The inspector keeps the BRep alive" )
            .def( "inspect_corners_topology",
                &BRepCornersTopology::inspect_corners_topology, ReleaseGil(),
                "Return the BRepCornersTopologyInspectionResult: the Corners "
                "that "
                "are not meshed or not linked to unique vertices, and the "
                "unique "
                "vertices with an invalid link to a Corner" );

        pybind11::class_< BRepLinesTopology >( module, "BRepLinesTopology",
            "Inspection of the topology of the Lines of a BRep through the "
            "unique vertices: each Line is embedded or incident to other "
            "components, the ends of a Line and the vertices shared by "
            "several Lines are Corners" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                "Create the inspector of the topology of the Lines of this "
                "BRep. "
                "The inspector keeps the BRep alive" )
            .def( "inspect_lines_topology",
                &BRepLinesTopology::inspect_lines_topology, ReleaseGil(),
                "Return the BRepLinesTopologyInspectionResult: the Lines that "
                "are "
                "not meshed, not linked to unique vertices or with an invalid "
                "status, and the unique vertices with an invalid link to a "
                "Line" );

        pybind11::class_< BRepSurfacesTopology >( module,
            "BRepSurfacesTopology",
            "Inspection of the topology of the Surfaces of a BRep through the "
            "unique vertices: each Surface is embedded or boundary, and the "
            "unique vertices shared by several Surfaces are consistent with "
            "the Lines and Blocks around" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                "Create the inspector of the topology of the Surfaces of this "
                "BRep. The inspector keeps the BRep alive" )
            .def( "inspect_surfaces_topology",
                &BRepSurfacesTopology::inspect_surfaces_topology, ReleaseGil(),
                "Return the BRepSurfacesTopologyInspectionResult: the Surfaces "
                "that are not meshed, not linked to unique vertices or with an "
                "invalid status, and the unique vertices with an invalid link "
                "to "
                "a Surface" );

        pybind11::class_< BRepBlocksTopology >( module, "BRepBlocksTopology",
            "Inspection of the topology of the Blocks of a BRep through the "
            "unique vertices: the boundary Surfaces of each Block are closed, "
            "and the unique vertices shared by Blocks are consistent with the "
            "Surfaces and Lines around" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                "Create the inspector of the topology of the Blocks of this "
                "BRep. "
                "The inspector keeps the BRep alive" )
            .def( "inspect_blocks", &BRepBlocksTopology::inspect_blocks,
                ReleaseGil(),
                "Return the BRepBlocksTopologyInspectionResult: the Blocks "
                "that "
                "are not meshed, not linked to unique vertices or with invalid "
                "boundaries, and the unique vertices with an invalid link to a "
                "Block" );

        pybind11::class_< BRepTopologyInspector, BRepCornersTopology,
            BRepLinesTopology, BRepSurfacesTopology, BRepBlocksTopology >(
            module, "BRepTopologyInspector",
            "Inspection of the topology of a BRep through its unique "
            "vertices. It also has the methods of the inspectors of the "
            "Corners, Lines, Surfaces and Blocks" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                "Create the topology inspector of this BRep. The inspector "
                "keeps "
                "the BRep alive" )
            .def( "brep_topology_is_valid",
                &BRepTopologyInspector::brep_topology_is_valid, ReleaseGil(),
                "Return True if the BRep is topologically valid through its "
                "unique vertices: it has unique vertices, the meshed "
                "components "
                "are linked to unique vertices, the unique vertices are "
                "bijectively linked to existing component mesh vertices, and "
                "the "
                "unique vertices are valid Corners, Lines, Surfaces and Blocks "
                "vertices" )
            .def( "brep_unique_vertices_are_bijectively_linked_to_an_existing_"
                  "component_vertex",
                &BRepTopologyInspector::
                    brep_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex,
                ReleaseGil(),
                "Return True if every unique vertex is linked to at least one "
                "component mesh vertex, these vertices exist, and each one in "
                "an "
                "active component is linked back to the same unique vertex" )
            .def( "inspect_brep_topology",
                &BRepTopologyInspector::inspect_brep_topology, ReleaseGil(),
                "Return the BRepTopologyInspectionResult. When a unique vertex "
                "is "
                "linked to a vertex beyond the mesh of its component, a "
                "warning "
                "is logged and only the issues of the links between unique "
                "vertices and component mesh vertices are listed (the "
                "inspections "
                "of the components are skipped)" );
    }
} // namespace geode
