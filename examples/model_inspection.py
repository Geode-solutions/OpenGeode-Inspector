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

"""Inspect the topology and the meshes of a BRep and print the issues found.

Usage: python model_inspection.py [model.og_brep]
"""

import os
import sys

import opengeode
import opengeode_inspector as inspector


def print_issues(name, issues):
    """Print the number of issues of an InspectionIssues (a list of issues,
    for example the indices of the unique vertices with a problem)."""
    print("There are", issues.nb_issues(), name)
    for issue in issues.issues():
        print("    ", issue)


def print_issues_by_component(name, issues_map):
    """Print the issues of an InspectionIssuesMap: the issues of each
    component (indexed by its uuid) having some."""
    print(
        "There are",
        issues_map.nb_issues(),
        name,
        "in",
        issues_map.nb_components_with_issues(),
        "components",
    )
    for component_id, issues in issues_map.issues_map().items():
        print("    ", component_id.string(), ":", issues.nb_issues())


def check_components_linking(topology):
    """The mesh vertices of each component must be linked to unique vertices."""
    print_issues(
        "corners without mesh vertex",
        topology.corners.corners_not_meshed,
    )
    print_issues_by_component(
        "corner vertices not linked to a unique vertex",
        topology.corners.corners_not_linked_to_a_unique_vertex,
    )
    print_issues_by_component(
        "line vertices not linked to a unique vertex",
        topology.lines.lines_not_linked_to_a_unique_vertex,
    )
    print_issues_by_component(
        "surface vertices not linked to a unique vertex",
        topology.surfaces.surfaces_not_linked_to_a_unique_vertex,
    )
    print_issues_by_component(
        "block vertices not linked to a unique vertex",
        topology.blocks.blocks_not_linked_to_a_unique_vertex,
    )


def check_unique_vertices(topology):
    """The links between the unique vertices and the component vertices."""
    print_issues(
        "unique vertices not linked to any component",
        topology.unique_vertices_not_linked_to_any_component,
    )
    print_issues(
        "unique vertices linked to an inexistant component vertex",
        topology.unique_vertices_linked_to_inexistant_cmv,
    )
    print_issues(
        "unique vertices not bijectively linked to the component vertices",
        topology.unique_vertices_nonbijectively_linked_to_cmv,
    )
    print_issues(
        "unique vertices shared by several corners",
        topology.corners.unique_vertices_linked_to_multiple_corners,
    )
    print_issues(
        "unique vertices of a line with an invalid topology",
        topology.lines.unique_vertices_linked_to_a_single_and_invalid_line,
    )
    print_issues(
        "unique vertices of lines that are not corners",
        topology.lines.unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner,
    )
    print_issues(
        "unique vertices of several invalid surfaces",
        topology.surfaces.unique_vertices_linked_to_several_and_invalid_surfaces,
    )
    print_issues(
        "unique vertices of two blocks without a boundary surface",
        topology.blocks.unique_vertices_part_of_two_blocks_and_no_boundary_surface,
    )


def inspect_topology(brep):
    """Check the topology of the BRep through its unique vertices."""
    topology_inspector = inspector.BRepTopologyInspector(brep)
    if topology_inspector.brep_topology_is_valid():
        print("Model topology is valid.")
    else:
        print("Model topology is invalid.")
    topology = topology_inspector.inspect_brep_topology()
    # repr gives the number of issues of each type of component
    print(repr(topology))
    check_components_linking(topology)
    check_unique_vertices(topology)
    return topology


def inspect_meshes(brep):
    """Check the meshes of the components of the BRep."""
    meshes = inspector.BRepMeshesInspector(brep).inspect_brep_meshes()
    print(
        "There are",
        meshes.unique_vertices_colocation.nb_issues(),
        "colocation issues of the unique vertices,",
        meshes.meshes_adjacencies.nb_issues(),
        "adjacency issues,",
        meshes.meshes_degenerations.nb_issues(),
        "degenerated elements,",
        meshes.meshes_non_manifolds.nb_issues(),
        "non manifold elements and",
        meshes.meshes_negative_elements.nb_issues(),
        "negative elements in the component meshes.",
    )
    return meshes


def inspect_model(filename):
    """Load the BRep and inspect it, return the BRepInspectionResult."""
    brep = opengeode.load(opengeode.BRep, filename)
    inspect_topology(brep)
    inspect_meshes(brep)
    # Both inspections in one call: repr summarizes the result, str gives the
    # full report
    result = inspector.inspect_brep(brep)
    print(repr(result))
    return result


def default_filename():
    data_directory = os.path.join(os.path.dirname(__file__), "..", "tests", "data")
    return os.path.abspath(os.path.join(data_directory, "model_A1_valid.og_brep"))


if __name__ == "__main__":
    inspect_model(sys.argv[1] if len(sys.argv) > 1 else default_filename())
