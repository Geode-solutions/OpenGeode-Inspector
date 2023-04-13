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


def check_intersections2D():
    surface = geode.TriangulatedSurface2D.create()
    builder = geode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point2D([0., 0.]))
    builder.set_point(1, geode.Point2D([3., 0.]))
    builder.set_point(2, geode.Point2D([0., 4.]))
    builder.set_point(3, geode.Point2D([3., 4.]))
    builder.set_point(4, geode.Point2D([-1., 2.]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([0, 1, 3])
    builder.create_triangle([1, 3, 4])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 0), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 0), 0)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 1), 2)
    builder.set_polygon_adjacent(geode.PolygonEdge(2, 0), 1)

    intersections_inspector = inspector.TriangulatedSurfaceIntersections2D(
        surface)
    if not intersections_inspector.mesh_has_self_intersections():
        raise ValueError("[Test] 2D Surface should have intersections.")
    if not intersections_inspector.nb_intersecting_elements_pair() == 3:
        raise ValueError(
            "[Test] 2D Surface should have 3 intersecting elements pair.")
    triangles_inter = intersections_inspector.intersecting_elements()
    if len(triangles_inter) != 3 or triangles_inter[0][0] != 2 or triangles_inter[0][1] != 0 or triangles_inter[1][0] != 2 or triangles_inter[1][1] != 1 or triangles_inter[2][0] != 0 or triangles_inter[2][1] != 1:
        raise ValueError(
            "[Test] 2D Surface has wrong intersecting elements pairs.")


def check_intersections3D():
    surface = geode.TriangulatedSurface3D.create()
    builder = geode.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_vertices(7)
    builder.set_point(0, geode.Point3D([0., 0., 0.]))
    builder.set_point(1, geode.Point3D([0., 0., 3.]))
    builder.set_point(2, geode.Point3D([0., 2., 0.]))
    builder.set_point(3, geode.Point3D([2., 0., 0.]))
    builder.set_point(4, geode.Point3D([5., 0., 1.5]))
    builder.set_point(5, geode.Point3D([2., 0.5, 2.]))
    builder.set_point(6, geode.Point3D([0., 0., 1.5]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([0, 2, 3])
    builder.create_triangle([3, 5, 2])
    builder.create_triangle([5, 2, 4])
    builder.create_triangle([4, 2, 6])
    builder.set_polygon_adjacent(geode.PolygonEdge(0, 2), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 0), 0)
    builder.set_polygon_adjacent(geode.PolygonEdge(1, 1), 2)
    builder.set_polygon_adjacent(geode.PolygonEdge(2, 2), 1)
    builder.set_polygon_adjacent(geode.PolygonEdge(2, 1), 3)
    builder.set_polygon_adjacent(geode.PolygonEdge(3, 0), 2)

    intersections_inspector = inspector.TriangulatedSurfaceIntersections3D(
        surface)
    if not intersections_inspector.mesh_has_self_intersections():
        raise ValueError("[Test] 3D Surface should have intersections.")
    nb_intersections = intersections_inspector.nb_intersecting_elements_pair()
    if not nb_intersections == 2:
        raise ValueError(
            "[Test] 3D Surface should have 2 intersecting elements pair.")
    triangles_inter = intersections_inspector.intersecting_elements()
    if len(triangles_inter) != 2 or triangles_inter[0][0] != 0 or triangles_inter[0][1] != 4 or triangles_inter[1][0] != 2 or triangles_inter[1][1] != 4:
        raise ValueError(
            "[Test] 3D Surface has wrong intersecting elements pairs.")


if __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    check_intersections2D()
    check_intersections3D()
