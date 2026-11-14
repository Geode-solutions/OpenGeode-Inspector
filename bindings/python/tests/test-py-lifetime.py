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

"""An inspector keeps the mesh or the model it inspects alive: it uses it when
it is called, and some of them modify it (the degeneration inspectors enable
the edges of the mesh and disable them again when they are destroyed)."""

import gc
import os
import weakref

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


def point(dimension, x, y):
    if dimension == 2:
        return opengeode.Point2D([x, y])
    return opengeode.Point3D([x, y, 0.0])


def create_surface(dimension):
    surface = getattr(opengeode, f"TriangulatedSurface{dimension}D").create()
    builder = getattr(opengeode, f"TriangulatedSurfaceBuilder{dimension}D").create(
        surface
    )
    builder.create_vertices(4)
    for vertex in range(4):
        builder.set_point(vertex, point(dimension, vertex % 2, vertex // 2))
    builder.create_triangle([0, 1, 2])
    builder.create_triangle([1, 3, 2])
    builder.compute_polygon_adjacencies()
    return surface


def create_solid():
    solid = opengeode.TetrahedralSolid3D.create()
    builder = opengeode.TetrahedralSolidBuilder3D.create(solid)
    builder.create_vertices(4)
    for vertex, coordinates in enumerate(
        [[0.0, 0.0, 0.0], [1.0, 0.0, 0.0], [0.0, 1.0, 0.0], [0.0, 0.0, 1.0]]
    ):
        builder.set_point(vertex, opengeode.Point3D(coordinates))
    builder.create_tetrahedron([0, 1, 2, 3])
    builder.compute_polyhedron_adjacencies()
    return solid


def create_edged_curve(dimension):
    curve = getattr(opengeode, f"EdgedCurve{dimension}D").create()
    builder = getattr(opengeode, f"EdgedCurveBuilder{dimension}D").create(curve)
    builder.create_vertices(3)
    for vertex in range(3):
        builder.set_point(vertex, point(dimension, vertex, 0.0))
    builder.create_edge_with_vertices(0, 1)
    return curve


def create_point_set(dimension):
    point_set = getattr(opengeode, f"PointSet{dimension}D").create()
    builder = getattr(opengeode, f"PointSetBuilder{dimension}D").create(point_set)
    builder.create_vertices(2)
    for vertex in range(2):
        builder.set_point(vertex, point(dimension, vertex, 0.0))
    return point_set


def mesh_inspectors():
    """(inspector class name, mesh creator) of every inspector built on a mesh"""
    for dimension in [2, 3]:
        for prefix in [
            "SurfaceMeshAdjacency",
            "SurfaceMeshColocation",
            "SurfaceMeshDegeneration",
            "SurfaceMeshEdgeManifold",
            "SurfaceMeshVertexManifold",
            "SurfaceMeshIntersections",
            "SurfaceMeshNegativeElements",
            "SurfaceMeshIsolation",
            "SurfaceMeshInspector",
        ]:
            yield f"{prefix}{dimension}D", lambda d=dimension: create_surface(d)
        for prefix in [
            "EdgedCurveColocation",
            "EdgedCurveDegeneration",
            "EdgedCurveIsolation",
            "EdgedCurveInspector",
        ]:
            yield f"{prefix}{dimension}D", lambda d=dimension: create_edged_curve(d)
        for prefix in ["PointSetColocation", "PointSetInspector"]:
            yield f"{prefix}{dimension}D", lambda d=dimension: create_point_set(d)
    for prefix in [
        "SolidMeshAdjacency",
        "SolidMeshColocation",
        "SolidMeshDegeneration",
        "SolidMeshEdgeManifold",
        "SolidMeshVertexManifold",
        "SolidMeshFacetManifold",
        "SolidMeshNegativeElements",
        "SolidMeshIsolation",
        "SolidMeshInspector",
    ]:
        yield f"{prefix}3D", create_solid


def model_inspectors():
    """(inspector class name, model name) of every inspector built on a model"""
    for model in ["BRep", "Section"]:
        for suffix in [
            "UniqueVerticesColocation",
            "ComponentMeshesColocation",
            "MeshesIntersections",
            "TopologyInspector",
            "MeshesInspector",
            "Inspector",
            "ComponentMeshesAdjacency",
            "ComponentMeshesDegeneration",
            "ComponentMeshesManifold",
            "ComponentMeshesNegativeElements",
            "CornersTopology",
            "LinesTopology",
            "SurfacesTopology",
        ]:
            yield f"{model}{suffix}", model
    for name in ["BRepBlocksTopology"]:
        yield name, "BRep"


def check_keeps_alive(name, create, extra=()):
    owner = getattr(inspection, name)
    mesh = create()
    reference = weakref.ref(mesh)
    instance = owner(mesh, *extra)
    del mesh
    gc.collect()
    check(reference() is not None, f"{name} does not keep its mesh alive")
    del instance
    gc.collect()
    check(reference() is None, f"{name} still holds its mesh when destroyed")


def test_mesh_inspectors():
    count = 0
    for name, create in mesh_inspectors():
        check_keeps_alive(name, create)
        count += 1
    check(count == 2 * (9 + 4 + 2) + 9, f"{count} mesh inspectors checked")


def load_model(model):
    if model == "BRep":
        return opengeode.load(
            opengeode.BRep, os.path.join(data_dir(), "model_A1_valid.og_brep")
        )
    return opengeode.load(
        opengeode.Section, os.path.join(data_dir(), "vertical_lines.og_sctn")
    )


def test_model_inspectors():
    count = 0
    for name, model in model_inspectors():
        if not hasattr(inspection, name):
            continue
        check_keeps_alive(name, lambda m=model: load_model(m))
        count += 1
    check(count == 2 * 13 + 1, f"{count} model inspectors checked")


def test_surface_curve_intersections():
    for dimension in [2, 3]:
        surface = create_surface(dimension)
        curve = create_edged_curve(dimension)
        surface_reference = weakref.ref(surface)
        curve_reference = weakref.ref(curve)
        instance = getattr(inspection, f"SurfaceCurveIntersections{dimension}D")(
            surface, curve
        )
        del surface, curve
        gc.collect()
        check(surface_reference() is not None, "the surface must be kept alive")
        check(curve_reference() is not None, "the curve must be kept alive")
        # The edge between the vertices 0 and 1 is on an edge of the surface
        instance.meshes_have_intersections()
        del instance
        gc.collect()
        check(surface_reference() is None, "the surface is released")
        check(curve_reference() is None, "the curve is released")


def test_degeneration_edges_after_mesh_deletion():
    # The destructor of the inspector disables the edges it enabled on the
    # mesh: the mesh must still be there
    for dimension in [2, 3]:
        surface = create_surface(dimension)
        degeneration = getattr(inspection, f"SurfaceMeshDegeneration{dimension}D")(
            surface
        )
        check(degeneration.small_edges(10.0).nb_issues() == 5, "five small edges")
        del surface
        gc.collect()
        check(degeneration.small_edges(10.0).nb_issues() == 5, "five small edges")
        del degeneration
        gc.collect()
    solid = create_solid()
    degeneration = inspection.SolidMeshDegeneration3D(solid)
    check(degeneration.small_edges(10.0).nb_issues() == 6, "six small edges")
    del solid
    gc.collect()
    del degeneration
    gc.collect()


def test_brep_degeneration_lifetime():
    # The inspector enables the edges of the Block meshes it finds small
    # edges in, and disables them when destroyed: the BRep must outlive it
    brep = opengeode.load(
        opengeode.BRep, os.path.join(data_dir(), "cube_segmented.og_brep")
    )
    reference = weakref.ref(brep)
    degeneration = inspection.BRepComponentMeshesDegeneration(brep)
    result = degeneration.inspect_polyhedra_degeneration(1e9)
    check(result.degenerated_edges.nb_issues() > 0, "small edges in the Blocks")
    del brep
    gc.collect()
    check(reference() is not None, "the inspector keeps the BRep alive")
    check(
        degeneration.inspect_polyhedra_degeneration(1e9).nb_issues()
        == result.nb_issues(),
        "the inspector still works",
    )
    del degeneration
    gc.collect()
    check(reference() is None, "the BRep is released with the inspector")
    check(result.nb_issues() > 0, "the result does not depend on the BRep")


if __name__ == "__main__":
    test_mesh_inspectors()
    test_model_inspectors()
    test_surface_curve_intersections()
    test_degeneration_edges_after_mesh_deletion()
    test_brep_degeneration_lifetime()
