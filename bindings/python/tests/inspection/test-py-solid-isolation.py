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


def create_solid():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point3D([0.0, 0.0, 2.0]))
    builder.set_point(1, geode.Point3D([3.0, 0.5, 0.0]))
    builder.set_point(2, geode.Point3D([0.5, 3.0, 0.5]))
    builder.set_point(3, geode.Point3D([3.5, 2.5, -0.5]))
    builder.set_point(4, geode.Point3D([2.0, 1.5, 3.0]))
    builder.create_tetrahedron([0, 1, 2, 4])
    return solid, builder


def check_no_isolated_vertices():
    solid, builder = create_solid()
    builder.create_tetrahedron([1, 3, 2, 4])
    builder.compute_polyhedron_adjacencies()

    isolation_inspector = inspector.SolidMeshIsolation3D(solid)
    if isolation_inspector.mesh_has_isolated_vertices():
        raise ValueError(
            "[Test] Solid is shown with isolated vertices whereas it has none."
        )
    if not isolation_inspector.isolated_vertices().nb_issues() == 0:
        raise ValueError("[Test] Solid has more isolated vertices than it should.")


def check_isolated_vertices():
    solid, builder = create_solid()

    isolation_inspector = inspector.SolidMeshIsolation3D(solid)
    if not isolation_inspector.mesh_has_isolated_vertices():
        raise ValueError(
            "[Test] Solid is shown without isolated vertices whereas it has one."
        )
    isolated_vertices = isolation_inspector.isolated_vertices()
    if not isolated_vertices.nb_issues() == 1:
        raise ValueError("[Test] Solid has wrong number of isolated vertices.")
    if not isolated_vertices.issues()[0] == 3:
        raise ValueError("[Test] Solid has wrong isolated vertices.")

    result = inspector.inspect_solid3D(solid)
    if not result.isolated_vertices.nb_issues() == 1:
        raise ValueError("[Test] Solid inspection has wrong number of isolated vertices.")


if __name__ == "__main__":
    inspector.OpenGeodeInspectorInspectionLibrary.initialize()
    check_no_isolated_vertices()
    check_isolated_vertices()
