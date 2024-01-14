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

import opengeode
import opengeode_inspector_py_inspector as inspector


def corners_topological_validity( result, verbose ): 
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
    print( "Section Corners Topology check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues

def lines_topological_validity( result, verbose ):
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
    nb_issues += unique_vertices_linked_to_not_internal_nor_boundary_line.number()
    unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner = result.unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
    nb_issues += unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner.number()
    print( "Section Lines Topology check: ", nb_issues, " issues." )
    if verbose : print( result.string(), "\n" )
    return nb_issues

def surfaces_topological_validity( result, verbose ):
    nb_issues = 0
    surfaces_not_linked_to_a_unique_vertex = result.surfaces_not_linked_to_a_unique_vertex
    for issue in surfaces_not_linked_to_a_unique_vertex :
        nb_issues += issue[1].number()
    surfaces_not_meshed = result.surfaces_not_meshed
    nb_issues += surfaces_not_meshed.number()
    unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border = result.unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
    nb_issues += unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border.number()
    unique_vertices_linked_to_a_surface_with_invalid_embbedings = result.unique_vertices_linked_to_a_surface_with_invalid_embbedings
    nb_issues += unique_vertices_linked_to_a_surface_with_invalid_embbedings.number()
    print( "Section Surfaces Topology check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues

def launch_topological_validity_checks( result, verbose ) :
    nb_issues = corners_topological_validity( result.corners, verbose )
    nb_issues += lines_topological_validity( result.lines, verbose )
    nb_issues += surfaces_topological_validity( result.surfaces, verbose )
    return nb_issues

def meshes_adjacencies_validity( result, verbose ):
    nb_issues = 0
    surfaces_edges_with_wrong_adjacencies = result.surfaces_edges_with_wrong_adjacencies
    for issue in surfaces_edges_with_wrong_adjacencies :
        nb_issues += issue[1].number()
    print( "Section meshes adjacencies check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues

def meshes_degenerations_validity( result, verbose ):
    nb_issues = 0
    elements = result.elements
    for degenerated_elements in elements :
        nb_issues += degenerated_elements[1].degenerated_edges.number()
        nb_issues += degenerated_elements[1].degenerated_polygons.number()
    print("Section meshes degenerated elements check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues

def meshes_intersections_validity( result, verbose ) :
    nb_issues = 0
    elements_intersections = result.elements_intersections
    nb_issues += elements_intersections.number()

    print( "Section meshes element intersections check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues

def meshes_manifolds_validity( result, verbose ):
    nb_issues = 0
    meshes_non_manifold_vertices = result.meshes_non_manifold_vertices
    for issue in meshes_non_manifold_vertices :
        nb_issues += issue[1].number()
    meshes_non_manifold_edges = result.meshes_non_manifold_edges
    for issue in meshes_non_manifold_edges :
        nb_issues += issue[1].number()
    print( "Section meshes non manifolds check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues

def meshes_colocations_validity( result, verbose ): 
    nb_issues = 0
    colocated_points_groups = result.colocated_points_groups
    for  issue in colocated_points_groups :
        nb_issues += issue[1].number()
    print( "Section meshes Colocations check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues

def meshes_unique_vertices_validity( result, verbose ):
    nb_issues = 0
    colocated_unique_vertices_groups = result.colocated_unique_vertices_groups
    for issue in colocated_unique_vertices_groups.problems :
        nb_issues += len(issue)
    unique_vertices_linked_to_different_points = result.unique_vertices_linked_to_different_points
    nb_issues += unique_vertices_linked_to_different_points.number()
    print("Section unique vertices check: ", nb_issues, " issues." )
    if verbose :
        print( result.string(), "\n" )
    return nb_issues

def launch_component_meshes_validity_checks(result, verbose ):
    nb_issues = meshes_adjacencies_validity( result.adjacencies, verbose )
    nb_issues = meshes_degenerations_validity( result.degenerations, verbose )
    nb_issues += meshes_intersections_validity( result.intersections, verbose )
    nb_issues += meshes_manifolds_validity( result.manifolds, verbose )
    nb_issues += meshes_colocations_validity( result.meshes_colocation, verbose )
    nb_issues += meshes_unique_vertices_validity(
        result.unique_vertices_colocation, verbose )
    return nb_issues

def check_section(verbose):
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_section = opengeode.load_section(
        data_dir + "/vertical_lines.og_sctn")
    section_inspector = inspector.SectionInspector(model_section)
    result = section_inspector.inspect_section()
    if section_inspector.section_topology_is_valid():
        print("model vertical_lines topology is valid.")
    else:
        print("model vertical_lines topology is invalid.")
    nb_issues = launch_topological_validity_checks(result.topology, verbose)
    if nb_issues != 0:
        raise ValueError(
            "[Test] There should be no topological issues with vertical_lines.og_sctn")
    nb_issues = launch_component_meshes_validity_checks(result.meshes, verbose)
    if nb_issues != 0:
        raise ValueError(
            "[Test] There should be no mesh issues with vertical_lines.og_sctn")


if __name__ == '__main__':
    inspector.InspectorInspectorLibrary.initialize()
    verbose = False
    check_section(verbose)
