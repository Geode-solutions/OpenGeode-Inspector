/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/model/representation/core/section.h>

#include <geode/inspector/topology/section_topology.h>

namespace geode
{
    void define_section_topology_inspector( pybind11::module& module )
    {
        pybind11::class_< SectionCornersInspectionResult >(
            module, "SectionCornersInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "corners_not_meshed",
                &SectionCornersInspectionResult::corners_not_meshed )
            .def_readwrite( "corners_not_linked_to_a_unique_vertex",
                &SectionCornersInspectionResult::
                    corners_not_linked_to_a_unique_vertex )
            .def_readwrite( "unique_vertices_linked_to_multiple_corners",
                &SectionCornersInspectionResult::
                    unique_vertices_linked_to_multiple_corners )
            .def_readwrite(
                "unique_vertices_linked_to_multiple_internals_corner",
                &SectionCornersInspectionResult::
                    unique_vertices_linked_to_multiple_internals_corner )
            .def_readwrite(
                "unique_vertices_linked_not_internal_nor_boundary_corner",
                &SectionCornersInspectionResult::
                    unique_vertices_linked_to_not_internal_nor_boundary_corner )
            .def_readwrite( "unique_vertices_liked_to_not_boundary_line_corner",
                &SectionCornersInspectionResult::
                    unique_vertices_liked_to_not_boundary_line_corner )
            .def( "string", &SectionCornersInspectionResult::string );

        pybind11::class_< SectionLinesInspectionResult >(
            module, "SectionLinesInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "lines_not_meshed",
                &SectionLinesInspectionResult::lines_not_meshed )
            .def_readwrite(
                "unique_vertices_linked_to_not_internal_nor_boundary_line",
                &SectionLinesInspectionResult::
                    unique_vertices_linked_to_not_internal_nor_boundary_line )
            .def_readwrite(
                "unique_vertices_linked_to_a_line_with_invalid_embeddings",
                &SectionLinesInspectionResult::
                    unique_vertices_linked_to_a_line_with_invalid_embeddings )
            .def_readwrite(
                "unique_vertices_linked_to_a_single_and_invalid_line",
                &SectionLinesInspectionResult::
                    unique_vertices_linked_to_a_single_and_invalid_line )
            .def_readwrite( "unique_vertices_linked_to_several_lines_but_not_"
                            "linked_to_a_corner",
                &SectionLinesInspectionResult::
                    unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner )
            .def( "string", &SectionLinesInspectionResult::string );

        pybind11::class_< SectionSurfacesInspectionResult >(
            module, "SectionSurfacesInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "surfaces_not_meshed",
                &SectionSurfacesInspectionResult::surfaces_not_meshed )
            .def_readwrite( "surfaces_not_linked_to_a_unique_vertex",
                &SectionSurfacesInspectionResult::
                    surfaces_not_linked_to_a_unique_vertex )
            .def_readwrite(
                "unique_vertices_linked_to_a_surface_with_invalid_embbedings",
                &SectionSurfacesInspectionResult::
                    unique_vertices_linked_to_a_surface_with_invalid_embbedings )
            .def_readwrite( "unique_vertices_linked_to_a_line_but_is_not_on_a_"
                            "surface_border",
                &SectionSurfacesInspectionResult::
                    unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border )
            .def( "string", &SectionSurfacesInspectionResult::string );

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
            .def( "string", &SectionTopologyInspectionResult::string );

        pybind11::class_< SectionTopologyInspector >(
            module, "SectionTopologyInspector" )
            .def( pybind11::init< const Section& >() )
            .def( "section_topology_is_valid",
                &SectionTopologyInspector::section_topology_is_valid )
            .def( "section_meshed_components_are_linked_to_unique_vertices",
                &SectionTopologyInspector::
                    section_meshed_components_are_linked_to_unique_vertices )
            .def( "section_unique_vertices_are_linked_to_a_component_vertex",
                &SectionTopologyInspector::
                    section_unique_vertices_are_linked_to_a_component_vertex )
            .def( "inspect_section_topology",
                &SectionTopologyInspector::inspect_section_topology );
    }
} // namespace geode
