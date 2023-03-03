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

add_geode_python_binding(
    NAME "py_inspector"
    SOURCES
        "adjacency/surface_adjacency.h"
        "adjacency/solid_adjacency.h"
        "adjacency/brep_meshes_adjacency.h"
        "adjacency/section_meshes_adjacency.h"
        "colocation/edgedcurve_colocation.h"
        "colocation/pointset_colocation.h"
        "colocation/surface_colocation.h"
        "colocation/solid_colocation.h"
        "colocation/unique_vertices_colocation.h"
        "colocation/component_meshes_colocation.h"
        "degeneration/edgedcurve_degeneration.h"
        "degeneration/solid_degeneration.h"
        "degeneration/surface_degeneration.h"
        "degeneration/component_meshes_degeneration.h"
        "intersections/surface_intersections.h"
        "intersections/surface_curve_intersections.h"
        "intersections/model_intersections.h"
        "manifold/surface_edge_manifold.h"
        "manifold/surface_vertex_manifold.h"
        "manifold/solid_vertex_manifold.h"
        "manifold/solid_edge_manifold.h"
        "manifold/solid_facet_manifold.h"
        "manifold/brep_meshes_manifold.h"
        "manifold/section_meshes_manifold.h"
        "topology/brep_topology.h"
        "topology/section_topology.h"
        "inspector.cpp"
        "edgedcurve_inspector.h"
        "brep_inspector.h"
        "pointset_inspector.h"
        "section_inspector.h"
        "solid_inspector.h"
        "surface_inspector.h"
    DEPENDENCIES
        ${PROJECT_NAME}::inspector
)