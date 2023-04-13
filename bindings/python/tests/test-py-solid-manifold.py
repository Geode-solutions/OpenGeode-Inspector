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
    for path in [x.strip() for x in os.environ['PATH'].split('') if x]:
        os.add_dll_directory(path)

import opengeode as geode
import opengeode_inspector_py_inspector as inspector


def check_vertex_manifold():
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

    manifold_inspector = geode.SolidMeshInspector3D(solid)
    if not manifold_inspector.mesh_vertices_are_manifold():
        raise ValueError("[Test] Solid is shown non-manifold whereas it is.")
    if manifold_inspector.nb_non_manifold_vertices() != 0:
        raise ValueError(
            "[Test] Solid has more non manifold vertices than it should.")
    if manifold_inspector.non_manifold_vertices():
        raise ValueError(
            "[Test] Solid vertices are shown non manifold whereas they are.")


def check_vertex_non_manifold():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(*solid)
    builder.create_vertices(7)
    builder.set_point(0, geode.Point3D([0., 0., 2.]))
    builder.set_point(1, geode.Point3D([3., .5, 0.]))
    builder.set_point(2, geode.Point3D([.5, 3., .5]))
    builder.set_point(3, geode.Point3D([2., 1.5, 3.]))
    builder.set_point(4, geode.Point3D([3.5, 2.5, -.5]))
    builder.set_point(5, geode.Point3D([3., .5, 0.]))
    builder.set_point(6, geode.Point3D([.5, 3., .5]))
    builder.create_tetrahedron([0, 1, 2, 3])
    builder.create_tetrahedron([5, 4, 6, 3])

    manifold_inspector = geode.SolidMeshInspector3D(solid)
    if manifold_inspector.mesh_vertices_are_manifold():
        raise ValueError("[Test] Solid is shown manifold whereas it is not.")
    if manifold_inspector.nb_non_manifold_vertices() != 1:
        raise ValueError(
            "[Test] Solid has wrong number of non manifold vertices.")
    if manifold_inspector.non_manifold_vertices()[0] != 3:
        raise ValueError("[Test] Solid shows wrong non manifold vertex id.")


def check_edge_manifold():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(*solid)
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

    manifold_inspector = geode.SolidMeshInspector3D(solid)
    if not manifold_inspector.mesh_edges_are_manifold():
        raise ValueError("[Test] Solid is shown non-manifold whereas it is.")
    if manifold_inspector.nb_non_manifold_edges() != 0:
        raise ValueError(
            "[Test] Solid has more non manifold edges than it should.")
    if manifold_inspector.non_manifold_edges():
        raise ValueError(
            "[Test] Solid edges are shown non manifold whereas they are.")


def check_edge_non_manifold():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(6)
    builder.set_point(0, geode.Point3D([0., 0., 2.]))
    builder.set_point(1, geode.Point3D([3., .5, 0.]))
    builder.set_point(2, geode.Point3D([.5, 3., .5]))
    builder.set_point(3, geode.Point3D([2., 1.5, 3.]))
    builder.set_point(4, geode.Point3D([3.5, 2.5, -.5]))
    builder.set_point(5, geode.Point3D([3., .5, 0.]))
    builder.create_tetrahedron([0, 1, 2, 3])
    builder.create_tetrahedron([5, 4, 2, 3])

    manifold_inspector = geode.SolidMeshInspector3D(solid)
    if manifold_inspector.mesh_edges_are_manifold():
        raise ValueError("[Test] Solid is shown manifold whereas it is not.")
    if manifold_inspector.nb_non_manifold_edges() != 1:
        raise ValueError(
            "[Test] Solid has wrong number of non manifold edges.")
    non_manifold_e = manifold_inspector.non_manifold_edges()
    if non_manifold_e[0][0] != 2 or non_manifold_e[0][1] != 3:
        raise ValueError("[Test] Solid shows wrong non manifold edge id.")


def check_facet_manifold():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(*solid)
    builder.create_vertices(5)
    builder.set_point(0, geode.Point3D([0., 0., 2.]))
    builder.set_point(1, geode.Point3D([3., .5, 0.]))
    builder.set_point(2, geode.Point3D([.5, 3., .5]))
    builder.set_point(3, geode.Point3D([2., 1.5, 3.]))
    builder.set_point(4, geode.Point3D([3.5, 2.5, -.5]))
    builder.create_tetrahedron([0, 1, 2, 3])
    builder.create_tetrahedron([1, 4, 2, 3])

    manifold_inspector = geode.SolidMeshInspector3D(solid)
    if manifold_inspector.mesh_facets_are_manifold():
        raise ValueError("[Test] Solid is shown non-manifold whereas it is.")
    if manifold_inspector.nb_non_manifold_facets() != 0:
        raise ValueError(
            "[Test] Solid has more non manifold facets than it should.")
    if manifold_inspector.non_manifold_facets():
        raise ValueError(
            "[Test] Solid facets are shown non manifold whereas they are.")


def check_facet_non_manifold():
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create(*solid)
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

    manifold_inspector = geode.SolidMeshInspector3D(solid)
    if manifold_inspector.mesh_facets_are_manifold():
        raise ValueError("[Test] Solid is shown manifold whereas it is not.")
    if manifold_inspector.nb_non_manifold_facets() != 1:
        raise ValueError(
            "[Test] Solid has wrong number of non manifold facets.")
    non_manifold_f = manifold_inspector.non_manifold_facets()
    if non_manifold_f[0][0] != 1 or non_manifold_f[0][1] != 2 or non_manifold_f[0][2] != 3:
        raise ValueError("[Test] Solid shows wrong non manifold facet id.")


if not __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    check_vertex_manifold()
    check_vertex_non_manifold()
    check_edge_manifold()
    check_edge_non_manifold()
    check_facet_manifold()
    check_facet_non_manifold()
