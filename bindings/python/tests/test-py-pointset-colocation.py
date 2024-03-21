# -*- coding: utf-8 -*-
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

import opengeode as geode
import os
import sys
import platform
if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode_inspector_py_inspector as inspector
print(dir(inspector))


def check_non_colocation2D():
    pointset = geode.PointSet2D.create()
    builder = geode.PointSetBuilder2D.create(pointset)
    builder.create_vertices(4)
    builder.set_point(0, geode.Point2D([0., 2.]))
    builder.set_point(1, geode.Point2D([2., 0.]))
    builder.set_point(2, geode.Point2D([1., 4.]))
    builder.set_point(3, geode.Point2D([3., 3.]))

    colocation_inspector = inspector.PointSetColocation2D(pointset)
    if colocation_inspector.mesh_has_colocated_points():
        raise ValueError(
            "[Test] PointSet has colocated points when it should have none.")
    if not colocation_inspector.colocated_points_groups().nb_issues() == 0:
        raise ValueError(
            "[Test] PointSet has more colocated points than it should.")

def check_colocation2D():
    pointset = geode.PointSet2D.create()
    builder = geode.PointSetBuilder2D.create(pointset)
    builder.create_vertices(7)
    builder.set_point(0, geode.Point2D([0., 2.]))
    builder.set_point(1, geode.Point2D([0., 2.]))
    builder.set_point(2, geode.Point2D([0., 0.]))
    builder.set_point(3, geode.Point2D([2., 0.]))
    builder.set_point(4, geode.Point2D([1., 4.]))
    builder.set_point(5, geode.Point2D([2., geode.global_epsilon / 2]))
    builder.set_point(6, geode.Point2D([geode.global_epsilon / 1.1, 2.]))

    colocation_inspector = inspector.PointSetColocation2D(pointset)
    if not colocation_inspector.mesh_has_colocated_points():
        raise ValueError(
            "[Test] PointSet doesn't have colocated points whereas it should have several.")
    if not colocation_inspector.colocated_points_groups().nb_issues() == 2:
        raise ValueError(
            "[Test] PointSet has wrong number of group of colocated points.")
    first_colocated_points_group = [0, 1, 6]
    if not colocation_inspector.colocated_points_groups().issues()[0] == first_colocated_points_group:
        raise ValueError(
            "[Test] PointSet has wrong first colocated points group.")
    second_colocated_points_group = [3, 5]
    if not colocation_inspector.colocated_points_groups().issues()[1] == second_colocated_points_group:
        raise ValueError(
            "[Test] PointSet has wrong second colocated points group.")


def check_non_colocation3D():
    pointset = geode.PointSet3D.create()
    builder = geode.PointSetBuilder3D.create(pointset)
    builder.create_vertices(4)
    builder.set_point(0, geode.Point3D([0., 2., 0.]))
    builder.set_point(1, geode.Point3D([2., 0., 0.5]))
    builder.set_point(2, geode.Point3D([1., 4., 1.]))
    builder.set_point(3, geode.Point3D([3., 3., 2.]))

    colocation_inspector = inspector.PointSetColocation3D(pointset)
    if colocation_inspector.mesh_has_colocated_points():
        raise ValueError(
            "[Test] (3D) PointSet has colocated points when it should have none.")
    if not colocation_inspector.colocated_points_groups().nb_issues() == 0:
        raise ValueError(
            "[Test] (3D) PointSet has more colocated points than it should.")

def check_colocation3D():
    pointset = geode.PointSet3D.create()
    builder = geode.PointSetBuilder3D.create(pointset)
    builder.create_vertices(7)
    builder.set_point(0, geode.Point3D([0., 2., 1.]))
    builder.set_point(1, geode.Point3D([0., 2., 1.]))
    builder.set_point(2, geode.Point3D([0., 0., 0.]))
    builder.set_point(3, geode.Point3D([2., 0., 0.]))
    builder.set_point(4, geode.Point3D([1., 4., 3.]))
    builder.set_point(
        5, geode.Point3D([2., geode.global_epsilon / 2, geode.global_epsilon / 2]))
    builder.set_point(6, geode.Point3D([geode.global_epsilon / 1.1, 2., 1.]))

    colocation_inspector = inspector.PointSetColocation3D(pointset)
    if not colocation_inspector.mesh_has_colocated_points():
        raise ValueError(
            "[Test] (3D) PointSet doesn't have colocated points whereas it should have several.")
    if not colocation_inspector.colocated_points_groups().nb_issues() == 2:
        raise ValueError(
            "[Test] (3D) PointSet has wrong number of colocated points.")
    first_colocated_points_group = [0, 1, 6]
    if not colocation_inspector.colocated_points_groups().issues()[0] == first_colocated_points_group:
        raise ValueError(
            "[Test] (3D) PointSet has wrong first colocated points group.")
    second_colocated_points_group = [3, 5]
    if not colocation_inspector.colocated_points_groups().issues()[1] == second_colocated_points_group:
        raise ValueError(
            "[Test] (3D) PointSet has wrong second colocated points group.")


if __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    check_non_colocation2D()
    check_colocation2D()
    check_non_colocation3D()
    check_colocation3D()
