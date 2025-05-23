# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2025 Geode-solutions
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
    for path in [x.strip() for x in os.environ["PATH"].split("") if x]:
        os.add_dll_directory(path)

import opengeode
import opengeode_inspector_py_inspector as inspector


def corners_topological_validity(result, verbose):
    nb_issues = 0
    for corner_issue in result.corners_not_linked_to_a_unique_vertex.issues_map():
        nb_issues += corner_issue[1].nb_issues()
    nb_issues += result.corners_not_meshed.nb_issues()
    nb_issues += result.unique_vertices_liked_to_not_boundary_line_corner.nb_issues()
    nb_issues += result.unique_vertices_linked_to_multiple_corners.nb_issues()
    nb_issues += (
        result.unique_vertices_linked_to_not_internal_nor_boundary_corner.nb_issues()
    )
    print("BRep Corners Topology check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def lines_topological_validity(result, verbose):
    nb_issues = 0
    for issue in result.lines_not_linked_to_a_unique_vertex.issues_map():
        nb_issues += issue[1].nb_issues()
    nb_issues += result.lines_not_meshed.nb_issues()
    nb_issues += (
        result.unique_vertices_linked_to_a_line_with_invalid_embeddings.nb_issues()
    )
    nb_issues += result.unique_vertices_linked_to_a_single_and_invalid_line.nb_issues()
    nb_issues += (
        result.unique_vertices_linked_to_line_with_wrong_relationship_to_surface.nb_issues()
    )
    nb_issues += (
        result.unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner.nb_issues()
    )
    print("BRep Lines Topology check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def surfaces_topological_validity(result, verbose):
    nb_issues = 0
    for issue in result.surfaces_not_linked_to_a_unique_vertex.issues_map():
        nb_issues += issue[1].nb_issues()
    nb_issues += result.surfaces_not_meshed.nb_issues()
    nb_issues += (
        result.unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border.nb_issues()
    )
    nb_issues += (
        result.unique_vertices_linked_to_several_and_invalid_surfaces.nb_issues()
    )
    print("BRep Surfaces Topology check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def blocks_topological_validity(result, verbose):
    nb_issues = 0
    for issue in result.blocks_not_linked_to_a_unique_vertex.issues_map():
        nb_issues += issue[1].nb_issues()
    nb_issues += result.some_blocks_not_meshed.nb_issues()
    nb_issues += (
        result.unique_vertices_part_of_two_blocks_and_no_boundary_surface.nb_issues()
    )
    nb_issues += result.unique_vertices_with_incorrect_block_cmvs_count.nb_issues()
    nb_issues += (
        result.unique_vertices_linked_to_a_single_and_invalid_surface.nb_issues()
    )
    nb_issues += (
        result.unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks.nb_issues()
    )
    nb_issues += result.unique_vertex_linked_to_multiple_invalid_surfaces.nb_issues()
    nb_issues += result.model_boundaries_dont_form_a_closed_surface.nb_issues()
    nb_issues += result.blocks_with_not_closed_boundary_surfaces.nb_issues()
    print("BRep Blocks Topology check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def launch_topological_validity_checks(result, verbose):
    nb_issues = corners_topological_validity(result.corners, verbose)
    nb_issues += lines_topological_validity(result.lines, verbose)
    nb_issues += surfaces_topological_validity(result.surfaces, verbose)
    nb_issues += blocks_topological_validity(result.blocks, verbose)
    return nb_issues


def meshes_adjacencies_validity(result, verbose):
    nb_issues = 0
    for issue in result.surfaces_edges_with_wrong_adjacencies.issues_map():
        nb_issues += issue[1].nb_issues()
    for issue in result.blocks_facets_with_wrong_adjacencies.issues_map():
        nb_issues += issue[1].nb_issues()
    print("BRep meshes adjacencies check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def meshes_degenerations_validity(result, verbose):
    nb_issues = 0
    for degenerated_elements in result.degenerated_edges.issues_map():
        nb_issues += degenerated_elements[1].nb_issues()
    for degenerated_elements in result.degenerated_polygons.issues_map():
        nb_issues += degenerated_elements[1].nb_issues()
    for degenerated_elements in result.degenerated_polyhedra.issues_map():
        nb_issues += degenerated_elements[1].nb_issues()
    print("BRep meshes degenerated elements check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def meshes_intersections_validity(result, verbose):
    nb_issues = 0
    nb_issues += result.elements_intersections.nb_issues()
    print("BRep meshes element intersections check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def meshes_manifolds_validity(result, verbose):
    nb_issues = 0
    for issue in result.meshes_non_manifold_vertices.issues_map():
        nb_issues += issue[1].nb_issues()
    for issue in result.meshes_non_manifold_edges.issues_map():
        nb_issues += issue[1].nb_issues()
    for issue in result.meshes_non_manifold_facets.issues_map():
        nb_issues += issue[1].nb_issues()
    for issue in result.brep_non_manifold_edges.issues():
        nb_issues += len(issue.component_ids)
    print("BRep meshes non manifolds check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def meshes_colocations_validity(result, verbose):
    nb_issues = 0
    for issue in result.colocated_points_groups.issues_map():
        nb_issues += issue[1].nb_issues()
    print("BRep meshes Colocations check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def meshes_unique_vertices_validity(result, verbose):
    nb_issues = 0
    for issue in result.colocated_unique_vertices_groups.issues():
        nb_issues += len(issue)
    nb_issues += result.unique_vertices_linked_to_different_points.nb_issues()
    print("BRep meshes Colocations check: ", nb_issues, " issues.")
    if verbose:
        print(result.string(), "\n")
    return nb_issues


def launch_component_meshes_validity_checks(result, verbose):
    nb_invalids = meshes_adjacencies_validity(result.meshes_adjacencies, verbose)
    nb_invalids += meshes_degenerations_validity(result.meshes_degenerations, verbose)
    nb_invalids += meshes_intersections_validity(result.meshes_intersections, verbose)
    nb_invalids += meshes_manifolds_validity(result.meshes_non_manifolds, verbose)
    nb_invalids += meshes_colocations_validity(result.meshes_colocation, verbose)
    nb_invalids += meshes_unique_vertices_validity(
        result.unique_vertices_colocation, verbose
    )
    return nb_invalids


def check_a1(verbose):
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_brep = opengeode.load_brep(data_dir + "/model_A1.og_brep")
    result = inspector.inspect_brep(model_brep)
    brep_inspector = inspector.BRepInspector(model_brep)
    if brep_inspector.brep_topology_is_valid():
        print("model_A1 topology is valid.")
    else:
        print("model_A1 topology is invalid.")
    nb_model_issues = launch_topological_validity_checks(result.topology, verbose)
    if nb_model_issues != 5165:
        raise ValueError(
            "[Test] model model_A1 should have 5615 unique vertices with topological problems."
        )
    nb_component_meshes_issues = launch_component_meshes_validity_checks(
        result.meshes, verbose
    )
    if nb_component_meshes_issues != 13494:
        raise ValueError(
            "[Test] model model_A1_valid should have 13494 component meshes issues (pairs of component meshes triangles intersecting)."
        )


def check_a1_valid(verbose):
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_brep = opengeode.load_brep(data_dir + "/model_A1_valid.og_brep")
    brep_inspector = inspector.BRepInspector(model_brep)
    result = brep_inspector.inspect_brep()
    if brep_inspector.brep_topology_is_valid():
        print("model_A1_valid topology is valid.")
    else:
        print("model_A1_valid topology is invalid.")
    nb_model_issues = launch_topological_validity_checks(result.topology, verbose)
    if nb_model_issues != 5165:
        raise ValueError(
            "[Test] model model_A1_valid should have 5165 topological problems."
        )
    nb_component_meshes_issues = launch_component_meshes_validity_checks(
        result.meshes, verbose
    )
    if nb_component_meshes_issues != 13494:
        raise ValueError(
            "[Test] model model_A1_valid should have 13494 component meshes issues (pairs of component meshes triangles intersecting)."
        )


def check_model_mss(verbose):
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_brep = opengeode.load_brep(data_dir + "/mss.og_brep")
    brep_inspector = inspector.BRepInspector(model_brep)
    result = brep_inspector.inspect_brep()
    if brep_inspector.brep_topology_is_valid():
        print("model mss topology is valid.")
    else:
        print("model mss topology is invalid.")
    nb_model_issues = launch_topological_validity_checks(result.topology, verbose)
    if nb_model_issues != 36:
        raise ValueError(
            "[Test] model mss.og_strm should have 36 topological problems."
        )
    nb_component_meshes_issues = launch_component_meshes_validity_checks(
        result.meshes, verbose
    )
    if nb_component_meshes_issues != 0:
        raise ValueError("[Test] model mss should have no component meshes issues.")


def check_model_D(verbose):
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_brep = opengeode.load_brep(data_dir + "/model_D.og_brep")
    brep_inspector = inspector.BRepInspector(model_brep)
    result = brep_inspector.inspect_brep()

    if brep_inspector.brep_topology_is_valid():
        print("model_D topology is valid.")
    else:
        print("model_D topology is invalid.")
    nb_model_issues = launch_topological_validity_checks(result.topology, verbose)
    if nb_model_issues != 0:
        raise ValueError(
            "[Test] model model_D.og_brep should have 0 topological problems (blocks not meshed)."
        )
    nb_component_meshes_issues = launch_component_meshes_validity_checks(
        result.meshes, verbose
    )
    if nb_component_meshes_issues != 0:
        raise ValueError("[Test] model_D should have no component meshes issues.")


if __name__ == "__main__":
    inspector.InspectorInspectorLibrary.initialize()
    verbose = False
    check_a1(verbose)
    check_a1_valid(verbose)
    check_model_mss(verbose)
    check_model_D(verbose)
