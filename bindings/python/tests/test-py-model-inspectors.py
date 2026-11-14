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

"""The inspectors of a model: the inspectors of each type of component, the
degeneration with a threshold and the self intersections, and their
consistency with the complete inspections."""

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


def load_brep(name="model_A1.og_brep"):
    return opengeode.load(opengeode.BRep, os.path.join(data_dir(), name))


def load_section():
    return opengeode.load(
        opengeode.Section, os.path.join(data_dir(), "vertical_lines.og_sctn")
    )


def test_brep_topology_by_component():
    brep = load_brep()
    topology = inspection.BRepTopologyInspector(brep).inspect_brep_topology()
    check(topology.nb_issues() > 0, "model_A1 has topology issues")
    for name, class_name, method in [
        ("corners", "BRepCornersTopology", "inspect_corners_topology"),
        ("lines", "BRepLinesTopology", "inspect_lines_topology"),
        ("surfaces", "BRepSurfacesTopology", "inspect_surfaces_topology"),
        ("blocks", "BRepBlocksTopology", "inspect_blocks"),
    ]:
        component = getattr(inspection, class_name)(brep)
        result = getattr(component, method)()
        expected = getattr(topology, name)
        check(type(result) is type(expected), f"{class_name}: {type(result)}")
        check(
            result.nb_issues() == expected.nb_issues(),
            f"{class_name}: {result.nb_issues()} != {expected.nb_issues()}",
        )
    # The topology inspector has the methods of the inspectors of the components
    inspector = inspection.BRepTopologyInspector(brep)
    for name in [
        "inspect_corners_topology",
        "inspect_lines_topology",
        "inspect_surfaces_topology",
        "inspect_blocks",
    ]:
        check(hasattr(inspector, name), f"BRepTopologyInspector.{name}")
    check(
        inspector.inspect_blocks().nb_issues() == topology.blocks.nb_issues(),
        "blocks of the topology inspector",
    )
    check(
        not inspector.brep_topology_is_valid(), "model_A1 topology is invalid"
    )
    check(
        not inspector.brep_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex()
        or topology.unique_vertices_nonbijectively_linked_to_cmv.nb_issues() == 0,
        "bijective links are consistent with the result",
    )


def test_section_topology_by_component():
    section = load_section()
    topology = inspection.SectionTopologyInspector(
        section
    ).inspect_section_topology()
    for name, class_name, method in [
        ("corners", "SectionCornersTopology", "inspect_corners_topology"),
        ("lines", "SectionLinesTopology", "inspect_lines_topology"),
        ("surfaces", "SectionSurfacesTopology", "inspect_surfaces"),
    ]:
        result = getattr(getattr(inspection, class_name)(section), method)()
        expected = getattr(topology, name)
        check(type(result) is type(expected), f"{class_name}: {type(result)}")
        check(
            result.nb_issues() == expected.nb_issues(),
            f"{class_name}: {result.nb_issues()} != {expected.nb_issues()}",
        )


def test_empty_models():
    for model, inspector_name in [
        (opengeode.BRep(), "BRepTopologyInspector"),
        (opengeode.Section(), "SectionTopologyInspector"),
    ]:
        inspector = getattr(inspection, inspector_name)(model)
        validity = getattr(
            inspector,
            "brep_topology_is_valid"
            if "BRep" in inspector_name
            else "section_topology_is_valid",
        )
        check(not validity(), "an empty model has no unique vertex: invalid")


def test_brep_degeneration_thresholds():
    # cube_segmented has Blocks with polyhedra, model_A1 has none
    brep = load_brep("cube_segmented.og_brep")
    degeneration = inspection.BRepComponentMeshesDegeneration(brep)
    everything = degeneration.inspect_elements_degeneration()
    check(
        isinstance(
            everything, inspection.BRepMeshesDegenerationInspectionResult
        ),
        "wrong result type",
    )
    huge = 1e9
    edges = degeneration.inspect_edges_degeneration(huge)
    check(edges.degenerated_edges.nb_issues() > 0, "every edge is smaller than huge")
    check(edges.degenerated_polygons.nb_issues() == 0, "edges only")
    check(edges.degenerated_polyhedra.nb_issues() == 0, "edges only")
    polygons = degeneration.inspect_polygons_degeneration(huge)
    check(polygons.degenerated_polygons.nb_issues() > 0, "every polygon is small")
    check(polygons.degenerated_edges.nb_issues() == 0, "polygons only")
    polyhedra = degeneration.inspect_polyhedra_degeneration(huge)
    check(polyhedra.degenerated_polyhedra.nb_issues() > 0, "every polyhedron")
    # No polygon has a negative height
    check(
        degeneration.inspect_polygons_degeneration(-1.0).nb_issues() == 0, "negative"
    )
    # The thresholds of the edges and of the elements are consistent with the
    # complete inspection: nothing is smaller than nothing
    check(degeneration.inspect_edges_degeneration(0.0).nb_issues() == 0, "zero")
    # The complete inspection of the meshes gives the same degenerations
    meshes = inspection.BRepMeshesInspector(brep)
    check(
        meshes.inspect_elements_degeneration().nb_issues() == everything.nb_issues(),
        "same degenerations in the meshes inspector",
    )


def test_surfaces_self_intersections():
    brep = load_brep()
    intersections = inspection.BRepMeshesIntersections(brep)
    surfaces_only = intersections.inspect_surfaces_self_intersections()
    everything = intersections.inspect_intersections()
    check(
        isinstance(
            surfaces_only, inspection.ElementsIntersectionsInspectionResult
        ),
        "wrong result type",
    )
    check(
        surfaces_only.nb_issues() <= everything.nb_issues(),
        "self intersections are some of the intersections",
    )
    for pair in surfaces_only.elements_intersections.issues():
        first, second = pair
        check(
            first.component_id.id == second.component_id.id,
            "a self intersection is inside one Surface",
        )
    section = load_section()
    check(
        inspection.SectionMeshesIntersections(section)
        .inspect_surfaces_self_intersections()
        .nb_issues()
        == 0,
        "no self intersection in vertical_lines",
    )


def test_meshes_inspector_has_the_inspectors_methods():
    brep = load_brep()
    inspector = inspection.BRepMeshesInspector(brep)
    result = inspector.inspect_brep_meshes()
    check(
        inspector.inspect_negative_elements().nb_issues()
        == result.meshes_negative_elements.nb_issues(),
        "negative elements of the meshes inspector",
    )
    check(
        inspector.inspect_brep_manifold().nb_issues()
        == result.meshes_non_manifolds.nb_issues(),
        "manifold of the meshes inspector",
    )
    check(
        inspector.inspect_unique_vertices().nb_issues()
        == result.unique_vertices_colocation.nb_issues(),
        "unique vertices of the meshes inspector",
    )
    section = load_section()
    section_inspector = inspection.SectionInspector(section)
    section_result = section_inspector.inspect_section()
    check(
        section_result.nb_issues()
        == section_result.meshes.nb_issues() + section_result.topology.nb_issues(),
        "the section result sums its members",
    )
    check(
        section_inspector.inspect_negative_elements().nb_issues()
        == section_result.meshes.meshes_negative_elements.nb_issues(),
        "negative elements of the section inspector",
    )


def test_unique_vertices_colocation():
    brep = load_brep()
    colocation = inspection.BRepUniqueVerticesColocation(brep)
    result = colocation.inspect_unique_vertices()
    check(
        colocation.model_has_colocated_unique_vertices()
        == (result.colocated_unique_vertices_groups.nb_issues() != 0),
        "colocated unique vertices",
    )
    check(
        colocation.model_has_unique_vertices_linked_to_different_points()
        == (result.unique_vertices_linked_to_different_points.nb_issues() != 0),
        "unique vertices linked to different points",
    )


THREAD_CAVEAT = "do not inspect or use the same mesh or model from two threads at once"


def check_enabled_edges(meshes, enabled, message):
    for mesh in meshes:
        check(mesh.are_edges_enabled() == enabled, message)


def degenerated_section():
    """A Section with one Surface whose triangle has a degenerated edge"""
    section = opengeode.Section()
    builder = opengeode.SectionBuilder(section)
    surface = section.surface(builder.add_surface())
    mesh_builder = builder.surface_mesh_builder(surface)
    for point in ([0, 0], [1, 0], [1, 0]):
        mesh_builder.create_point(opengeode.Point2D(point))
    mesh_builder.create_polygon([0, 1, 2])
    return section


def test_degeneration_enables_edges():
    """The degeneration inspections of a model enable the edges of the Surface
    and Block meshes where they find small edges, until the inspector is
    destroyed, as documented with the thread caveat"""
    for model, class_name, kinds, inspect, claim in [
        (
            load_brep("cube_segmented.og_brep"),
            "BRepComponentMeshesDegeneration",
            ["surfaces", "blocks"],
            lambda inspector: inspector.inspect_edges_degeneration(1e9),
            "The edges of the Surface and Block meshes are enabled",
        ),
        (
            degenerated_section(),
            "SectionComponentMeshesDegeneration",
            ["surfaces"],
            lambda inspector: inspector.inspect_elements_degeneration(),
            "The edges of the Surface meshes are enabled",
        ),
    ]:
        cls = getattr(inspection, class_name)
        meshes = [
            component.mesh()
            for kind in kinds
            for component in getattr(model, kind)()
        ]
        check_enabled_edges(meshes, False, f"{class_name}: no edges before")
        inspector = cls(model)
        check(inspect(inspector).degenerated_edges.nb_issues() > 0, f"{class_name}: small edges")
        check_enabled_edges(meshes, True, f"{class_name}: edges enabled by the inspection")
        del inspector
        check_enabled_edges(meshes, False, f"{class_name}: edges disabled with the inspector")
        check(claim in cls.__doc__, f"{class_name} documents the enabled edges")
        check(THREAD_CAVEAT in cls.__doc__, f"{class_name} thread caveat")
    for class_name in [
        "SurfaceMeshDegeneration2D",
        "SurfaceMeshDegeneration3D",
        "SolidMeshDegeneration3D",
    ]:
        check(
            THREAD_CAVEAT in getattr(inspection, class_name).__doc__,
            f"{class_name} thread caveat",
        )


if __name__ == "__main__":
    test_brep_topology_by_component()
    test_section_topology_by_component()
    test_empty_models()
    test_brep_degeneration_thresholds()
    test_surfaces_self_intersections()
    test_meshes_inspector_has_the_inspectors_methods()
    test_unique_vertices_colocation()
    test_degeneration_enables_edges()
