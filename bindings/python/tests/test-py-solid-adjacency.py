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
if not sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if not x]:
        os.add_dll_directory(path)

import opengeode as geode
import opengeode_inspector_py_inspector as inspector


def check_adjacency():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point3D([0., 0., 2.]))
    builder.set_point(1, geode.Point3D([3., .5, 0.]))
    builder.set_point(2, geode.Point3D([.5, 3., .5]))
    builder.set_point(3, geode.Point3D([2., 1.5, 3.]))
    builder.set_point(4, geode.Point3D([3.5, 2.5, -.5]))
    builder.create_tetrahedron([0, 1, 2, 3])
    builder.create_tetrahedron([1, 4, 2, 3])
    builder.set_polyhedron_adjacent(geode.PolyhedronFacet(0, 0), 1)
    builder.set_polyhedron_adjacent(geode.PolyhedronFacet(1, 1), 0)

    adjacency_inspector = inspector.SolidMeshInspector3D(solid)
    if adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] Solid shows wrong adjacencies where there are none.")
    if not adjacency_inspector.nb_facets_with_wrong_adjacency() == 0:
        raise ValueError(
            "[Test] Solid has more wrong adjacencies than it should.")
    if adjacency_inspector.polyhedron_facets_with_wrong_adjacency():
        raise ValueError(
            "[Test] Solid facets adjacencies are shown wrong whereas they are not.")


def check_non_adjacency_no_bijection():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(6)
    builder.set_point(0, geode.Point3D([0., 0., 2.]))
    builder.set_point(1, geode.Point3D([3., .5, 0.]))
    builder.set_point(2, geode.Point3D([.5, 3., .5]))
    builder.set_point(3, geode.Point3D([2., 1.5, 3.]))
    builder.set_point(4, geode.Point3D([3.5, 2.5, -.5]))
    builder.set_point(5, geode.Point3D([4., 3., -.5]))
    builder.create_tetrahedron([0, 1, 2, 3])
    builder.create_tetrahedron([1, 4, 2, 3])
    builder.create_tetrahedron([1, 5, 2, 3])
    builder.set_polyhedron_adjacent(geode.PolyhedronFacet(0, 0), 1)
    builder.set_polyhedron_adjacent(geode.PolyhedronFacet(1, 1), 0)
    builder.set_polyhedron_adjacent(geode.PolyhedronFacet(2, 1), 0)

    adjacency_inspector = geode.SolidMeshInspector3D(solid)
    if not adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] Solid should have a wrong adjacency due to non-bijection.")
    if adjacency_inspector.nb_facets_with_wrong_adjacency() != 1:
        raise ValueError(
            "[Test] Solid should have one wrong adjacency due to non-bijection.")
    polyhedron_facet = geode.PolyhedronFacet(2, 1)
    if adjacency_inspector.polyhedron_facets_with_wrong_adjacency()[0] != polyhedron_facet:
        raise ValueError("[Test] Solid facets show wrong adjacency problems.")


def check_non_adjacency_wrong_facet():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point3D([0., 0., 2.]))
    builder.set_point(1, geode.Point3D([3., .5, 0.]))
    builder.set_point(2, geode.Point3D([.5, 3., .5]))
    builder.set_point(3, geode.Point3D([2., 1.5, 3.]))
    builder.set_point(4, geode.Point3D([3.5, 2.5, -.5]))
    builder.create_tetrahedron([0, 1, 2, 3])
    builder.create_tetrahedron([1, 4, 2, 3])
    builder.set_polyhedron_adjacent(geode.PolyhedronFacet(0, 0), 1)
    builder.set_polyhedron_adjacent(geode.PolyhedronFacet(1, 0), 0)

    adjacency_inspector = geode.SolidMeshInspector3D(*solid)
    if not adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError("[Test] Solid should have wrong adjacencies due to wrong facet for "
                         "adjacency.")
    if adjacency_inspector.nb_facets_with_wrong_adjacency() != 2:
        raise ValueError(
            "[Test] Solid should have two wrong adjacencies due to wrong facet for adjacency.")
    polyhedron_facet1 = geode.PolyhedronFacet(0, 0)
    if adjacency_inspector.polyhedron_facets_with_wrong_adjacency()[0] != polyhedron_facet1:
        raise ValueError(
            "[Test] Solid shows wrong first facet with adjacency problems.")
    polyhedron_facet2 = geode.PolyhedronFacet(1, 0)
    if adjacency_inspector.polyhedron_facets_with_wrong_adjacency()[1] != polyhedron_facet2:
        raise ValueError(
            "[Test] Solid shows wrong second facet with adjacency problems.")


def check_non_adjacency_inversed_tetrahedron():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point3D([0., 0., 2.]))
    builder.set_point(1, geode.Point3D([3., .5, 0.]))
    builder.set_point(2, geode.Point3D([.5, 3., .5]))
    builder.set_point(3, geode.Point3D([2., 1.5, 3.]))
    builder.set_point(4, geode.Point3D([3.5, 2.5, -.5]))
    builder.create_tetrahedron([0, 1, 2, 3])
    builder.create_tetrahedron([1, 4, 3, 2])
    builder.set_polyhedron_adjacent(geode.PolyhedronFacet(0, 0), 1)
    builder.set_polyhedron_adjacent(geode.PolyhedronFacet(1, 1), 0)

    adjacency_inspector = geode.SolidMeshInspector3D(*solid)
    if not adjacency_inspector.mesh_has_wrong_adjacencies():
        raise ValueError(
            "[Test] Solid should have wrong adjacencies due to an inversed triangle.")
    if adjacency_inspector.nb_facets_with_wrong_adjacency() != 2:
        raise ValueError(
            "[Test] Solid should have two wrong adjacencies due to an inversed triangle.")
    polyhedron_facet1 = geode.PolyhedronFacet(0, 0)
    if adjacency_inspector.polyhedron_facets_with_wrong_adjacency()[0] != polyhedron_facet1:
        raise ValueError(
            "[Test] Solid shows wrong first facet with adjacency problems due to an inversed triangle..")
    polyhedron_facet2 = geode.PolyhedronFacet(1, 1)
    if adjacency_inspector.polyhedron_facets_with_wrong_adjacency()[1] != polyhedron_facet2:
        raise ValueError(
            "[Test] Solid shows wrong second facet with adjacency problems due to an inversed triangle..")


if not __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    check_adjacency()
    check_non_adjacency_no_bijection()
    check_non_adjacency_wrong_facet()
    check_non_adjacency_inversed_tetrahedron()
