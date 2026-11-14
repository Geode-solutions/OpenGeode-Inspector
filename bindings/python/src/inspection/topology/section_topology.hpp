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

#include <geode/inspector/inspection/topology/section_topology.hpp>

#include "../../inspection_helpers.hpp"

namespace geode
{
    void define_section_topology_inspector( pybind11::module& module )
    {
        auto corners_result =
            define_inspection_result< SectionCornersTopologyInspectionResult >(
                module, "SectionCornersTopologyInspectionResult",
                "Result of the inspection of the topology of the Corners of a "
                "Section, through the unique vertices" );
        INSPECTION_RESULT_MEMBER( corners_result,
            SectionCornersTopologyInspectionResult, corners_not_meshed,
            "InspectionIssuesUUID: the uuids of the Corners whose mesh has no "
            "vertex" );
        INSPECTION_RESULT_MEMBER( corners_result,
            SectionCornersTopologyInspectionResult,
            corners_not_linked_to_a_unique_vertex,
            "InspectionIssuesMapIndex: the vertices of the Corner meshes that "
            "are not linked to a unique vertex, by Corner" );
        INSPECTION_RESULT_MEMBER( corners_result,
            SectionCornersTopologyInspectionResult,
            unique_vertices_linked_to_multiple_corners,
            "InspectionIssuesIndex: the unique vertices linked to several "
            "Corners" );
        INSPECTION_RESULT_MEMBER( corners_result,
            SectionCornersTopologyInspectionResult,
            unique_vertices_linked_to_multiple_internals_corner,
            "InspectionIssuesIndex: the unique vertices linked to a Corner "
            "with several embeddings" );
        INSPECTION_RESULT_MEMBER( corners_result,
            SectionCornersTopologyInspectionResult,
            unique_vertices_linked_to_not_internal_nor_boundary_corner,
            "InspectionIssuesIndex: the unique vertices linked to a Corner "
            "that is neither internal nor boundary" );
        INSPECTION_RESULT_MEMBER( corners_result,
            SectionCornersTopologyInspectionResult,
            unique_vertices_linked_to_not_boundary_line_corner,
            "InspectionIssuesIndex: the unique vertices linked to the Corner "
            "of a Line without being a boundary of this Line" );

        auto lines_result =
            define_inspection_result< SectionLinesTopologyInspectionResult >(
                module, "SectionLinesTopologyInspectionResult",
                "Result of the inspection of the topology of the Lines of a "
                "Section, through the unique vertices" );
        INSPECTION_RESULT_MEMBER( lines_result,
            SectionLinesTopologyInspectionResult, lines_not_meshed,
            "InspectionIssuesUUID: the uuids of the Lines whose mesh has no "
            "vertex" );
        INSPECTION_RESULT_MEMBER( lines_result,
            SectionLinesTopologyInspectionResult,
            lines_not_linked_to_a_unique_vertex,
            "InspectionIssuesMapIndex: the vertices of the Line meshes that "
            "are not linked to a unique vertex, by Line" );
        INSPECTION_RESULT_MEMBER( lines_result,
            SectionLinesTopologyInspectionResult,
            unique_vertices_linked_to_line_with_wrong_relationship_to_surface,
            "InspectionIssuesIndex: the unique vertices linked to a Line "
            "incorrectly linked to a Surface" );
        INSPECTION_RESULT_MEMBER( lines_result,
            SectionLinesTopologyInspectionResult,
            unique_vertices_linked_to_a_line_with_invalid_embeddings,
            "InspectionIssuesIndex: the unique vertices linked to a Line with "
            "invalid internal topology" );
        INSPECTION_RESULT_MEMBER( lines_result,
            SectionLinesTopologyInspectionResult,
            unique_vertices_linked_to_a_single_and_invalid_line,
            "InspectionIssuesIndex: the unique vertices linked to a single "
            "Line which is invalid" );
        INSPECTION_RESULT_MEMBER( lines_result,
            SectionLinesTopologyInspectionResult,
            unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner,
            "InspectionIssuesIndex: the unique vertices linked to several "
            "Lines but not to a Corner" );

        auto surfaces_result =
            define_inspection_result< SectionSurfacesTopologyInspectionResult >(
                module, "SectionSurfacesTopologyInspectionResult",
                "Result of the inspection of the topology of the Surfaces of a "
                "Section, through the unique vertices" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            SectionSurfacesTopologyInspectionResult, surfaces_not_meshed,
            "InspectionIssuesUUID: the uuids of the Surfaces whose mesh has "
            "no vertex" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            SectionSurfacesTopologyInspectionResult,
            surfaces_not_linked_to_a_unique_vertex,
            "InspectionIssuesMapIndex: the vertices of the Surface meshes "
            "that are not linked to a unique vertex, by Surface" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            SectionSurfacesTopologyInspectionResult,
            unique_vertices_linked_to_a_surface_with_invalid_embbedings,
            "InspectionIssuesIndex: the unique vertices linked to a Surface "
            "with invalid internal topology" );
        INSPECTION_RESULT_MEMBER( surfaces_result,
            SectionSurfacesTopologyInspectionResult,
            unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border,
            "InspectionIssuesIndex: the unique vertices linked to a Line but "
            "not on the border of a Surface" );

        auto topology_result =
            define_inspection_result< SectionTopologyInspectionResult >( module,
                "SectionTopologyInspectionResult",
                "Result of the inspection of the topology of a Section: one "
                "result per type of component, and the issues of the links "
                "between the unique vertices and the component mesh vertices" );
        INSPECTION_RESULT_MEMBER( topology_result,
            SectionTopologyInspectionResult, corners,
            "SectionCornersTopologyInspectionResult: the topology of the "
            "Corners" );
        INSPECTION_RESULT_MEMBER( topology_result,
            SectionTopologyInspectionResult, lines,
            "SectionLinesTopologyInspectionResult: the topology of the Lines" );
        INSPECTION_RESULT_MEMBER( topology_result,
            SectionTopologyInspectionResult, surfaces,
            "SectionSurfacesTopologyInspectionResult: the topology of the "
            "Surfaces" );
        INSPECTION_RESULT_MEMBER( topology_result,
            SectionTopologyInspectionResult,
            unique_vertices_not_linked_to_any_component,
            "InspectionIssuesIndex: the unique vertices not linked to any "
            "component mesh vertex" );
        INSPECTION_RESULT_MEMBER( topology_result,
            SectionTopologyInspectionResult,
            unique_vertices_linked_to_inexistant_cmv,
            "InspectionIssuesIndex: the unique vertices linked to a component "
            "mesh vertex that does not exist" );
        INSPECTION_RESULT_MEMBER( topology_result,
            SectionTopologyInspectionResult,
            unique_vertices_nonbijectively_linked_to_cmv,
            "InspectionIssuesIndex: the unique vertices linked to a component "
            "mesh vertex that is linked to another unique vertex" );

        pybind11::class_< SectionCornersTopology >( module,
            "SectionCornersTopology",
            "Inspection of the topology of the Corners of a Section through "
            "the unique vertices: each unique vertex is linked to at most one "
            "Corner, a Corner is internal to at most one component and is a "
            "boundary of every Line it is linked to" )
            .def( pybind11::init< const Section& >(),
                pybind11::arg( "section" ), pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the topology of the Corners of this "
                "Section. The inspector keeps the Section alive" )
            .def( "inspect_corners_topology",
                &SectionCornersTopology::inspect_corners_topology, ReleaseGil(),
                "Return the SectionCornersTopologyInspectionResult: the "
                "Corners "
                "that are not meshed or not linked to unique vertices, and the "
                "unique vertices with an invalid link to a Corner" );

        pybind11::class_< SectionLinesTopology >( module,
            "SectionLinesTopology",
            "Inspection of the topology of the Lines of a Section through the "
            "unique vertices: each Line is internal or boundary, the ends of "
            "a Line and the vertices shared by several Lines are Corners" )
            .def( pybind11::init< const Section& >(),
                pybind11::arg( "section" ), pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the topology of the Lines of this "
                "Section. The inspector keeps the Section alive" )
            .def( "inspect_lines_topology",
                &SectionLinesTopology::inspect_lines_topology, ReleaseGil(),
                "Return the SectionLinesTopologyInspectionResult: the Lines "
                "that "
                "are not meshed or not linked to unique vertices, and the "
                "unique "
                "vertices with an invalid link to a Line" );

        pybind11::class_< SectionSurfacesTopology >( module,
            "SectionSurfacesTopology",
            "Inspection of the topology of the Surfaces of a Section through "
            "the unique vertices: a unique vertex shared by two Surfaces is "
            "part of a Line which is a boundary of both" )
            .def( pybind11::init< const Section& >(),
                pybind11::arg( "section" ), pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the topology of the Surfaces of this "
                "Section. The inspector keeps the Section alive" )
            .def( "inspect_surfaces",
                &SectionSurfacesTopology::inspect_surfaces, ReleaseGil(),
                "Return the SectionSurfacesTopologyInspectionResult: the "
                "Surfaces "
                "that are not meshed or not linked to unique vertices, and the "
                "unique vertices with an invalid link to a Surface" );

        pybind11::class_< SectionTopologyInspector, SectionCornersTopology,
            SectionLinesTopology, SectionSurfacesTopology >( module,
            "SectionTopologyInspector",
            "Inspection of the topology of a Section through its unique "
            "vertices. It also has the methods of the inspectors of the "
            "Corners, Lines and Surfaces" )
            .def( pybind11::init< const Section& >(),
                pybind11::arg( "section" ), pybind11::keep_alive< 1, 2 >(),
                "Create the topology inspector of this Section. The inspector "
                "keeps the Section alive" )
            .def( "section_topology_is_valid",
                &SectionTopologyInspector::section_topology_is_valid,
                ReleaseGil(),
                "Return True if the Section is topologically valid through its "
                "unique vertices: it has unique vertices, the meshed "
                "components "
                "are linked to unique vertices, the unique vertices are "
                "bijectively linked to existing component mesh vertices, and "
                "the "
                "unique vertices are valid Corners, Lines and Surfaces "
                "vertices" )
            .def( "section_unique_vertices_are_bijectively_linked_to_an_"
                  "existing_component_vertex",
                &SectionTopologyInspector::
                    section_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex,
                ReleaseGil(),
                "Return True if every unique vertex is linked to at least one "
                "component mesh vertex, these vertices exist, and each one in "
                "an "
                "active component is linked back to the same unique vertex" )
            .def( "inspect_section_topology",
                &SectionTopologyInspector::inspect_section_topology,
                ReleaseGil(),
                "Return the SectionTopologyInspectionResult. When a unique "
                "vertex "
                "is linked to a vertex beyond the mesh of its component, a "
                "warning is logged and only the issues of the links between "
                "unique vertices and component mesh vertices are listed (the "
                "inspections of the components are skipped)" );
    }
} // namespace geode
