# Copyright (c) 2019 - 2024 Geode-solutions
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
        "criterion/brep_meshes_inspector.cpp"
        "criterion/section_meshes_inspector.cpp"
        "criterion/adjacency/surface_adjacency.cpp"
        "criterion/adjacency/solid_adjacency.cpp"
        "criterion/internal/component_meshes_adjacency.cpp"
        "criterion/adjacency/section_meshes_adjacency.cpp"
        "criterion/adjacency/brep_meshes_adjacency.cpp"
        "criterion/internal/colocation_impl.cpp"
        "criterion/colocation/pointset_colocation.cpp"
        "criterion/colocation/edgedcurve_colocation.cpp"
        "criterion/colocation/surface_colocation.cpp"
        "criterion/colocation/solid_colocation.cpp"
        "criterion/colocation/unique_vertices_colocation.cpp"
        "criterion/colocation/component_meshes_colocation.cpp"
        "criterion/internal/degeneration_impl.cpp"
        "criterion/internal/component_meshes_degeneration.cpp"
        "criterion/degeneration/edgedcurve_degeneration.cpp"
        "criterion/degeneration/surface_degeneration.cpp"
        "criterion/degeneration/solid_degeneration.cpp"
        "criterion/degeneration/brep_meshes_degeneration.cpp"
        "criterion/degeneration/section_meshes_degeneration.cpp"
        "criterion/intersections/surface_intersections.cpp"
        "criterion/intersections/surface_curve_intersections.cpp"
        "criterion/intersections/model_intersections.cpp"
        "criterion/manifold/surface_vertex_manifold.cpp"
        "criterion/manifold/surface_edge_manifold.cpp"
        "criterion/manifold/solid_vertex_manifold.cpp"
        "criterion/manifold/solid_edge_manifold.cpp"
        "criterion/manifold/solid_facet_manifold.cpp"
        "criterion/negative_elements/brep_meshes_negative_elements.cpp"
        "criterion/negative_elements/section_meshes_negative_elements.cpp"
        "criterion/negative_elements/solid_negative_elements.cpp"
        "criterion/negative_elements/surface_negative_elements.cpp"
        "criterion/internal/component_meshes_manifold.cpp"
        "criterion/manifold/section_meshes_manifold.cpp"
        "criterion/manifold/brep_meshes_manifold.cpp"
        "topology/brep_topology.cpp"
        "topology/brep_corners_topology.cpp"
        "topology/brep_lines_topology.cpp"
        "topology/brep_surfaces_topology.cpp"
        "topology/brep_blocks_topology.cpp"
        "topology/section_topology.cpp"
        "topology/section_corners_topology.cpp"
        "topology/section_lines_topology.cpp"
        "topology/section_surfaces_topology.cpp"
        "topology/internal/topology_helpers.cpp"
        "section_inspector.cpp"
        "brep_inspector.cpp"
        "pointset_inspector.cpp"
        "edgedcurve_inspector.cpp"
        "surface_inspector.cpp"
        "solid_inspector.cpp"
    PUBLIC_HEADERS
        "common.hpp"
        "criterion/brep_meshes_inspector.hpp"
        "criterion/section_meshes_inspector.hpp"
        "criterion/adjacency/surface_adjacency.hpp"
        "criterion/adjacency/solid_adjacency.hpp"
        "criterion/adjacency/section_meshes_adjacency.hpp"
        "criterion/adjacency/brep_meshes_adjacency.hpp"
        "criterion/colocation/pointset_colocation.hpp"
        "criterion/colocation/edgedcurve_colocation.hpp"
        "criterion/colocation/surface_colocation.hpp"
        "criterion/colocation/solid_colocation.hpp"
        "criterion/colocation/unique_vertices_colocation.hpp"
        "criterion/colocation/component_meshes_colocation.hpp"
        "criterion/degeneration/edgedcurve_degeneration.hpp"
        "criterion/degeneration/surface_degeneration.hpp"
        "criterion/degeneration/solid_degeneration.hpp"
        "criterion/degeneration/brep_meshes_degeneration.hpp"
        "criterion/degeneration/section_meshes_degeneration.hpp"
        "criterion/intersections/surface_intersections.hpp"
        "criterion/intersections/surface_curve_intersections.hpp"
        "criterion/intersections/model_intersections.hpp"
        "criterion/manifold/surface_vertex_manifold.hpp"
        "criterion/manifold/surface_edge_manifold.hpp"
        "criterion/manifold/solid_vertex_manifold.hpp"
        "criterion/manifold/solid_edge_manifold.hpp"
        "criterion/manifold/solid_facet_manifold.hpp"
        "criterion/manifold/section_meshes_manifold.hpp"
        "criterion/manifold/brep_meshes_manifold.hpp"
        "criterion/negative_elements/brep_meshes_negative_elements.hpp"
        "criterion/negative_elements/section_meshes_negative_elements.hpp"
        "criterion/negative_elements/solid_negative_elements.hpp"
        "criterion/negative_elements/surface_negative_elements.hpp"
        "topology/brep_topology.hpp"
        "topology/section_topology.hpp"
        "mixin/add_inspectors.hpp"
        "section_inspector.hpp"
        "brep_inspector.hpp"
        "information.hpp"
        "pointset_inspector.hpp"
        "edgedcurve_inspector.hpp"
        "surface_inspector.hpp"
        "solid_inspector.hpp"
    INTERNAL_HEADERS
        "criterion/internal/colocation_impl.hpp"
        "criterion/internal/component_meshes_adjacency.hpp"
        "criterion/internal/component_meshes_degeneration.hpp"
        "criterion/internal/component_meshes_manifold.hpp"
        "criterion/internal/degeneration_impl.hpp"
        "topology/brep_corners_topology.hpp"
        "topology/brep_lines_topology.hpp"
        "topology/brep_surfaces_topology.hpp"
        "topology/brep_blocks_topology.hpp"
        "topology/section_corners_topology.hpp"
        "topology/section_lines_topology.hpp"
        "topology/section_surfaces_topology.hpp"
        "topology/internal/topology_helpers.hpp"
    PUBLIC_DEPENDENCIES
        OpenGeode::basic
    PRIVATE_DEPENDENCIES
        OpenGeode::geometry
        OpenGeode::mesh
        OpenGeode::model
)

