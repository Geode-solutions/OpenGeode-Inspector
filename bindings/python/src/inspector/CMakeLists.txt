# Copyright (c) 2019 - 2026 Geode-solutions
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

add_geode_python_binding(
    NAME "py_inspector"
    SOURCES
        "criterion/adjacency/surface_adjacency.hpp"
        "criterion/adjacency/solid_adjacency.hpp"
        "criterion/adjacency/brep_meshes_adjacency.hpp"
        "criterion/adjacency/section_meshes_adjacency.hpp"
        "criterion/colocation/edgedcurve_colocation.hpp"
        "criterion/colocation/pointset_colocation.hpp"
        "criterion/colocation/surface_colocation.hpp"
        "criterion/colocation/solid_colocation.hpp"
        "criterion/colocation/unique_vertices_colocation.hpp"
        "criterion/colocation/component_meshes_colocation.hpp"
        "criterion/degeneration/edgedcurve_degeneration.hpp"
        "criterion/degeneration/solid_degeneration.hpp"
        "criterion/degeneration/surface_degeneration.hpp"
        "criterion/degeneration/brep_meshes_degeneration.hpp"
        "criterion/degeneration/section_meshes_degeneration.hpp"
        "criterion/intersections/surface_intersections.hpp"
        "criterion/intersections/surface_curve_intersections.hpp"
        "criterion/intersections/model_intersections.hpp"
        "criterion/manifold/surface_edge_manifold.hpp"
        "criterion/manifold/surface_vertex_manifold.hpp"
        "criterion/manifold/solid_vertex_manifold.hpp"
        "criterion/manifold/solid_edge_manifold.hpp"
        "criterion/manifold/solid_facet_manifold.hpp"
        "criterion/manifold/brep_meshes_manifold.hpp"
        "criterion/manifold/section_meshes_manifold.hpp"
        "criterion/brep_meshes_inspector.hpp"
        "criterion/section_meshes_inspector.hpp"
        "topology/brep_topology.hpp"
        "topology/section_topology.hpp"
        "inspector.cpp"
        "edgedcurve_inspector.hpp"
        "brep_inspector.hpp"
        "pointset_inspector.hpp"
        "section_inspector.hpp"
        "solid_inspector.hpp"
        "surface_inspector.hpp"
        "information.hpp"
    DEPENDENCIES
        ${PROJECT_NAME}::inspector
)