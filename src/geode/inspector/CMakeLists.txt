# Copyright (c) 2019 - 2021 Geode-solutions
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
        "criterion/degeneration/edgedcurve_degeneration.cpp"
        "criterion/degeneration/surface_degeneration.cpp"
        "criterion/degeneration/solid_degeneration.cpp"
        "criterion/private/degeneration_impl.cpp"
        "criterion/colocation/pointset_colocation.cpp"
        "criterion/colocation/edgedcurve_colocation.cpp"
        "criterion/colocation/surface_colocation.cpp"
        "criterion/colocation/solid_colocation.cpp"
        "criterion/adjacency/surface_adjacency.cpp"
        "criterion/manifold/surface_vertex_manifold.cpp"
        "criterion/manifold/surface_edge_manifold.cpp"
        "criterion/private/colocation_impl.cpp"
    PUBLIC_HEADERS
        "common.h"
        "criterion/degeneration/edgedcurve_degeneration.h"
        "criterion/degeneration/surface_degeneration.h"
        "criterion/degeneration/solid_degeneration.h"
        "criterion/colocation/pointset_colocation.h"
        "criterion/colocation/edgedcurve_colocation.h"
        "criterion/colocation/surface_colocation.h"
        "criterion/colocation/solid_colocation.h"
        "criterion/adjacency/surface_adjacency.h"
        "criterion/manifold/surface_vertex_manifold.h"
        "criterion/manifold/surface_edge_manifold.h"
    PRIVATE_HEADERS
        "criterion/private/degeneration_impl.h"
        "criterion/private/colocation_impl.h"
    PUBLIC_DEPENDENCIES
        OpenGeode::basic
    PRIVATE_DEPENDENCIES
        OpenGeode::geometry
        OpenGeode::mesh
)

