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

#include <geode/model/representation/core/brep.h>

#include <geode/inspector/topology/brep_topology.h>

namespace geode
{

    void define_brep_topology_inspector( pybind11::module& module )
    {
        pybind11::class_< BRepCornersTopologyInspectionResult >(
            module, "BRepCornersTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "multiple_corners_unique_vertices",
                &BRepCornersTopologyInspectionResult::
                    multiple_corners_unique_vertices )
            .def_readwrite( "multiple_internals_corner_vertices",
                &BRepCornersTopologyInspectionResult::
                    multiple_internals_corner_vertices )
            .def_readwrite( "not_internal_nor_boundary_corner_vertices",
                &BRepCornersTopologyInspectionResult::
                    not_internal_nor_boundary_corner_vertices )
            .def_readwrite( "line_corners_without_boundary_status",
                &BRepCornersTopologyInspectionResult::
                    line_corners_without_boundary_status );

        pybind11::class_< BRepLinesTopologyInspectionResult >(
            module, "BRepLinesTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite(
                "part_of_not_boundary_nor_internal_line_unique_vertices",
                &BRepLinesTopologyInspectionResult::
                    part_of_not_boundary_nor_internal_line_unique_vertices )
            .def_readwrite(
                "part_of_line_with_invalid_internal_topology_unique_vertices",
                &BRepLinesTopologyInspectionResult::
                    part_of_line_with_invalid_internal_topology_unique_vertices )
            .def_readwrite( "part_of_invalid_unique_line_unique_vertices",
                &BRepLinesTopologyInspectionResult::
                    part_of_invalid_unique_line_unique_vertices )
            .def_readwrite( "part_of_lines_but_not_corner_unique_vertices",
                &BRepLinesTopologyInspectionResult::
                    part_of_lines_but_not_corner_unique_vertices );

        pybind11::class_< BRepSurfacesTopologyInspectionResult >(
            module, "BRepSurfacesTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite(
                "part_of_not_boundary_nor_internal_surface_unique_vertices",
                &BRepSurfacesTopologyInspectionResult::
                    part_of_not_boundary_nor_internal_surface_unique_vertices )
            .def_readwrite( "part_of_surface_with_invalid_internal_topology_"
                            "unique_vertices",
                &BRepSurfacesTopologyInspectionResult::
                    part_of_surface_with_invalid_internal_topology_unique_vertices )
            .def_readwrite( "part_of_invalid_unique_surface_unique_vertices",
                &BRepSurfacesTopologyInspectionResult::
                    part_of_invalid_unique_surface_unique_vertices )
            .def_readwrite( "part_of_invalid_multiple_surfaces_unique_vertices",
                &BRepSurfacesTopologyInspectionResult::
                    part_of_invalid_multiple_surfaces_unique_vertices )
            .def_readwrite(
                "part_of_line_and_not_on_surface_border_unique_vertices",
                &BRepSurfacesTopologyInspectionResult::
                    part_of_line_and_not_on_surface_border_unique_vertices );

        pybind11::class_< BRepBlocksTopologyInspectionResult >(
            module, "BRepBlocksTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "part_of_invalid_blocks_unique_vertices",
                &BRepBlocksTopologyInspectionResult::
                    part_of_invalid_blocks_unique_vertices );

        pybind11::class_< BRepTopologyInspectionResult >(
            module, "BRepTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "corners", &BRepTopologyInspectionResult::corners )
            .def_readwrite( "lines", &BRepTopologyInspectionResult::lines )
            .def_readwrite(
                "surfaces", &BRepTopologyInspectionResult::surfaces )
            .def_readwrite( "blocks", &BRepTopologyInspectionResult::blocks );

        pybind11::class_< BRepTopologyInspector >(
            module, "BRepTopologyInspector" )
            .def( pybind11::init< const BRep& >() )
            .def( pybind11::init< const BRep&, bool >() )
            .def( "brep_topology_is_valid",
                &BRepTopologyInspector::brep_topology_is_valid )
            .def( "brep_meshed_components_are_linked_to_unique_vertices",
                &BRepTopologyInspector::
                    brep_meshed_components_are_linked_to_unique_vertices )
            .def( "brep_unique_vertices_are_linked_to_a_component_vertex",
                &BRepTopologyInspector::
                    brep_unique_vertices_are_linked_to_a_component_vertex )
            /*.def( "unique_vertices_not_linked_to_a_component_vertex",
                &BRepTopologyInspector::
                    unique_vertices_not_linked_to_a_component_vertex )
        */
            .def( "invalid_components_topology_unique_vertices",
                &BRepTopologyInspector::
                    invalid_components_topology_unique_vertices )
            .def( "inspect_brep_topology",
                &BRepTopologyInspector::inspect_brep_topology );
    }
} // namespace geode
