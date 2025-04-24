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

#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/topology/brep_topology.hpp>

namespace geode
{

    void define_brep_topology_inspector( pybind11::module& module )
    {
        pybind11::class_< BRepCornersTopologyInspectionResult >(
            module, "BRepCornersTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "corners_not_meshed",
                &BRepCornersTopologyInspectionResult::corners_not_meshed )
            .def_readwrite( "corners_not_linked_to_a_unique_vertex",
                &BRepCornersTopologyInspectionResult::
                    corners_not_linked_to_a_unique_vertex )
            .def_readwrite( "unique_vertices_linked_to_multiple_corners",
                &BRepCornersTopologyInspectionResult::
                    unique_vertices_linked_to_multiple_corners )
            .def_readwrite(
                "unique_vertices_linked_to_multiple_internals_corner",
                &BRepCornersTopologyInspectionResult::
                    unique_vertices_linked_to_multiple_internals_corner )
            .def_readwrite(
                "unique_vertices_linked_to_not_internal_nor_boundary_corner",
                &BRepCornersTopologyInspectionResult::
                    unique_vertices_linked_to_not_internal_nor_boundary_corner )
            .def_readwrite( "unique_vertices_liked_to_not_boundary_line_corner",
                &BRepCornersTopologyInspectionResult::
                    unique_vertices_liked_to_not_boundary_line_corner )
            .def( "string", &BRepCornersTopologyInspectionResult::string )
            .def( "inspection_type",
                &BRepCornersTopologyInspectionResult::inspection_type );

        pybind11::class_< BRepLinesTopologyInspectionResult >(
            module, "BRepLinesTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "lines_not_meshed",
                &BRepLinesTopologyInspectionResult::lines_not_meshed )
            .def_readwrite( "lines_not_linked_to_a_unique_vertex",
                &BRepLinesTopologyInspectionResult::
                    lines_not_linked_to_a_unique_vertex )
            .def_readwrite(
                "unique_vertices_linked_to_not_internal_nor_boundary_line",
                &BRepLinesTopologyInspectionResult::
                    unique_vertices_linked_to_not_internal_nor_boundary_line )
            .def_readwrite(
                "unique_vertices_linked_to_a_line_with_invalid_embeddings",
                &BRepLinesTopologyInspectionResult::
                    unique_vertices_linked_to_a_line_with_invalid_embeddings )
            .def_readwrite(
                "unique_vertices_linked_to_a_single_and_invalid_line",
                &BRepLinesTopologyInspectionResult::
                    unique_vertices_linked_to_a_single_and_invalid_line )
            .def_readwrite( "unique_vertices_linked_to_several_lines_but_not_"
                            "linked_to_a_corner",
                &BRepLinesTopologyInspectionResult::
                    unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner )
            .def( "string", &BRepLinesTopologyInspectionResult::string )
            .def( "inspection_type",
                &BRepLinesTopologyInspectionResult::inspection_type );

        pybind11::class_< BRepSurfacesTopologyInspectionResult >(
            module, "BRepSurfacesTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "surfaces_not_meshed",
                &BRepSurfacesTopologyInspectionResult::surfaces_not_meshed )
            .def_readwrite( "surfaces_not_linked_to_a_unique_vertex",
                &BRepSurfacesTopologyInspectionResult::
                    surfaces_not_linked_to_a_unique_vertex )
            .def_readwrite(
                "unique_vertices_linked_to_not_internal_nor_boundary_surface",
                &BRepSurfacesTopologyInspectionResult::
                    unique_vertices_linked_to_not_internal_nor_boundary_surface )
            .def_readwrite(
                "unique_vertices_linked_to_a_surface_with_invalid_embbedings",
                &BRepSurfacesTopologyInspectionResult::
                    unique_vertices_linked_to_a_surface_with_invalid_embbedings )
            .def_readwrite(
                "unique_vertices_linked_to_a_single_and_invalid_surface",
                &BRepSurfacesTopologyInspectionResult::
                    unique_vertices_linked_to_a_single_and_invalid_surface )
            .def_readwrite(
                "unique_vertices_linked_to_several_and_invalid_surfaces",
                &BRepSurfacesTopologyInspectionResult::
                    unique_vertices_linked_to_several_and_invalid_surfaces )
            .def_readwrite( "unique_vertices_linked_to_a_line_but_is_not_on_a_"
                            "surface_border",
                &BRepSurfacesTopologyInspectionResult::
                    unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border )
            .def( "string", &BRepSurfacesTopologyInspectionResult::string )
            .def( "inspection_type",
                &BRepSurfacesTopologyInspectionResult::inspection_type );

        pybind11::class_< BRepBlocksTopologyInspectionResult >(
            module, "BRepBlocksTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "blocks_not_meshed",
                &BRepBlocksTopologyInspectionResult::blocks_not_meshed )
            .def_readwrite( "blocks_not_linked_to_a_unique_vertex",
                &BRepBlocksTopologyInspectionResult::
                    blocks_not_linked_to_a_unique_vertex )
            .def_readwrite(
                "unique_vertices_part_of_two_blocks_and_no_boundary_surface",
                &BRepBlocksTopologyInspectionResult::
                    unique_vertices_part_of_two_blocks_and_no_boundary_surface )
            .def_readwrite( "unique_vertices_with_incorrect_block_cmvs_count",
                &BRepBlocksTopologyInspectionResult::
                    unique_vertices_with_incorrect_block_cmvs_count )
            .def( "string", &BRepBlocksTopologyInspectionResult::string )
            .def( "inspection_type",
                &BRepBlocksTopologyInspectionResult::inspection_type );

        pybind11::class_< BRepTopologyInspectionResult >(
            module, "BRepTopologyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "corners", &BRepTopologyInspectionResult::corners )
            .def_readwrite( "lines", &BRepTopologyInspectionResult::lines )
            .def_readwrite(
                "surfaces", &BRepTopologyInspectionResult::surfaces )
            .def_readwrite( "blocks", &BRepTopologyInspectionResult::blocks )
            .def_readwrite( "unique_vertices_not_linked_to_any_component",
                &BRepTopologyInspectionResult::
                    unique_vertices_not_linked_to_any_component )
            .def( "string", &BRepTopologyInspectionResult::string )
            .def( "inspection_type",
                &BRepTopologyInspectionResult::inspection_type );

        pybind11::class_< BRepTopologyInspector >(
            module, "BRepTopologyInspector" )
            .def( pybind11::init< const BRep& >() )
            .def( "brep_topology_is_valid",
                &BRepTopologyInspector::brep_topology_is_valid )
            .def( "brep_unique_vertices_are_bijectively_linked_to_an_existing_"
                  "component_vertex",
                &BRepTopologyInspector::
                    brep_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex )
            .def( "inspect_brep_topology",
                &BRepTopologyInspector::inspect_brep_topology );
    }
} // namespace geode
