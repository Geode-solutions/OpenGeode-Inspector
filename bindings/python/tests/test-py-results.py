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

"""The result classes: the members are readable and writable (the setter
argument is named), nb_issues sums the members, str is the report and repr
summarizes the number of issues of each member, and the lists of issues can be
built."""

import inspect

import geode_test_env

import opengeode
import opengeode_inspector_py_inspection as inspection
import opengeode_inspector_py_validity as validity


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


# Result class -> members, as defined by the C++ structures
MEMBERS = {
    "PointSetInspectionResult": ["colocated_points_groups"],
    "EdgedCurveInspectionResult": [
        "colocated_points_groups",
        "degenerated_edges",
        "isolated_vertices",
    ],
    "SurfaceInspectionResult": [
        "polygon_edges_with_wrong_adjacency",
        "colocated_points_groups",
        "degenerated_edges",
        "degenerated_polygons",
        "non_manifold_edges",
        "non_manifold_vertices",
        "intersecting_elements",
        "negative_polygons",
        "isolated_vertices",
    ],
    "SolidInspectionResult": [
        "polyhedron_facets_with_wrong_adjacency",
        "colocated_points_groups",
        "degenerated_edges",
        "degenerated_polyhedra",
        "non_manifold_vertices",
        "non_manifold_edges",
        "non_manifold_facets",
        "negative_polyhedra",
        "isolated_vertices",
    ],
    "BRepMeshesAdjacencyInspectionResult": [
        "surfaces_edges_with_wrong_adjacencies",
        "blocks_facets_with_wrong_adjacencies",
    ],
    "SectionMeshesAdjacencyInspectionResult": ["surfaces_edges_with_wrong_adjacencies"],
    "MeshesColocationInspectionResult": ["colocated_points_groups"],
    "UniqueVerticesInspectionResult": [
        "colocated_unique_vertices_groups",
        "unique_vertices_linked_to_different_points",
    ],
    "BRepMeshesDegenerationInspectionResult": [
        "degenerated_edges",
        "degenerated_polygons",
        "degenerated_polyhedra",
    ],
    "SectionMeshesDegenerationInspectionResult": [
        "degenerated_edges",
        "degenerated_polygons",
    ],
    "ElementsIntersectionsInspectionResult": ["elements_intersections"],
    "BRepMeshesManifoldInspectionResult": [
        "meshes_non_manifold_vertices",
        "meshes_non_manifold_edges",
        "meshes_non_manifold_facets",
        "brep_non_manifold_edges",
        "brep_non_manifold_facets",
    ],
    "SectionMeshesManifoldInspectionResult": [
        "meshes_non_manifold_vertices",
        "meshes_non_manifold_edges",
    ],
    "BRepMeshesNegativeElementsInspectionResult": ["negative_polyhedra"],
    "SectionMeshesNegativeElementsInspectionResult": ["negative_polygons"],
    "BRepMeshesInspectionResult": [
        "unique_vertices_colocation",
        "meshes_colocation",
        "meshes_adjacencies",
        "meshes_degenerations",
        "meshes_intersections",
        "meshes_non_manifolds",
        "meshes_negative_elements",
    ],
    "SectionMeshesInspectionResult": [
        "unique_vertices_colocation",
        "meshes_colocation",
        "meshes_adjacencies",
        "meshes_degenerations",
        "meshes_intersections",
        "meshes_non_manifolds",
        "meshes_negative_elements",
    ],
    "BRepCornersTopologyInspectionResult": [
        "corners_not_meshed",
        "corners_not_linked_to_a_unique_vertex",
        "unique_vertices_linked_to_multiple_corners",
        "unique_vertices_linked_to_multiply_embedded_corner",
        "unique_vertices_linked_to_not_internal_nor_boundary_corner",
        "unique_vertices_linked_to_not_boundary_line_corner",
    ],
    "BRepLinesTopologyInspectionResult": [
        "lines_not_meshed",
        "lines_not_embedded_nor_incident",
        "lines_multiply_embedded",
        "unique_vertices_linked_to_line_with_wrong_relationship_to_surface",
        "lines_not_linked_to_a_unique_vertex",
        "unique_vertices_linked_to_a_line_with_invalid_embeddings",
        "unique_vertices_linked_to_a_single_and_invalid_line",
        "unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner",
        "line_edges_with_wrong_component_edges_around",
    ],
    "BRepSurfacesTopologyInspectionResult": [
        "surfaces_not_meshed",
        "surfaces_multiply_embedded",
        "surfaces_not_linked_to_a_unique_vertex",
        "unique_vertices_linked_to_a_surface_with_invalid_embbedings",
        "unique_vertices_linked_to_several_and_invalid_surfaces",
        "unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border",
        "surface_polygons_with_wrong_component_facets_around",
    ],
    "BRepBlocksTopologyInspectionResult": [
        "some_blocks_not_meshed",
        "wrong_block_boundary_surface",
        "blocks_not_linked_to_a_unique_vertex",
        "blocks_with_not_closed_boundary_surfaces",
        "blocks_split_by_internal_surfaces",
        "model_boundaries_dont_form_a_closed_surface",
        "unique_vertices_part_of_two_blocks_and_no_boundary_surface",
        "unique_vertices_with_incorrect_block_cmvs_count",
        "unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks",
        "unique_vertices_linked_to_a_single_and_invalid_surface",
        "unique_vertex_linked_to_multiple_invalid_surfaces",
    ],
    "BRepTopologyInspectionResult": [
        "corners",
        "lines",
        "surfaces",
        "blocks",
        "unique_vertices_not_linked_to_any_component",
        "unique_vertices_linked_to_inexistant_cmv",
        "unique_vertices_nonbijectively_linked_to_cmv",
    ],
    "SectionCornersTopologyInspectionResult": [
        "corners_not_meshed",
        "corners_not_linked_to_a_unique_vertex",
        "unique_vertices_linked_to_multiple_corners",
        "unique_vertices_linked_to_multiple_internals_corner",
        "unique_vertices_linked_to_not_internal_nor_boundary_corner",
        "unique_vertices_linked_to_not_boundary_line_corner",
    ],
    "SectionLinesTopologyInspectionResult": [
        "lines_not_meshed",
        "lines_not_linked_to_a_unique_vertex",
        "unique_vertices_linked_to_line_with_wrong_relationship_to_surface",
        "unique_vertices_linked_to_a_line_with_invalid_embeddings",
        "unique_vertices_linked_to_a_single_and_invalid_line",
        "unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner",
    ],
    "SectionSurfacesTopologyInspectionResult": [
        "surfaces_not_meshed",
        "surfaces_not_linked_to_a_unique_vertex",
        "unique_vertices_linked_to_a_surface_with_invalid_embbedings",
        "unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border",
    ],
    "SectionTopologyInspectionResult": [
        "corners",
        "lines",
        "surfaces",
        "unique_vertices_not_linked_to_any_component",
        "unique_vertices_linked_to_inexistant_cmv",
        "unique_vertices_nonbijectively_linked_to_cmv",
    ],
    "BRepInspectionResult": ["meshes", "topology"],
    "SectionInspectionResult": ["meshes", "topology"],
}


def test_members_are_exposed():
    for name, members in MEMBERS.items():
        result_class = getattr(inspection, name)
        result = result_class()
        for member in members:
            check(hasattr(result, member), f"{name} has no member {member}")
            property_ = getattr(result_class, member)
            check(isinstance(property_, property), f"{name}.{member} is a property")
            check((property_.__doc__ or "").strip(), f"{name}.{member} has no doc")
            check(
                "value" in (property_.fset.__doc__ or ""),
                f"{name}.{member} setter argument is not named: "
                f"{property_.fset.__doc__}",
            )
        check(result.nb_issues() == 0, f"{name} should have no issue")
        check(result.inspection_type(), f"{name} has no inspection type")
        check(str(result) == result.string(), f"{name} str is the report")
        check(
            repr(result) == f"{name}(nb_issues=0)",
            f"{name} repr {repr(result)}",
        )
    # Every result class of the module is checked
    result_classes = {
        name
        for name, member in vars(inspection).items()
        if inspect.isclass(member) and name.endswith("InspectionResult")
    }
    check(result_classes == set(MEMBERS), f"unchecked {result_classes ^ set(MEMBERS)}")


def test_members_are_writable():
    result = inspection.SurfaceInspectionResult()
    issues = inspection.InspectionIssuesIndex("isolated vertices of a test")
    issues.add_issue(2, "vertex 2")
    issues.add_issue(5, "vertex 5")
    result.isolated_vertices = issues
    check(result.nb_issues() == 2, "the members are summed")
    check(result.isolated_vertices.issues() == [2, 5], "the member is set")
    check(
        repr(result) == "SurfaceInspectionResult(nb_issues=2, isolated_vertices=2)",
        repr(result),
    )
    check("vertex 5" in str(result), "str gives the messages")
    try:
        result.isolated_vertices = 3
    except TypeError:
        pass
    else:
        raise ValueError("[Test] a member only accepts its type")
    brep_result = inspection.BRepInspectionResult()
    brep_result.topology.corners.corners_not_meshed.add_issue(
        opengeode.uuid(), "a Corner"
    )
    # Members are references to the members of the result
    check(brep_result.nb_issues() == 1, "nested members are modified in place")
    check(
        repr(brep_result) == "BRepInspectionResult(nb_issues=1, topology=1)",
        repr(brep_result),
    )


def test_issues():
    issues = inspection.InspectionIssuesIndex()
    check(issues.description().startswith("Default inspection issue"), "description")
    issues.set_description("things")
    check(issues.description() == "things", "description is set")
    check(issues.string() == "no issues with things\n", issues.string())
    issues.add_issue(4, "four")
    issues.add_issue(1, "one")
    check(issues.nb_issues() == 2 and issues.issues() == [4, 1], "issue order")
    check(
        issues.string() == "2 issues with things\n\tfour\n\tone\n", issues.string()
    )
    check(repr(issues) == "InspectionIssuesIndex(description='things', nb_issues=2)", repr(issues))
    groups = inspection.InspectionIssuesVectorIndex("groups")
    groups.add_issue([1, 2], "group")
    check(groups.issues() == [[1, 2]], "list issues")
    pairs = inspection.InspectionIssuesPairIndex("pairs")
    pairs.add_issue((3, 4), "pair")
    check(pairs.issues() == [(3, 4)], "pair issues")
    edges = inspection.InspectionIssuesArrayIndex2("edges")
    edges.add_issue([3, 4], "edge")
    check(edges.issues() == [[3, 4]], "array issues")


def test_issues_map():
    issues_map = inspection.InspectionIssuesMapIndex("many things")
    check(issues_map.nb_issues() == 0, "empty map")
    check(
        issues_map.string() == "no issues with many things\n", issues_map.string()
    )
    first, second = opengeode.uuid(), opengeode.uuid()
    one = inspection.InspectionIssuesIndex("one")
    one.add_issue(7, "seven")
    issues_map.add_issues_to_map(first, one)
    # A component without issue is not added
    issues_map.add_issues_to_map(second, inspection.InspectionIssuesIndex("none"))
    check(issues_map.nb_components_with_issues() == 1, "only the issues are added")
    two = inspection.InspectionIssuesIndex("two")
    two.add_issue(8, "eight")
    two.add_issue(9, "nine")
    issues_map.add_issues_to_map(second, two)
    check(issues_map.nb_issues() == 3, "issues of every component")
    check(issues_map.nb_components_with_issues() == 2, "two components")
    content = issues_map.issues_map()
    check(list(content) == [first, second], "order of insertion")
    check(content[second].issues() == [8, 9], "issues of the component")
    check(
        repr(issues_map)
        == "InspectionIssuesMapIndex(description='many things', nb_issues=3, nb_components_with_issues=2)",
        repr(issues_map),
    )
    check("seven" in str(issues_map), "the report has the messages")
    # issues_map is a copy
    content[first].add_issue(1, "added")
    check(issues_map.nb_issues() == 3, "issues_map returns a copy")


def test_non_manifold_structures():
    first, second = opengeode.uuid(), opengeode.uuid()
    edge = inspection.BRepNonManifoldEdge([3, 5], [first, second])
    check(edge.edge == [3, 5], "edge vertices")
    check(edge.component_ids == [first, second], "edge components")
    edge.edge = [1, 2]
    check(edge.edge == [1, 2], "edge is writable")
    check(inspection.BRepNonManifoldEdge().component_ids == [], "default edge")
    facet = inspection.BRepNonManifoldFacet([1, 2, 3], [first])
    check(list(facet.facet) == [1, 2, 3], "facet vertices")
    check(facet.component_ids == [first], "facet components")
    issues = inspection.InspectionIssuesBRepNonManifoldFacet("facets")
    issues.add_issue(facet, "facet")
    check(issues.issues()[0].component_ids == [first], "issue of a structure")
    result = inspection.BRepMeshesManifoldInspectionResult()
    result.brep_non_manifold_facets = issues
    check(result.nb_issues() == 1, "BRep non manifold facets are counted")


def test_object_validity():
    result = validity.ObjectValidity()
    check(result.nb_issues() == 0 and result.string() == "", "valid object")
    result.invalidities = ["first issue\n", "second issue\n"]
    check(result.nb_issues() == 2, "two invalidities")
    check(result.string() == "first issue\nsecond issue\n", result.string())
    check(str(result) == result.string(), "str")
    check(repr(result) == "ObjectValidity(nb_issues=2)", repr(result))
    check(
        "value" in validity.ObjectValidity.invalidities.fset.__doc__,
        "the setter argument is named",
    )


def test_constructors_without_inputs():
    # The inspectors need an object: no constructor without argument
    for name in ["SurfaceMeshAdjacency2D", "BRepInspector", "SolidMeshInspector3D"]:
        try:
            getattr(inspection, name)()
        except TypeError:
            continue
        raise ValueError(f"[Test] {name}() needs the object to inspect")
    # Bound without constructor: the error says how to use the class
    for library in [
        inspection.OpenGeodeInspectorInspectionLibrary,
        validity.OpenGeodeInspectorValidityLibrary,
    ]:
        check(
            "has only static methods" in library.__init__.__doc__,
            f"{library.__name__}: missing constructor is documented",
        )
        try:
            library()
        except TypeError as error:
            check("static methods" in str(error), f"{error}")
        else:
            raise ValueError(f"[Test] {library.__name__}() should not be possible")


if __name__ == "__main__":
    test_members_are_exposed()
    test_members_are_writable()
    test_issues()
    test_issues_map()
    test_non_manifold_structures()
    test_object_validity()
    test_constructors_without_inputs()
