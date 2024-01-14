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
    for path in [x.strip() for x in os.environ["PATH"].split("") if x]:
        os.add_dll_directory(path)

import opengeode
import opengeode_inspector_py_inspector as inspector


def corners_topological_validity(result, verbose ) :
    nb_issues = 0 
    corners_not_linked_to_a_unique_vertex = result.corners_not_linked_to_a_unique_vertex
    for corner_issue in corners_not_linked_to_a_unique_vertex :
        nb_issues += corner_issue[1].number()
    corners_not_meshed = result.corners_not_meshed
    nb_issues += corners_not_meshed.number()
    unique_vertices_liked_to_not_boundary_line_corner = result.unique_vertices_liked_to_not_boundary_line_corner
    nb_issues += unique_vertices_liked_to_not_boundary_line_corner.number()
    unique_vertices_linked_to_multiple_corners = result.unique_vertices_linked_to_multiple_corners
    nb_issues += unique_vertices_linked_to_multiple_corners.number()
    unique_vertices_linked_to_multiple_internals_corner = result.unique_vertices_linked_to_multiple_internals_corner
    nb_issues += unique_vertices_linked_to_multiple_internals_corner.number()
    unique_vertices_linked_to_not_internal_nor_boundary_corner = result.unique_vertices_linked_to_not_internal_nor_boundary_corner
    nb_issues += unique_vertices_linked_to_not_internal_nor_boundary_corner.number()
    print( "BRep Corners Topology check: ", nb_issues, " issues." )
    if verbose : 
        print(result.string(), "\n" ) 
    return nb_issues

def lines_topological_validity(result, verbose):
    nb_issues = 0
    lines_not_linked_to_a_unique_vertex = result.lines_not_linked_to_a_unique_vertex
    for issue in lines_not_linked_to_a_unique_vertex :
        nb_issues += issue[1].number()
    lines_not_meshed = result.lines_not_meshed
    nb_issues += lines_not_meshed.number()
    unique_vertices_linked_to_a_line_with_invalid_embeddings = result.unique_vertices_linked_to_a_line_with_invalid_embeddings
    nb_issues += unique_vertices_linked_to_a_line_with_invalid_embeddings.number()
    unique_vertices_linked_to_a_single_and_invalid_line = result.unique_vertices_linked_to_a_single_and_invalid_line
    nb_issues += unique_vertices_linked_to_a_single_and_invalid_line.number()
    unique_vertices_linked_to_not_internal_nor_boundary_line = result.unique_vertices_linked_to_not_internal_nor_boundary_line
    nb_issues +=unique_vertices_linked_to_not_internal_nor_boundary_line.number()
    unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner = result.unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
    nb_issues +=unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner.number()
    print("BRep Lines Topology check: ", nb_issues, " issues." )
    if verbose : print(result.string(), "\n" )
    return nb_issues

def surfaces_topological_validity( result, verbose ) :
    nb_issues = 0
    surfaces_not_linked_to_a_unique_vertex = result.surfaces_not_linked_to_a_unique_vertex
    for issue in surfaces_not_linked_to_a_unique_vertex :
        nb_issues += issue[1].number()
    surfaces_not_meshed = result.surfaces_not_meshed
    nb_issues += surfaces_not_meshed.number()
    unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border = result.unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
    nb_issues += unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border.number()
    unique_vertices_linked_to_a_single_and_invalid_surface = result.unique_vertices_linked_to_a_single_and_invalid_surface
    nb_issues += unique_vertices_linked_to_a_single_and_invalid_surface.number()
    unique_vertices_linked_to_not_internal_nor_boundary_surface = result.unique_vertices_linked_to_not_internal_nor_boundary_surface
    nb_issues += unique_vertices_linked_to_not_internal_nor_boundary_surface.number()
    unique_vertices_linked_to_several_and_invalid_surfaces = result.unique_vertices_linked_to_several_and_invalid_surfaces
    nb_issues += unique_vertices_linked_to_several_and_invalid_surfaces.number()
    
    print("BRep Surfaces Topology check: ", nb_issues, " issues." )
    if verbose : 
        print(result.string(), "\n" )
    return nb_issues

def blocks_topological_validity(result, verbose ):
    nb_issues = 0
    blocks_not_linked_to_a_unique_vertex = result.blocks_not_linked_to_a_unique_vertex
    for issue in blocks_not_linked_to_a_unique_vertex :
        nb_issues += issue[1].number()
    blocks_not_meshed = result.blocks_not_meshed
    nb_issues += blocks_not_meshed.number()
    unique_vertices_part_of_two_blocks_and_no_boundary_surface = result.unique_vertices_part_of_two_blocks_and_no_boundary_surface
    nb_issues += unique_vertices_part_of_two_blocks_and_no_boundary_surface.number()
    unique_vertices_with_incorrect_block_cmvs_count = result.unique_vertices_with_incorrect_block_cmvs_count
    nb_issues += unique_vertices_with_incorrect_block_cmvs_count.number()
    print(
        "BRep Blocks Topology check: ", nb_issues, " issues." )
    if verbose : 
        print(result.string(), "\n" )
    return nb_issues

def launch_topological_validity_checks(result, verbose ):
    nb_issues = corners_topological_validity( result.corners, verbose )
    nb_issues += lines_topological_validity( result.lines, verbose )
    nb_issues += surfaces_topological_validity( result.surfaces, verbose )
    nb_issues += blocks_topological_validity( result.blocks, verbose )
    return nb_issues

def meshes_adjacencies_validity(result, verbose ) :
    nb_issues= 0
    surfaces_edges_with_wrong_adjacencies = result.surfaces_edges_with_wrong_adjacencies
    for issue in surfaces_edges_with_wrong_adjacencies :
        nb_issues += issue[1].number()
    blocks_facets_with_wrong_adjacencies = result.blocks_facets_with_wrong_adjacencies
    for issue in blocks_facets_with_wrong_adjacencies :
        nb_issues += issue[1].number()
    print(
        "BRep meshes adjacencies check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues


def meshes_degenerations_validity (result, verbose) :
    nb_issues = 0
    elements = result.elements
    for degenerated_elements in elements :
        nb_issues += degenerated_elements[1].degenerated_edges.number()
        nb_issues += degenerated_elements[1].degenerated_polygons.number()
        nb_issues += degenerated_elements[1].degenerated_polyhedra.number()
    print(
        "BRep meshes degenerated elements check: ", nb_issues, " issues." )
    if verbose : print( result.string(), "\n" )
    return nb_issues

def meshes_intersections_validity( result, verbose ) :
    nb_issues = 0 
    elements_intersections = result.elements_intersections
    nb_issues += elements_intersections.number()
    print(
        "BRep meshes element intersections check: ", nb_issues, " issues." )
    if verbose : 
        print( result.string(), "\n" )
    return nb_issues


def meshes_manifolds_validity(result, verbose) :
    nb_issues= 0 
    meshes_non_manifold_vertices = result.meshes_non_manifold_vertices
    for issue in meshes_non_manifold_vertices :
        nb_issues += issue[1].number()
    meshes_non_manifold_edges = result.meshes_non_manifold_edges
    for issue in meshes_non_manifold_edges :
        nb_issues += issue[1].number()
    meshes_non_manifold_facets = result.meshes_non_manifold_facets
    for issue in meshes_non_manifold_facets :
        nb_issues += issue[1].number()
    model_non_manifold_edges = result.model_non_manifold_edges
    for issue in model_non_manifold_edges :
        nb_issues += issue[1].number()
    print("BRep meshes non manifolds check: ", nb_issues, " issues." )
    if verbose : 
       print(result.string(), "\n" )
    return nb_issues

def meshes_colocations_validity(result, verbose):
    nb_issues = 0
    colocated_points_groups = result.colocated_points_groups
    for issue in colocated_points_groups:
        nb_issues += issue[1].number()
    print(
        "BRep meshes Colocations check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues

def meshes_unique_vertices_validity(result, verbose):
    nb_issues = 0
    colocated_unique_vertices_groups = result.colocated_unique_vertices_groups
    for issue in colocated_unique_vertices_groups.problems :
        nb_issues += len(issue)
    unique_vertices_linked_to_different_points = result.unique_vertices_linked_to_different_points
    nb_issues += unique_vertices_linked_to_different_points.number()
    print("BRep meshes Colocations check: ", nb_issues, " issues." )
    if verbose :
        print(result.string(), "\n" )
    return nb_issues

def launch_component_meshes_validity_checks(result, verbose):
    nb_invalids = meshes_adjacencies_validity(result.adjacencies,verbose)
    nb_invalids += meshes_degenerations_validity(result.degenerations,verbose)
    nb_invalids += meshes_intersections_validity(result.intersections,verbose)
    nb_invalids += meshes_manifolds_validity(result.manifolds,verbose)
    nb_invalids += meshes_colocations_validity(result.meshes_colocation,verbose)
    nb_invalids += meshes_unique_vertices_validity(result.unique_vertices_colocation,verbose)
    return nb_invalids


def check_a1(verbose):
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_brep = opengeode.load_brep(data_dir + "/model_A1.og_brep")
    brep_inspector = inspector.BRepInspector(model_brep)
    result = brep_inspector.inspect_brep()
    if brep_inspector.brep_topology_is_valid():
        print("model_A1 topology is valid.")
    else:
        print("model_A1 topology is invalid.")
    nb_model_issues = launch_topological_validity_checks(result.topology,verbose)
    if nb_model_issues != 267:
        raise ValueError(
            "[Test] model model_A1 should have 267 unique vertices with topological problems."
        )
    nb_component_meshes_issues = launch_component_meshes_validity_checks(result.meshes,verbose)
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
    nb_model_issues = launch_topological_validity_checks(result.topology,verbose)
    if nb_model_issues != 267:
        raise ValueError(
            "[Test] model model_A1_valid should have 267 topological problems."
        )
    nb_component_meshes_issues = launch_component_meshes_validity_checks(result.meshes,verbose)
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
    nb_model_issues = launch_topological_validity_checks(result.topology,verbose)
    if nb_model_issues != 34:
        raise ValueError(
            "[Test] model mss.og_strm should have 34 topological problems."
        )
    nb_component_meshes_issues = launch_component_meshes_validity_checks(result.meshes, verbose)
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
    if nb_model_issues != 5:
        raise ValueError(
            "[Test] model model_D.og_brep should have 5 topological problems (blocks not meshed)."
        )
    nb_component_meshes_issues = launch_component_meshes_validity_checks(result.meshes, verbose)
    if nb_component_meshes_issues != 0:
        raise ValueError("[Test] model_D should have no component meshes issues.")


if __name__ == "__main__":
    inspector.InspectorInspectorLibrary.initialize()
    verbose = False
    check_a1(verbose)
    check_a1_valid(verbose)
    check_model_mss(verbose)
    check_model_D(verbose)
