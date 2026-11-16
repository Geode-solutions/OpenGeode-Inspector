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

"""The inspections release the GIL: the main thread runs while an inspection is
in progress in another thread."""

import os

import geode_test_env

import opengeode
import opengeode_inspector_py_inspection as inspection
import opengeode_inspector_py_validity as validity


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def data_dir():
    return os.path.abspath(
        os.path.join(os.path.dirname(__file__), "../../../tests/data")
    )


def check_releases_gil(name, call):
    geode_test_env.assert_gil_released(call, name=name)


def test_model_inspections():
    """Each callable contains only the call under test: the model is loaded
    and the inspectors are built outside of it"""
    brep = opengeode.load(
        opengeode.BRep, os.path.join(data_dir(), "model_A1.og_brep")
    )
    check_releases_gil("inspect_brep", lambda: inspection.inspect_brep(brep))
    check_releases_gil("is_brep_valid", lambda: validity.is_brep_valid(brep))
    check_releases_gil(
        "BRepInspector.__init__", lambda: inspection.BRepInspector(brep)
    )
    inspector = inspection.BRepInspector(brep)
    check_releases_gil("BRepInspector.inspect_brep", inspector.inspect_brep)
    check_releases_gil(
        "BRepMeshesIntersections.__init__",
        lambda: inspection.BRepMeshesIntersections(brep),
    )
    intersections = inspection.BRepMeshesIntersections(brep)
    check_releases_gil(
        "BRepMeshesIntersections.inspect_intersections",
        intersections.inspect_intersections,
    )


def test_mesh_inspections():
    brep = opengeode.load(
        opengeode.BRep, os.path.join(data_dir(), "model_D.og_brep")
    )
    surfaces = sorted(
        (surface.mesh() for surface in brep.surfaces()),
        key=lambda mesh: mesh.nb_polygons(),
    )
    biggest = surfaces[-1]
    check_releases_gil("inspect_surface3D", lambda: inspection.inspect_surface3D(biggest))
    intersections = inspection.SurfaceMeshIntersections3D(biggest)
    check_releases_gil(
        "SurfaceMeshIntersections3D.intersecting_elements",
        intersections.intersecting_elements,
    )
    colocation = inspection.SurfaceMeshColocation3D(biggest)
    check_releases_gil(
        "SurfaceMeshColocation3D.colocated_points_groups",
        colocation.colocated_points_groups,
    )
    check_releases_gil(
        "is_surface_valid3D", lambda: validity.is_surface_valid3D(biggest)
    )


if __name__ == "__main__":
    test_model_inspections()
    test_mesh_inspections()
