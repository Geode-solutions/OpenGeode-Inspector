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

"""The inspection of the negative elements (polygons with a negative area,
polyhedra with a negative volume) of the meshes and of the models, alone and in
the complete inspections."""

import os

import geode_test_env

import opengeode
import opengeode_inspector_py_inspection as inspection


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def data_dir():
    return os.path.abspath(
        os.path.join(os.path.dirname(__file__), "../../../tests/data")
    )


def create_surface_2d(clockwise_second_triangle):
    surface = opengeode.TriangulatedSurface2D.create()
    builder = opengeode.TriangulatedSurfaceBuilder2D.create(surface)
    builder.create_vertices(4)
    builder.set_point(0, opengeode.Point2D([0.0, 2.0]))
    builder.set_point(1, opengeode.Point2D([2.0, 0.0]))
    builder.set_point(2, opengeode.Point2D([1.0, 4.0]))
    builder.set_point(3, opengeode.Point2D([3.0, 3.0]))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([2, 3, 1] if clockwise_second_triangle else [1, 3, 2])
    return surface


def test_surface_2d():
    surface = create_surface_2d(True)
    negative = inspection.SurfaceMeshNegativeElements2D(surface)
    check(negative.mesh_has_negative_elements(), "a negative polygon expected")
    issues = negative.negative_polygons()
    check(isinstance(issues, inspection.InspectionIssuesIndex), "wrong issues type")
    check(issues.issues() == [1], f"negative polygons {issues.issues()}")
    check("negative polygon 1" in issues.string(), issues.string())
    surface = create_surface_2d(False)
    negative = inspection.SurfaceMeshNegativeElements2D(surface)
    check(not negative.mesh_has_negative_elements(), "no negative polygon expected")
    check(negative.negative_polygons().nb_issues() == 0, "no negative polygon")


def test_surface_3d_has_no_orientation():
    surface = opengeode.TriangulatedSurface3D.create()
    builder = opengeode.TriangulatedSurfaceBuilder3D.create(surface)
    builder.create_vertices(3)
    builder.set_point(0, opengeode.Point3D([0.0, 0.0, 0.0]))
    builder.set_point(1, opengeode.Point3D([0.0, 1.0, 0.0]))
    builder.set_point(2, opengeode.Point3D([1.0, 0.0, 0.0]))
    builder.create_triangle([0, 1, 2])
    negative = inspection.SurfaceMeshNegativeElements3D(surface)
    check(not negative.mesh_has_negative_elements(), "no negative polygon in 3D")
    check(negative.negative_polygons().nb_issues() == 0, "no negative polygon in 3D")


def create_solid(first_vertices):
    solid = opengeode.TetrahedralSolid3D.create()
    builder = opengeode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(4)
    for vertex, coordinates in enumerate(
        [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]
    ):
        builder.set_point(vertex, opengeode.Point3D(coordinates))
    builder.create_tetrahedron(first_vertices)
    return solid


def test_solid():
    positive = create_solid([0, 1, 2, 3])
    reversed_solid = create_solid([0, 2, 1, 3])
    results = []
    for solid in [positive, reversed_solid]:
        negative = inspection.SolidMeshNegativeElements3D(solid)
        results.append(negative.mesh_has_negative_elements())
        issues = negative.negative_polyhedra()
        check(
            issues.nb_issues() == (1 if results[-1] else 0),
            "inconsistent negative polyhedra",
        )
    check(sorted(results) == [False, True], f"one negative solid expected {results}")


def test_complete_inspections():
    surface = create_surface_2d(True)
    result = inspection.inspect_surface2D(surface)
    check(result.negative_polygons.issues() == [1], "negative polygon in the result")
    inspector = inspection.SurfaceMeshInspector2D(surface)
    check(
        inspector.inspect_surface().negative_polygons.nb_issues() == 1,
        "negative polygon of the inspector",
    )
    check(
        inspector.mesh_has_negative_elements(),
        "the inspector has the methods of the negative elements inspector",
    )
    check("negative_polygons=1" in repr(result), repr(result))
    solid = create_solid([0, 1, 2, 3])
    solid_result = inspection.inspect_solid3D(solid)
    check(
        solid_result.negative_polyhedra.nb_issues()
        == inspection.SolidMeshNegativeElements3D(solid)
        .negative_polyhedra()
        .nb_issues(),
        "negative polyhedra of the solid result",
    )


def test_models():
    brep = opengeode.load(
        opengeode.BRep, os.path.join(data_dir(), "model_A1_valid.og_brep")
    )
    brep_negative = inspection.BRepComponentMeshesNegativeElements(brep)
    result = brep_negative.inspect_negative_elements()
    check(
        isinstance(result, inspection.BRepMeshesNegativeElementsInspectionResult),
        "wrong BRep result type",
    )
    check(
        isinstance(
            result.negative_polyhedra, inspection.InspectionIssuesMapIndex
        ),
        "wrong BRep negative polyhedra type",
    )
    meshes = inspection.BRepMeshesInspector(brep).inspect_brep_meshes()
    check(
        meshes.meshes_negative_elements.nb_issues() == result.nb_issues(),
        "the meshes inspection includes the negative elements",
    )
    section = opengeode.load(
        opengeode.Section, os.path.join(data_dir(), "vertical_lines.og_sctn")
    )
    section_result = inspection.SectionComponentMeshesNegativeElements(
        section
    ).inspect_negative_elements()
    check(
        isinstance(
            section_result.negative_polygons, inspection.InspectionIssuesMapIndex
        ),
        "wrong Section negative polygons type",
    )
    check(
        inspection.SectionMeshesInspector(section)
        .inspect_section_meshes()
        .meshes_negative_elements.nb_issues()
        == section_result.nb_issues(),
        "the Section meshes inspection includes the negative elements",
    )


def test_model_with_negative_polygon():
    section = opengeode.Section()
    builder = opengeode.SectionBuilder(section)
    surface_id = builder.add_surface()
    mesh_builder = builder.surface_mesh_builder(section.surface(surface_id))
    mesh_builder.create_vertices(3)
    mesh_builder.set_point(0, opengeode.Point2D([0.0, 0.0]))
    mesh_builder.set_point(1, opengeode.Point2D([0.0, 1.0]))
    mesh_builder.set_point(2, opengeode.Point2D([1.0, 0.0]))
    mesh_builder.create_polygon([0, 1, 2])
    result = inspection.SectionComponentMeshesNegativeElements(
        section
    ).inspect_negative_elements()
    check(result.nb_issues() == 1, f"one negative polygon expected {result}")
    issues = result.negative_polygons.issues_map()
    check(list(issues) == [surface_id], "the issue is on the Surface")
    check(issues[surface_id].issues() == [0], "polygon 0 is negative")


if __name__ == "__main__":
    test_surface_2d()
    test_surface_3d_has_no_orientation()
    test_solid()
    test_complete_inspections()
    test_models()
    test_model_with_negative_polygon()
