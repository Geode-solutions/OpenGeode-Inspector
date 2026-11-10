# -*- coding: utf-8 -*-
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


import geode_test_env

import opengeode as geode
import opengeode_inspector_py_inspection as inspector


def point(dimension, x, y):
    if dimension == 2:
        return geode.Point2D([x, y])
    return geode.Point3D([x, y, 0.0])


def create_curve(dimension):
    curve = getattr(geode, "EdgedCurve" + str(dimension) + "D").create()
    builder = getattr(geode, "EdgedCurveBuilder" + str(dimension) + "D").create(curve)
    builder.create_vertices(4)
    for vertex in range(4):
        builder.set_point(vertex, point(dimension, float(vertex), float(vertex % 2)))
    builder.create_edge_with_vertices(0, 1)
    builder.create_edge_with_vertices(1, 3)
    return curve, builder


def check_no_isolated_vertices(dimension):
    curve, builder = create_curve(dimension)
    builder.create_edge_with_vertices(3, 2)

    isolation_inspector = getattr(
        inspector, "EdgedCurveIsolation" + str(dimension) + "D"
    )(curve)
    if isolation_inspector.mesh_has_isolated_vertices():
        raise ValueError(
            "[Test] EdgedCurve is shown with isolated vertices whereas it has none."
        )
    if not isolation_inspector.isolated_vertices().nb_issues() == 0:
        raise ValueError("[Test] EdgedCurve has more isolated vertices than it should.")


def check_isolated_vertices(dimension):
    curve, builder = create_curve(dimension)

    isolation_inspector = getattr(
        inspector, "EdgedCurveIsolation" + str(dimension) + "D"
    )(curve)
    if not isolation_inspector.mesh_has_isolated_vertices():
        raise ValueError(
            "[Test] EdgedCurve is shown without isolated vertices whereas it has one."
        )
    isolated_vertices = isolation_inspector.isolated_vertices()
    if not isolated_vertices.nb_issues() == 1:
        raise ValueError("[Test] EdgedCurve has wrong number of isolated vertices.")
    if not isolated_vertices.issues()[0] == 2:
        raise ValueError("[Test] EdgedCurve has wrong isolated vertices.")

    result = getattr(inspector, "inspect_edged_curve" + str(dimension) + "D")(curve)
    if not result.isolated_vertices.nb_issues() == 1:
        raise ValueError(
            "[Test] EdgedCurve inspection has wrong number of isolated vertices."
        )


if __name__ == "__main__":
    inspector.OpenGeodeInspectorInspectionLibrary.initialize()
    for dimension in [2, 3]:
        check_no_isolated_vertices(dimension)
        check_isolated_vertices(dimension)
