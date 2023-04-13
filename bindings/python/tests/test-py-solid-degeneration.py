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


def check_non_degeneration():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point3D([0., 0., 0.]))
    builder.set_point(1, geode.Point3D([3., 3., -0.5]))
    builder.set_point(2, geode.Point3D([-0.5, 4., -1.]))
    builder.set_point(3, geode.Point3D([1., 3., 3.]))
    builder.set_point(4, geode.Point3D([1., 2., -3.]))

    builder.create_tetrahedron([0, 1, 2, 3])
    builder.create_tetrahedron([0, 1, 4, 2])

    degeneration_inspector = inspector.SolidMeshDegeneration3D(solid)
    if degeneration_inspector.is_mesh_degenerated():
        raise ValueError(
            "[Test] Solid is shown degenerated whereas it is not.")
    if not degeneration_inspector.nb_degenerated_edges() == 0:
        raise ValueError(
            "[Test] Solid has more degenerated edges than it should.")
    if degeneration_inspector.degenerated_edges():
        raise ValueError(
            "[Test] Solid has degenerated edges when it should have none.")


def check_degeneration_by_colocalisation():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point3D([0., 0., 0.]))
    builder.set_point(1, geode.Point3D([3., 3., 0.]))
    builder.set_point(2, geode.Point3D([-0.5, 4., -1.]))
    builder.set_point(3, geode.Point3D([1., 3., 3.]))
    builder.set_point(4, geode.Point3D([3., 3., -geode.global_epsilon / 2]))

    builder.create_tetrahedron([0, 1, 3, 2])
    builder.create_tetrahedron([0, 1, 4, 2])
    solid.enable_edges()

    degeneration_inspector = inspector.SolidMeshDegeneration3D(solid)
    if not degeneration_inspector.is_mesh_degenerated():
        raise ValueError(
            "[Test] Solid is shown not degenerated whereas it is.")
    if not degeneration_inspector.nb_degenerated_edges() == 1:
        raise ValueError("[Test] Solid has wrong number of degenerated edges.")
    if not degeneration_inspector.degenerated_edges()[0] == solid.edges().edge_from_vertices([1, 4]):
        raise ValueError("[Test] Solid has wrong degenerated edges.")


def check_degeneration_by_point_multiple_presence():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point3D([0., 0., 0.]))
    builder.set_point(1, geode.Point3D([3., 3., -0.5]))
    builder.set_point(2, geode.Point3D([-0.5, 4., -1.]))
    builder.set_point(3, geode.Point3D([1., 3., 3.]))

    builder.create_tetrahedron([0, 1, 3, 2])
    builder.create_tetrahedron([0, 1, 1, 2])
    solid.enable_edges()

    degeneration_inspector = inspector.SolidMeshDegeneration3D(solid)
    if not degeneration_inspector.is_mesh_degenerated():
        raise ValueError(
            "[Test] Solid is not shown degenerated whereas it is.")
    if not degeneration_inspector.nb_degenerated_edges() == 1:
        raise ValueError(
            "[Test] Solid has the wrong number of degenerated edges.")
    if not degeneration_inspector.degenerated_edges()[0] == solid.edges().edge_from_vertices([1, 1]):
        raise ValueError("[Test] Solid shows the wrong degenerated edges.")


if __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    check_non_degeneration()
    check_degeneration_by_colocalisation()
    check_degeneration_by_point_multiple_presence()
