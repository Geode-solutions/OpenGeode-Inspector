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

#define PYTHON_BREP_TOPOLOGY()                                                   \
    pybind11::class_< BRepTopologyInspector >(                                   \
        module, "BRepTopologyInspector" )                                        \
        .def( pybind11::init< const BRep& >() )                                  \
        .def( pybind11::init< const BRep&, bool >() )                            \
        .def( "brep_topology_is_valid",                                          \
            &BRepTopologyInspector::brep_topology_is_valid )                     \
        .def( "brep_meshed_components_are_linked_to_a_unique_vertex",            \
            &BRepTopologyInspector::                                             \
                brep_meshed_components_are_linked_to_a_unique_vertex )           \
        .def( "brep_unique_vertices_are_linked_to_a_component_vertex",           \
            &BRepTopologyInspector::                                             \
                brep_unique_vertices_are_linked_to_a_component_vertex )          \
        .def( "nb_corners_not_linked_to_a_unique_vertex",                        \
            &BRepTopologyInspector::nb_corners_not_linked_to_a_unique_vertex )   \
        .def( "nb_lines_meshed_but_not_linked_to_a_unique_vertex",               \
            &BRepTopologyInspector::                                             \
                nb_lines_meshed_but_not_linked_to_a_unique_vertex )              \
        .def( "nb_surfaces_meshed_but_not_linked_to_a_unique_vertex",            \
            &BRepTopologyInspector::                                             \
                nb_surfaces_meshed_but_not_linked_to_a_unique_vertex )           \
        .def( "nb_blocks_meshed_but_not_linked_to_a_unique_vertex",              \
            &BRepTopologyInspector::                                             \
                nb_blocks_meshed_but_not_linked_to_a_unique_vertex )             \
        .def( "nb_unique_vertices_not_linked_to_a_component_vertex",             \
            &BRepTopologyInspector::                                             \
                nb_unique_vertices_not_linked_to_a_component_vertex )            \
        .def( "unique_vertices_not_linked_to_a_component_vertex",                \
            &BRepTopologyInspector::                                             \
                unique_vertices_not_linked_to_a_component_vertex )               \
        .def( "invalid_components_topology_unique_vertices",                     \
            &BRepTopologyInspector::                                             \
                invalid_components_topology_unique_vertices )                    \
        .def( "multiple_corners_unique_vertices",                                \
            &BRepTopologyInspector::multiple_corners_unique_vertices )           \
        .def( "multiple_internals_corner_vertices",                              \
            &BRepTopologyInspector::multiple_internals_corner_vertices )         \
        .def( "not_internal_nor_boundary_corner_vertices",                       \
            &BRepTopologyInspector::                                             \
                not_internal_nor_boundary_corner_vertices )                      \
        .def( "line_corners_without_boundary_status",                            \
            &BRepTopologyInspector::line_corners_without_boundary_status )       \
        .def( "part_of_not_boundary_nor_internal_line_unique_vertices",          \
            &BRepTopologyInspector::                                             \
                part_of_not_boundary_nor_internal_line_unique_vertices )         \
        .def( "part_of_line_with_invalid_internal_topology_unique_vertices",     \
            &BRepTopologyInspector::                                             \
                part_of_line_with_invalid_internal_topology_unique_vertices )    \
        .def( "part_of_invalid_unique_line_unique_vertices",                     \
            &BRepTopologyInspector::                                             \
                part_of_invalid_unique_line_unique_vertices )                    \
        .def( "part_of_lines_but_not_corner_unique_vertices",                    \
            &BRepTopologyInspector::                                             \
                part_of_lines_but_not_corner_unique_vertices )                   \
        .def( "part_of_not_boundary_nor_internal_surface_unique_vertices",       \
            &BRepTopologyInspector::                                             \
                part_of_not_boundary_nor_internal_surface_unique_vertices )      \
        .def(                                                                    \
            "part_of_surface_with_invalid_internal_topology_unique_vertices",    \
            &BRepTopologyInspector::                                             \
                part_of_surface_with_invalid_internal_topology_unique_vertices ) \
        .def( "part_of_invalid_unique_surface_unique_vertices",                  \
            &BRepTopologyInspector::                                             \
                part_of_invalid_unique_surface_unique_vertices )                 \
        .def( "part_of_invalid_multiple_surfaces_unique_vertices",               \
            &BRepTopologyInspector::                                             \
                part_of_invalid_multiple_surfaces_unique_vertices )              \
        .def( "part_of_line_and_not_on_surface_border_unique_vertices",          \
            &BRepTopologyInspector::                                             \
                part_of_line_and_not_on_surface_border_unique_vertices )         \
        .def( "part_of_invalid_blocks_unique_vertices",                          \
            &BRepTopologyInspector::part_of_invalid_blocks_unique_vertices )

namespace geode
{
    void define_brep_topology_inspector( pybind11::module& module )
    {
        PYTHON_BREP_TOPOLOGY();
    }
} // namespace geode
