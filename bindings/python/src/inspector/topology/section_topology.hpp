/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/inspector/topology/section_topology.hpp>

namespace geode
{
    void define_section_topology_inspector( pybind11::module& module )
    {
        pybind11::class_< SectionCornersTopologyInspectionResult >(
            module, "SectionCornersTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "corners_not_meshed",
                &SectionCornersTopologyInspectionResult::corners_not_meshed )
            .def_readwrite( "corners_not_linked_to_a_unique_vertex",
                &SectionCornersTopologyInspectionResult::
                    corners_not_linked_to_a_unique_vertex )
            .def_readwrite( "unique_vertices_linked_to_multiple_corners",
                &SectionCornersTopologyInspectionResult::
                    unique_vertices_linked_to_multiple_corners )
            .def_readwrite(
                "unique_vertices_linked_to_multiple_internals_corner",
                &SectionCornersTopologyInspectionResult::
                    unique_vertices_linked_to_multiple_internals_corner )
            .def_readwrite(
                "unique_vertices_linked_to_not_internal_nor_boundary_corner",
                &SectionCornersTopologyInspectionResult::
                    unique_vertices_linked_to_not_internal_nor_boundary_corner )
            .def_readwrite(
                "unique_vertices_linked_to_not_boundary_line_corner",
                &SectionCornersTopologyInspectionResult::
                    unique_vertices_linked_to_not_boundary_line_corner )
            .def( "string", &SectionCornersTopologyInspectionResult::string )
            .def( "inspection_type",
                &SectionCornersTopologyInspectionResult::inspection_type );

        pybind11::class_< SectionLinesTopologyInspectionResult >(
            module, "SectionLinesTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "lines_not_meshed",
                &SectionLinesTopologyInspectionResult::lines_not_meshed )
            .def_readwrite( "lines_not_linked_to_a_unique_vertex",
                &SectionLinesTopologyInspectionResult::
                    lines_not_linked_to_a_unique_vertex )
            .def_readwrite( "unique_vertices_linked_to_line_with_wrong_"
                            "relationship_to_surface",
                &SectionLinesTopologyInspectionResult::
                    unique_vertices_linked_to_line_with_wrong_relationship_to_surface )
            .def_readwrite(
                "unique_vertices_linked_to_a_line_with_invalid_embeddings",
                &SectionLinesTopologyInspectionResult::
                    unique_vertices_linked_to_a_line_with_invalid_embeddings )
            .def_readwrite(
                "unique_vertices_linked_to_a_single_and_invalid_line",
                &SectionLinesTopologyInspectionResult::
                    unique_vertices_linked_to_a_single_and_invalid_line )
            .def_readwrite( "unique_vertices_linked_to_several_lines_but_not_"
                            "linked_to_a_corner",
                &SectionLinesTopologyInspectionResult::
                    unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner )
            .def( "string", &SectionLinesTopologyInspectionResult::string )
            .def( "inspection_type",
                &SectionLinesTopologyInspectionResult::inspection_type );

        pybind11::class_< SectionSurfacesTopologyInspectionResult >(
            module, "SectionSurfacesTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "surfaces_not_meshed",
                &SectionSurfacesTopologyInspectionResult::surfaces_not_meshed )
            .def_readwrite( "surfaces_not_linked_to_a_unique_vertex",
                &SectionSurfacesTopologyInspectionResult::
                    surfaces_not_linked_to_a_unique_vertex )
            .def_readwrite(
                "unique_vertices_linked_to_a_surface_with_invalid_embbedings",
                &SectionSurfacesTopologyInspectionResult::
                    unique_vertices_linked_to_a_surface_with_invalid_embbedings )
            .def_readwrite( "unique_vertices_linked_to_a_line_but_is_not_on_a_"
                            "surface_border",
                &SectionSurfacesTopologyInspectionResult::
                    unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border )
            .def( "string", &SectionSurfacesTopologyInspectionResult::string )
            .def( "inspection_type",
                &SectionSurfacesTopologyInspectionResult::inspection_type );

        pybind11::class_< SectionTopologyInspectionResult >(
            module, "SectionTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite(
                "corners", &SectionTopologyInspectionResult::corners )
            .def_readwrite( "lines", &SectionTopologyInspectionResult::lines )
            .def_readwrite(
                "surfaces", &SectionTopologyInspectionResult::surfaces )
            .def_readwrite( "unique_vertices_not_linked_to_any_component",
                &SectionTopologyInspectionResult::
                    unique_vertices_not_linked_to_any_component )
            .def( "string", &SectionTopologyInspectionResult::string )
            .def( "inspection_type",
                &SectionTopologyInspectionResult::inspection_type );

        pybind11::class_< SectionTopologyInspector >(
            module, "SectionTopologyInspector" )
            .def( pybind11::init< const Section& >() )
            .def( "section_topology_is_valid",
                &SectionTopologyInspector::section_topology_is_valid )
            .def( "section_unique_vertices_are_bijectively_linked_to_an_"
                  "existing_component_vertex",
                &SectionTopologyInspector::
                    section_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex )
            .def( "inspect_section_topology",
                &SectionTopologyInspector::inspect_section_topology );
    }
} // namespace geode
