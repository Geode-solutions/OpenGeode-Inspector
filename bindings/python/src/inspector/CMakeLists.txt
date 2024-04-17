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

add_geode_python_binding(
    NAME "py_inspector"
    SOURCES
        "criterion/adjacency/surface_adjacency.h"
        "criterion/adjacency/solid_adjacency.h"
        "criterion/adjacency/brep_meshes_adjacency.h"
        "criterion/adjacency/section_meshes_adjacency.h"
        "criterion/colocation/edgedcurve_colocation.h"
        "criterion/colocation/pointset_colocation.h"
        "criterion/colocation/surface_colocation.h"
        "criterion/colocation/solid_colocation.h"
        "criterion/colocation/unique_vertices_colocation.h"
        "criterion/colocation/component_meshes_colocation.h"
        "criterion/degeneration/edgedcurve_degeneration.h"
        "criterion/degeneration/solid_degeneration.h"
        "criterion/degeneration/surface_degeneration.h"
        "criterion/degeneration/brep_meshes_degeneration.h"
        "criterion/degeneration/section_meshes_degeneration.h"
        "criterion/intersections/surface_intersections.h"
        "criterion/intersections/surface_curve_intersections.h"
        "criterion/intersections/model_intersections.h"
        "criterion/manifold/surface_edge_manifold.h"
        "criterion/manifold/surface_vertex_manifold.h"
        "criterion/manifold/solid_vertex_manifold.h"
        "criterion/manifold/solid_edge_manifold.h"
        "criterion/manifold/solid_facet_manifold.h"
        "criterion/manifold/brep_meshes_manifold.h"
        "criterion/manifold/section_meshes_manifold.h"
        "criterion/brep_meshes_inspector.h"
        "criterion/section_meshes_inspector.h"
        "topology/brep_topology.h"
        "topology/section_topology.h"
        "inspector.cpp"
        "edgedcurve_inspector.h"
        "brep_inspector.h"
        "pointset_inspector.h"
        "section_inspector.h"
        "solid_inspector.h"
        "surface_inspector.h"
        "information.h"
    DEPENDENCIES
        ${PROJECT_NAME}::inspector
)