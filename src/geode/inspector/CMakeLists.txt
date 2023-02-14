# Copyright (c) 2019 - 2023 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

add_geode_library(
    NAME inspector
    FOLDER "geode/inspector"
    SOURCES
        "common.cpp"
        "criterion/adjacency/surface_adjacency.cpp"
        "criterion/adjacency/solid_adjacency.cpp"
        "criterion/private/component_meshes_adjacency.cpp"
        "criterion/adjacency/section_meshes_adjacency.cpp"
        "criterion/adjacency/brep_meshes_adjacency.cpp"
        "criterion/private/colocation_impl.cpp"
        "criterion/colocation/pointset_colocation.cpp"
        "criterion/colocation/edgedcurve_colocation.cpp"
        "criterion/colocation/surface_colocation.cpp"
        "criterion/colocation/solid_colocation.cpp"
        "criterion/colocation/unique_vertices_colocation.cpp"
        "criterion/colocation/component_meshes_colocation.cpp"
        "criterion/private/degeneration_impl.cpp"
        "criterion/degeneration/edgedcurve_degeneration.cpp"
        "criterion/degeneration/surface_degeneration.cpp"
        "criterion/degeneration/solid_degeneration.cpp"
        "criterion/degeneration/component_meshes_degeneration.cpp"
        "criterion/intersections/surface_intersections.cpp"
        "criterion/intersections/surface_curve_intersections.cpp"
        "criterion/intersections/model_intersections.cpp"
        "criterion/manifold/surface_vertex_manifold.cpp"
        "criterion/manifold/surface_edge_manifold.cpp"
        "criterion/manifold/solid_vertex_manifold.cpp"
        "criterion/manifold/solid_edge_manifold.cpp"
        "criterion/manifold/solid_facet_manifold.cpp"
        "criterion/private/component_meshes_manifold.cpp"
        "criterion/manifold/section_meshes_manifold.cpp"
        "criterion/manifold/brep_meshes_manifold.cpp"
        "topology/brep_topology.cpp"
        "topology/private/brep_corners_topology_impl.cpp"
        "topology/private/brep_lines_topology_impl.cpp"
        "topology/private/brep_surfaces_topology_impl.cpp"
        "topology/private/brep_blocks_topology_impl.cpp"
        "topology/section_topology.cpp"
        "topology/private/section_corners_topology_impl.cpp"
        "topology/private/section_lines_topology_impl.cpp"
        "topology/private/section_surfaces_topology_impl.cpp"
        "topology/private/topology_helpers.cpp"
        "section_inspector.cpp"
        "brep_inspector.cpp"
        "pointset_inspector.cpp"
        "edgedcurve_inspector.cpp"
        "surface_inspector.cpp"
        "solid_inspector.cpp"
    PUBLIC_HEADERS
        "common.h"
        "criterion/adjacency/surface_adjacency.h"
        "criterion/adjacency/solid_adjacency.h"
        "criterion/adjacency/section_meshes_adjacency.h"
        "criterion/adjacency/brep_meshes_adjacency.h"
        "criterion/colocation/pointset_colocation.h"
        "criterion/colocation/edgedcurve_colocation.h"
        "criterion/colocation/surface_colocation.h"
        "criterion/colocation/solid_colocation.h"
        "criterion/colocation/unique_vertices_colocation.h"
        "criterion/colocation/component_meshes_colocation.h"
        "criterion/degeneration/edgedcurve_degeneration.h"
        "criterion/degeneration/surface_degeneration.h"
        "criterion/degeneration/solid_degeneration.h"
        "criterion/degeneration/component_meshes_degeneration.h"
        "criterion/intersections/surface_intersections.h"
        "criterion/intersections/surface_curve_intersections.h"
        "criterion/intersections/model_intersections.h"
        "criterion/manifold/surface_vertex_manifold.h"
        "criterion/manifold/surface_edge_manifold.h"
        "criterion/manifold/solid_vertex_manifold.h"
        "criterion/manifold/solid_edge_manifold.h"
        "criterion/manifold/solid_facet_manifold.h"
        "criterion/manifold/section_meshes_manifold.h"
        "criterion/manifold/brep_meshes_manifold.h"
        "topology/brep_topology.h"
        "topology/section_topology.h"
        "mixin/add_inspectors.h"
        "section_inspector.h"
        "brep_inspector.h"
        "pointset_inspector.h"
        "edgedcurve_inspector.h"
        "surface_inspector.h"
        "solid_inspector.h"
    PRIVATE_HEADERS
        "criterion/private/component_meshes_adjacency.h"
        "criterion/private/colocation_impl.h"
        "criterion/private/degeneration_impl.h"
        "criterion/private/component_meshes_manifold.h"
        "topology/private/brep_corners_topology_impl.h"
        "topology/private/brep_lines_topology_impl.h"
        "topology/private/brep_surfaces_topology_impl.h"
        "topology/private/brep_blocks_topology_impl.h"
        "topology/private/section_corners_topology_impl.h"
        "topology/private/section_lines_topology_impl.h"
        "topology/private/section_surfaces_topology_impl.h"
        "topology/private/topology_helpers.h"
    PUBLIC_DEPENDENCIES
        OpenGeode::basic
    PRIVATE_DEPENDENCIES
        OpenGeode::geometry
        OpenGeode::mesh
        OpenGeode::model
)

