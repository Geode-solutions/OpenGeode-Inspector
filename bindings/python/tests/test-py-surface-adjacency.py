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

def check_adjacency2D():
    surface = geode.TriangulatedSurface2D.create()
    builder = geode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(4)
    builder.set_point(0, geode.Point2D([0., 1.]))
    builder.set_point(1, geode.Point2D([3., 0.]))
    builder.set_point(2, geode.Point2D([2., 4.]))
    builder.set_point(3, geode.Point2D([5., 3.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([2, 1, 3])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 0), 0)

    adjacency_inspector = inspector.SurfaceMeshAdjacency2D(surface)
    if adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] Surface has wrong adjacencies when it should have none.")
    if not adjacency_inspector.nb_edges_with_wrong_adjacency() == 0:
        raise ValueError(
            "[Test] Surface has more wrong adjacencies on edges than it should.")
    if adjacency_inspector.polygon_edges_with_wrong_adjacency():
        raise ValueError(
            "[Test] Surface edges adjacencies are shown wrong whereas they are not.")

def check_non_adjacency_no_bijection2D():
    surface = geode.TriangulatedSurface2D.create()
    builder = geode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point2D([0., 1.]))
    builder.set_point(1, geode.Point2D([3., 0.]))
    builder.set_point(2, geode.Point2D([2., 4.]))
    builder.set_point(3, geode.Point2D([5., 3.]))
    builder.set_point(4, geode.Point2D([1., 4.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([2, 1, 3])
    builder.create_triangle([4, 1, 2])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 0), 0)
    builder.set_polygon_adjacent(geode.PolygonEdge(2, 1), 1)

    adjacency_inspector = inspector.SurfaceMeshAdjacency2D(surface)
    if not adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] Surface should have a wrong adjacency due to non-bijection.")
    if not adjacency_inspector.nb_edges_with_wrong_adjacency() == 1:
        raise ValueError(
            "[Test] Surface should have one wrong adjacency due to non-bijection.")
    polygon_edge = geode.PolygonEdge(2, 1)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[0] == polygon_edge:
        raise ValueError("[Test] Surface edges show wrong adjacency problems.")

def check_non_adjacency_wrong_edge2D():
    surface = geode.TriangulatedSurface2D.create()
    builder = geode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(4)
    builder.set_point(0, geode.Point2D([0., 1.]))
    builder.set_point(1, geode.Point2D([3., 0.]))
    builder.set_point(2, geode.Point2D([2., 4.]))
    builder.set_point(3, geode.Point2D([5., 3.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([2, 1, 3])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 1), 0)

    adjacency_inspector = inspector.SurfaceMeshAdjacency2D(surface)
    if not adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] Surface should have wrong adjacencies due to wrong edge for adjacency.")
    if not adjacency_inspector.nb_edges_with_wrong_adjacency() == 2:
        raise ValueError(
            "[Test] Surface should have two wrong adjacencies due to wrong edge for adjacency.")
    polygon_edge1 = geode.PolygonEdge(0, 1)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[0] == polygon_edge1:
        raise ValueError(
            "[Test] Surface shows wrong first edge with adjacency problems.")
    polygon_edge2 = geode.PolygonEdge(1, 1)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[1] == polygon_edge2:
        raise ValueError(
            "[Test] Surface shows wrong second edge with adjacency problems.")

def check_non_adjacency_inversed_triangle2D():
    surface = geode.TriangulatedSurface2D.create()
    builder = geode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(4)
    builder.set_point(0, geode.Point2D([0., 1.]))
    builder.set_point(1, geode.Point2D([3., 0.]))
    builder.set_point(2, geode.Point2D([2., 4.]))
    builder.set_point(3, geode.Point2D([5., 3.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 2, 3])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 0), 0)

    adjacency_inspector = inspector.SurfaceMeshAdjacency2D(surface)
    if not adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] Surface should have wrong adjacencies due to an inversed triangle.")
    if not adjacency_inspector.nb_edges_with_wrong_adjacency() == 2:
        raise ValueError(
            "[Test] Surface should have two wrong adjacencies due to an inversed triangle.")
    polygon_edge1 = geode.PolygonEdge(0, 1)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[0] == polygon_edge1:
        raise ValueError(
            "[Test] Surface shows wrong first edge with adjacency problems due to an inversed triangle..")
    polygon_edge2 = geode.PolygonEdge(1, 0)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[1] == polygon_edge2:
        raise ValueError(
            "[Test] Surface shows wrong second edge with adjacency problems due to an inversed triangle..")

def check_adjacency3D():
    surface = geode.TriangulatedSurface3D.create()
    builder = geode.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_vertices(4)
    builder.set_point(0, geode.Point3D([0., 1., 0.]))
    builder.set_point(1, geode.Point3D([3., 0., 1.]))
    builder.set_point(2, geode.Point3D([2., 4., 2.]))
    builder.set_point(3, geode.Point3D([5., 3., 0.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([2, 1, 3])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 0), 0)

    adjacency_inspector = inspector.SurfaceMeshAdjacency3D(surface)
    if adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] 3D Surface has wrong adjacencies when it should have none.")
    if not adjacency_inspector.nb_edges_with_wrong_adjacency() == 0:
        raise ValueError(
            "[Test] 3D Surface has more wrong adjacencies on edges than it should.")
    if adjacency_inspector.polygon_edges_with_wrong_adjacency():
        raise ValueError(
            "[Test] 3D Surface edges adjacencies are shown wrong whereas they are not.")

def check_non_adjacency_no_bijection3D():
    surface = geode.TriangulatedSurface3D.create()
    builder = geode.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point3D([0., 1., 0.]))
    builder.set_point(1, geode.Point3D([3., 0., 1.]))
    builder.set_point(2, geode.Point3D([2., 4., 2.]))
    builder.set_point(3, geode.Point3D([5., 3., 0.]))
    builder.set_point(4, geode.Point3D([1., 4., 4.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([2, 1, 3])
    builder.create_triangle([4, 1, 2])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 0), 0)
    builder.set_polygon_adjacent(geode.PolygonEdge(2, 1), 1)

    adjacency_inspector = inspector.SurfaceMeshAdjacency3D(surface)
    if not adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] 3D Surface should have a wrong adjacency due to non-bijection.")
    if not adjacency_inspector.nb_edges_with_wrong_adjacency() == 1:
        raise ValueError(
            "[Test] 3D Surface should have one wrong adjacency due to non-bijection.")
    polygon_edge = geode.PolygonEdge(2, 1)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[0] == polygon_edge:
        raise ValueError(
            "[Test] 3D Surface edges show wrong adjacency problems.")

def check_non_adjacency_wrong_edge3D():
    surface = geode.TriangulatedSurface3D.create()
    builder = geode.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_vertices(4)
    builder.set_point(0, geode.Point3D([0., 1., 0.]))
    builder.set_point(1, geode.Point3D([3., 0., 1.]))
    builder.set_point(2, geode.Point3D([2., 4., 2.]))
    builder.set_point(3, geode.Point3D([5., 3., 0.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([2, 1, 3])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 1), 0)

    adjacency_inspector = inspector.SurfaceMeshAdjacency3D(surface)
    if not adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] 3D Surface should have wrong adjacencies due to wrong edge for adjacency.")
    if not adjacency_inspector.nb_edges_with_wrong_adjacency() == 2:
        raise ValueError(
            "[Test] 3D Surface should have two wrong adjacencies due to wrong edge for adjacency.")
    polygon_edge1 = geode.PolygonEdge(0, 1)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[0] == polygon_edge1:
        raise ValueError(
            "[Test] 3D Surface shows wrong first edge with adjacency problems.")
    polygon_edge2 = geode.PolygonEdge(1, 1)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[1] == polygon_edge2:
        raise ValueError(
            "[Test] Surface shows wrong second edge with adjacency problems.")

def check_non_adjacency_inversed_triangle3D():
    surface = geode.TriangulatedSurface3D.create()
    builder = geode.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_vertices(4)
    builder.set_point(0, geode.Point3D([0., 1., 0.]))
    builder.set_point(1, geode.Point3D([3., 0., 1.]))
    builder.set_point(2, geode.Point3D([2., 4., 2.]))
    builder.set_point(3, geode.Point3D([5., 3., 0.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 2, 3])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 1), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 0), 0)

    adjacency_inspector = inspector.SurfaceMeshAdjacency3D(surface)
    if not adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] 3D Surface should have wrong adjacencies due to an inversed triangle.")
    if not adjacency_inspector.nb_edges_with_wrong_adjacency() == 2:
        raise ValueError(
            "[Test] 3D Surface should have two wrong adjacencies due to an inversed triangle.")
    polygon_edge1 = geode.PolygonEdge(0, 1)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[0] == polygon_edge1:
        raise ValueError(
            "[Test] 3D Surface shows wrong first edge with adjacency problems due to an inversed triangle..")
    polygon_edge2 = geode.PolygonEdge(1, 0)
    if not adjacency_inspector.polygon_edges_with_wrong_adjacency()[1] == polygon_edge2:
        raise ValueError(
            "[Test] 3D Surface shows wrong second edge with adjacency problems due to an inversed triangle..")

if __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    check_adjacency2D()
    check_non_adjacency_no_bijection2D()
    check_non_adjacency_wrong_edge2D()
    check_non_adjacency_inversed_triangle2D()
    check_adjacency3D()
    check_non_adjacency_no_bijection3D()
    check_non_adjacency_wrong_edge3D()
    check_non_adjacency_inversed_triangle3D()
