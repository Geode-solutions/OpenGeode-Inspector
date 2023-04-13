# -*- coding: utf-8 -*-
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

import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode as geode
import opengeode_inspector_py_inspector as inspector


def check_vertex_manifold2D():
    surface = geode.TriangulatedSurface2D.create()
    builder = geode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(4)
    builder.set_point(0, geode.Point2D([0., 2.]))
    builder.set_point(1, geode.Point2D([2., 2.]))
    builder.set_point(2, geode.Point2D([2., 5.]))
    builder.set_point(3, geode.Point2D([3., 0.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 3, 2])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 2), 0)

    manifold_inspector = inspector.SurfaceMeshVertexManifold2D(surface)
    if not manifold_inspector.mesh_vertices_are_manifold():
        raise ValueError("[Test] Surface is shown non-manifold whereas it is.")
    if not manifold_inspector.nb_non_manifold_vertices() == 0:
        raise ValueError(
            "[Test] Surface has more non manifold vertices than it should.")
    if manifold_inspector.non_manifold_vertices():
        raise ValueError(
            "[Test] Surface vertices are shown non manifold whereas they are.")


def check_vertex_non_manifold2D():
    surface = geode.TriangulatedSurface2D.create()
    builder = geode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point2D([0., 2.]))
    builder.set_point(1, geode.Point2D([2., 2.]))
    builder.set_point(2, geode.Point2D([2., 5.]))
    builder.set_point(3, geode.Point2D([3., 0.]))
    builder.set_point(4, geode.Point2D([5., 3.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 3, 4])

    manifold_inspector = inspector.SurfaceMeshVertexManifold2D(surface)
    if manifold_inspector.mesh_vertices_are_manifold():
        raise ValueError(
            "[Test] Surface vertices are shown manifold whereas one is not.")
    if not manifold_inspector.nb_non_manifold_vertices() == 1:
        raise ValueError(
            "[Test] Surface has wrong number of non manifold vertices.")
    if not manifold_inspector.non_manifold_vertices()[0] == 1:
        raise ValueError("[Test] Surface shows wrong non manifold vertex id.")


def check_edge_manifold2D():
    surface = geode.TriangulatedSurface2D.create()
    builder = geode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point2D([0., 2.]))
    builder.set_point(1, geode.Point2D([2., 2.]))
    builder.set_point(2, geode.Point2D([2., 5.]))
    builder.set_point(3, geode.Point2D([3., 0.]))
    builder.set_point(4, geode.Point2D([5., 3.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 4, 2])
    builder.create_triangle([1, 3, 4])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 2), 0)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 0), 2)
    builder.set_polygon_adjacent(geode.PolygonEdge(2, 2), 1)

    manifold_inspector = inspector.SurfaceMeshEdgeManifold2D(surface)
    if not manifold_inspector.mesh_edges_are_manifold():
        raise ValueError(
            "[Test] Surface is shown non-manifold through edges whereas it is.")
    if not manifold_inspector.nb_non_manifold_edges() == 0:
        raise ValueError(
            "[Test] Surface has more non manifold edges than it should.")
    if manifold_inspector.non_manifold_edges():
        raise ValueError(
            "[Test] Surface edges are shown non manifold whereas they are.")


def check_edge_non_manifold2D():
    surface = geode.TriangulatedSurface2D.create()
    builder = geode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point2D([0., 2.]))
    builder.set_point(1, geode.Point2D([2., 2.]))
    builder.set_point(2, geode.Point2D([2., 5.]))
    builder.set_point(3, geode.Point2D([3., 0.]))
    builder.set_point(4, geode.Point2D([5., 3.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 4, 2])
    builder.create_triangle([1, 3, 2])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 2), 0)
    builder.set_polygon_adjacent(geode.PolygonEdge(2, 2), 0)

    manifold_inspector = inspector.SurfaceMeshEdgeManifold2D(surface)
    if manifold_inspector.mesh_edges_are_manifold():
        raise ValueError(
            "[Test] Surface is shown manifold through edges whereas it is not.")
    if not manifold_inspector.nb_non_manifold_edges() == 1:
        raise ValueError(
            "[Test] Surface has wrong number of non manifold edges.")


if __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    check_vertex_manifold2D()
    check_vertex_non_manifold2D()
    check_edge_manifold2D()
    check_edge_non_manifold2D()
