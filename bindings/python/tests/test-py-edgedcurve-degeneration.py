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


def check_non_degeneration2D():
    curve = geode.EdgedCurve2D.create()
    builder = geode.EdgedCurveBuilder2D.create(curve)
    builder.create_vertices(3)
    builder.set_point(0, geode.Point2D([0., 2.]))
    builder.set_point(1, geode.Point2D([2., 0.]))
    builder.set_point(2, geode.Point2D([1., 4.]))

    builder.create_edge_with_vertices(0, 1)
    builder.create_edge_with_vertices(1, 2)
    builder.create_edge_with_vertices(2, 0)

    degeneration_inspector = inspector.EdgedCurveDegeneration2D(curve)
    if degeneration_inspector.is_mesh_degenerated():
        raise ValueError(
            "[Test] EdgedCurve is shown degenerated whereas it is not.")
    if not degeneration_inspector.nb_degenerated_edges() == 0:
        raise ValueError(
            "[Test] EdgedCurve has more degenerated edges than it should.")
    if degeneration_inspector.degenerated_edges():
        raise ValueError(
            "[Test] EdgedCurve has degenerated edges when it should have none.")


def check_degeneration_by_colocalisation2D():
    curve = geode.EdgedCurve2D.create()
    builder = geode.EdgedCurveBuilder2D.create(curve)
    builder.create_vertices(3)
    builder.set_point(0, geode.Point2D([0., 2.]))
    builder.set_point(1, geode.Point2D([2., 0.]))
    builder.set_point(2, geode.Point2D([2., geode.global_epsilon / 2]))

    builder.create_edge_with_vertices(0, 1)
    builder.create_edge_with_vertices(0, 2)
    builder.create_edge_with_vertices(1, 2)

    degeneration_inspector = inspector.EdgedCurveDegeneration2D(curve)
    if not degeneration_inspector.is_mesh_degenerated():
        raise ValueError(
            "[Test] EdgedCurve is shown not degenerated whereas it is.")
    if not degeneration_inspector.nb_degenerated_edges() == 1:
        raise ValueError(
            "[Test] EdgedCurve has wrong number of degenerated edges.")
    if not degeneration_inspector.degenerated_edges()[0] == 2:
        raise ValueError("[Test] EdgedCurve has wrong degenerated edges.")


def check_non_degeneration3D():
    curve = geode.EdgedCurve3D.create()
    builder = geode.EdgedCurveBuilder3D.create(curve)
    builder.create_vertices(3)
    builder.set_point(0, geode.Point3D([0., 2., 0.]))
    builder.set_point(1, geode.Point3D([2., 0., 0.5]))
    builder.set_point(2, geode.Point3D([1., 4., 1.]))

    builder.create_edge_with_vertices(0, 1)
    builder.create_edge_with_vertices(1, 2)
    builder.create_edge_with_vertices(2, 0)

    degeneration_inspector = inspector.EdgedCurveDegeneration3D(curve)
    if degeneration_inspector.is_mesh_degenerated():
        raise ValueError(
            "[Test] (3D) EdgedCurve is shown degenerated whereas it is not.")
    if not degeneration_inspector.nb_degenerated_edges() == 0:
        raise ValueError(
            "[Test] (3D) EdgedCurve has more degenerated edges than it should.")
    if degeneration_inspector.degenerated_edges():
        raise ValueError(
            "[Test] (3D) EdgedCurve has degenerated edges when it should have none.")


def check_degeneration_by_colocalisation3D():
    curve = geode.EdgedCurve3D.create()
    builder = geode.EdgedCurveBuilder3D.create(curve)
    builder.create_vertices(3)
    builder.set_point(0, geode.Point3D([0., 2., 0.]))
    builder.set_point(1, geode.Point3D([2., 0., 0.5]))
    builder.set_point(2, geode.Point3D([2., geode.global_epsilon / 2,
                                        0.5 + geode.global_epsilon / 2]))

    builder.create_edge_with_vertices(0, 1)
    builder.create_edge_with_vertices(0, 2)
    builder.create_edge_with_vertices(1, 2)

    degeneration_inspector = inspector.EdgedCurveDegeneration3D(curve)
    if not degeneration_inspector.is_mesh_degenerated():
        raise ValueError(
            "[Test] (3D) EdgedCurve is shown not degenerated whereas it is.")
    if not degeneration_inspector.nb_degenerated_edges() == 1:
        raise ValueError(
            "[Test] (3D) EdgedCurve has wrong number of degenerated edges.")
    if not degeneration_inspector.degenerated_edges()[0] == 2:
        raise ValueError("[Test] (3D) EdgedCurve has wrong degenerated edges.")


if __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    check_non_degeneration2D()
    check_degeneration_by_colocalisation2D()
    check_non_degeneration3D()
    check_degeneration_by_colocalisation3D()
