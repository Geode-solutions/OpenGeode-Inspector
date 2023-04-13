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
    builder = geode.TriangulatedSurfaceBuilder2D.create( surface )
    builder.create_vertices( 5 )
    builder.set_point( 0, geode.Point2D( [ 0., 0. ] ) )
    builder.set_point( 1, geode.Point2D( [ 3., -1. ] ) )
    builder.set_point( 2, geode.Point2D( [ 3., 3. ] ) )
    builder.set_point( 3, geode.Point2D( [ 0., -3. ] ) )
    builder.set_point( 4, geode.Point2D( [ 3., -3. ] ) )
    builder.create_triangle( [ 0, 1, 2 ] )
    builder.create_triangle( [ 0, 1, 3 ] )
    builder.create_triangle( [ 1, 3, 4 ] )
    builder.set_polygon_adjacent( geode.PolygonEdge( 0, 0 ), 1 )
    builder.set_polygon_adjacent( geode.PolygonEdge( 1, 0 ), 0 )
    builder.set_polygon_adjacent( geode.PolygonEdge( 1, 1 ), 2 )
    builder.set_polygon_adjacent( geode.PolygonEdge( 2, 0 ), 1 )

    curve = geode.EdgedCurve2D.create()
    builder_curve = geode.EdgedCurveBuilder2D.create( curve )
    builder_curve.create_vertices( 8 )
    builder_curve.set_point( 0, geode.Point2D( [  5., 0. ] ) )
    builder_curve.set_point( 1, geode.Point2D( [  3., 0. ] ) )
    builder_curve.set_point( 2, geode.Point2D( [  2., 0. ] ) )
    builder_curve.set_point( 3, geode.Point2D( [  1., 0. ] ) )
    builder_curve.set_point( 4, geode.Point2D( [  -1., 0. ] ) )
    builder_curve.set_point( 5, geode.Point2D( [  0., -3. ] ) )
    builder_curve.set_point( 6, geode.Point2D( [  3., -3. ] ) )
    builder_curve.set_point( 7, geode.Point2D( [  1.5, -2. ] ) )
    builder_curve.create_edge_with_vertices( 0, 1 )
    builder_curve.create_edge_with_vertices( 1, 2 )
    builder_curve.create_edge_with_vertices( 2, 3 )
    builder_curve.create_edge_with_vertices( 3, 4 )
    builder_curve.create_edge_with_vertices( 4, 5 )
    builder_curve.create_edge_with_vertices( 5, 6 )
    builder_curve.create_edge_with_vertices( 6, 7 )

    intersections_inspector = inspector.SurfaceCurveIntersections2D( surface, curve )
    if not intersections_inspector.meshes_have_intersections():
        raise ValueError( "[Test] 2D Surface and Curve should have intersections." )
    if not intersections_inspector.nb_intersecting_elements_pair() == 7:
        raise ValueError( "[Test] 2D Surface and Curve should have 7 intersecting elements pair." )

def check_intersections3D():
    surface = geode.TriangulatedSurface3D.create()
    builder = geode.TriangulatedSurfaceBuilder3D.create( surface )
    builder.create_vertices( 5 )
    builder.set_point( 0, geode.Point3D( [ 0., 0., 0 ] ) )
    builder.set_point( 1, geode.Point3D( [ 3., -1., 0 ] ) )
    builder.set_point( 2, geode.Point3D( [ 3., 3., 0 ] ) )
    builder.set_point( 3, geode.Point3D( [ 0., -3., 0 ] ) )
    builder.set_point( 4, geode.Point3D( [ 3., -3., 0 ] ) )
    builder.create_triangle( [ 0, 1, 2 ] )
    builder.create_triangle( [ 0, 1, 3 ] )
    builder.create_triangle( [ 1, 3, 4 ] )
    builder.set_polygon_adjacent( geode.PolygonEdge( 0, 0 ), 1 )
    builder.set_polygon_adjacent( geode.PolygonEdge( 1, 0 ), 0 )
    builder.set_polygon_adjacent( geode.PolygonEdge( 1, 1 ), 2 )
    builder.set_polygon_adjacent( geode.PolygonEdge( 2, 0 ), 1 )

    curve = geode.EdgedCurve3D.create()
    builder_curve = geode.EdgedCurveBuilder3D.create( curve )
    builder_curve.create_vertices( 9 )
    builder_curve.set_point( 0, geode.Point3D( [  5., 0., 1 ] ) )
    builder_curve.set_point( 1, geode.Point3D( [  3., 0., 0 ] ) )
    builder_curve.set_point( 2, geode.Point3D( [  2., 0., 0 ] ) )
    builder_curve.set_point( 3, geode.Point3D( [  1., 0., 0 ] ) )
    builder_curve.set_point( 4, geode.Point3D( [  -1., 0., 0 ] ) )
    builder_curve.set_point( 5, geode.Point3D( [  0., -3., 0 ] ) )
    builder_curve.set_point( 6, geode.Point3D( [  3., -3., 0 ] ) )
    builder_curve.set_point( 7, geode.Point3D( [  1.5, -2., 2 ] ) )
    builder_curve.set_point( 8, geode.Point3D( [  2, -2., -2 ] ) )
    builder_curve.create_edge_with_vertices( 0, 1 )
    builder_curve.create_edge_with_vertices( 1, 2 )
    builder_curve.create_edge_with_vertices( 2, 3 )
    builder_curve.create_edge_with_vertices( 3, 4 )
    builder_curve.create_edge_with_vertices( 4, 5 )
    builder_curve.create_edge_with_vertices( 5, 6 )
    builder_curve.create_edge_with_vertices( 6, 7 )
    builder_curve.create_edge_with_vertices( 7, 8 )

    intersections_inspector = inspector.SurfaceCurveIntersections3D(surface, curve )
    if not intersections_inspector.meshes_have_intersections():
        raise ValueError( "[Test] 3D Surface and Curve should have intersections." )
    if not intersections_inspector.nb_intersecting_elements_pair() == 6:
        raise ValueError( "[Test] 3D Surface and Curve should have 6 intersecting elements pair." )

if __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    check_intersections2D()
    check_intersections3D()
