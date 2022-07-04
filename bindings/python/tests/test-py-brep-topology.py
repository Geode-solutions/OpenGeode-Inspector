# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2022 Geode-solutions
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

import os, sys, platform
if sys.version_info >= (3,8,0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ['PATH'].split(';') if x]:
        os.add_dll_directory(path)

import opengeode 
import opengeode_geosciences as og_geosciences
import opengeode_inspector_py_inspector as inspector

def check_components_linking( brep_inspector ):
    nb_unlinked_corners = brep_inspector.nb_corners_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_corners, " corners not linked to a unique vertex." )
    nb_unlinked_lines = brep_inspector.nb_lines_meshed_but_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_lines, " lines meshed but not linked to a unique vertex." )
    nb_unlinked_surfaces = brep_inspector.nb_surfaces_meshed_but_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_surfaces, " surfaces meshed but not linked to a unique vertex." )
    nb_unlinked_blocks = brep_inspector.nb_blocks_meshed_but_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_blocks, " blocks meshed but not linked to a unique vertex." )
    return nb_unlinked_blocks + nb_unlinked_surfaces + nb_unlinked_lines + nb_unlinked_corners

def check_unique_vertices_colocation( brep_inspector ):
    nb_unique_vertices_linked_to_different_points = brep_inspector.nb_unique_vertices_linked_to_different_points()
    print( "There are ", nb_unique_vertices_linked_to_different_points, " vertices linked to different points in space." )
    nb_colocated_unique_vertices = brep_inspector.nb_colocated_unique_vertices()
    print( "There are ", nb_colocated_unique_vertices, " vertices linked to different points in space." )
    return nb_unique_vertices_linked_to_different_points + nb_colocated_unique_vertices

def check_invalid_components_topology_unique_vertices( brep_inspector ):
    invalid_components_unique_vertices = brep_inspector.invalid_components_topology_unique_vertices()
    print( "There are ", len( invalid_components_unique_vertices ), " vertices with invalid components." )
    for vertex_index in invalid_components_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " has invalid components." )
    return len( invalid_components_unique_vertices )

def check_multiple_corners_unique_vertices( brep_inspector ):
    multiple_corners_unique_vertices = brep_inspector.multiple_corners_unique_vertices()
    print( "There are ", len( multiple_corners_unique_vertices ), " vertices with multiple corners." )
    for vertex_index in multiple_corners_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is associated to multiple corners." )
    return len( multiple_corners_unique_vertices )

def check_multiple_internals_corner_vertices( brep_inspector ):
    multiple_internals_corner_vertices = brep_inspector.multiple_internals_corner_vertices()
    print( "There are ", len( multiple_internals_corner_vertices ), " vertices with multiple internals." )
    for vertex_index in multiple_internals_corner_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is a corner associated with multiple embeddings." )
    return len( multiple_internals_corner_vertices )

def check_not_internal_nor_boundary_corner_vertices( brep_inspector ):
    not_internal_nor_boundary_corner_vertices = brep_inspector.not_internal_nor_boundary_corner_vertices()
    print( "There are ", len( not_internal_nor_boundary_corner_vertices ), " corner vertices with no boundary nor internal property." )
    for vertex_index in not_internal_nor_boundary_corner_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is neither internal nor a boundary." )
    return len( not_internal_nor_boundary_corner_vertices )

def check_line_corners_without_boundary_status( brep_inspector ):
    line_corners_without_boundary_status = brep_inspector.line_corners_without_boundary_status()
    print( "There are ", len( line_corners_without_boundary_status ), " corner vertices part of a line but not its boundary." )
    for vertex_index in line_corners_without_boundary_status:
        print( "[Test] Model unique vertex with index ", vertex_index, " is a corner but has a line for which it is not a boundary." )
    return len( line_corners_without_boundary_status )

def check_part_of_not_boundary_nor_internal_line_unique_vertices( brep_inspector ):
    part_of_not_boundary_nor_internal_line_unique_vertices = brep_inspector.part_of_not_boundary_nor_internal_line_unique_vertices()
    print( "There are ", len( part_of_not_boundary_nor_internal_line_unique_vertices ), " vertices part of a line which is not boundary not internal." )
    for vertex_index in part_of_not_boundary_nor_internal_line_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a line which is neither boundary nor internal." )
    return len( part_of_not_boundary_nor_internal_line_unique_vertices )

def check_part_of_line_with_invalid_internal_topology_unique_vertices( brep_inspector ):
    part_of_line_with_invalid_internal_topology_unique_vertices = brep_inspector.part_of_line_with_invalid_internal_topology_unique_vertices()
    print( "There are ", len( part_of_line_with_invalid_internal_topology_unique_vertices ), " vertices part of lines with invalid internal property." )
    for vertex_index in part_of_line_with_invalid_internal_topology_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a line with invalid internal properties." )
    return len( part_of_line_with_invalid_internal_topology_unique_vertices )

def check_part_of_invalid_unique_line_unique_vertices( brep_inspector ):
    part_of_invalid_unique_line_unique_vertices = brep_inspector.part_of_invalid_unique_line_unique_vertices()
    print( "There are ", len( part_of_invalid_unique_line_unique_vertices ), " vertices part of a unique line with invalid toplogy." )
    for vertex_index in part_of_invalid_unique_line_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a unique line with invalid topological properties." )
    return len( part_of_invalid_unique_line_unique_vertices )

def check_part_of_lines_but_not_corner_unique_vertices( brep_inspector ):
    part_of_lines_but_not_corner_unique_vertices = brep_inspector.part_of_lines_but_not_corner_unique_vertices()
    print( "There are ", len( part_of_lines_but_not_corner_unique_vertices ), " vertices part of multiple lines but not corner." )
    for vertex_index in part_of_lines_but_not_corner_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of multiple lines but is not a corner." )
    return len( part_of_lines_but_not_corner_unique_vertices )

def check_part_of_not_boundary_nor_internal_surface_unique_vertices( brep_inspector ):
    part_of_not_boundary_nor_internal_surface_unique_vertices = brep_inspector.part_of_not_boundary_nor_internal_surface_unique_vertices()
    print( "There are ", len( part_of_not_boundary_nor_internal_surface_unique_vertices ), " vertices part of a surface which is neither internal nor boundary." )
    for vertex_index in part_of_not_boundary_nor_internal_surface_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a surface which is neither internal nor boundary." )
    return len( part_of_not_boundary_nor_internal_surface_unique_vertices )

def check_part_of_surface_with_invalid_internal_topology_unique_vertices( brep_inspector ):
    part_of_surface_with_invalid_internal_topology_unique_vertices = brep_inspector.part_of_surface_with_invalid_internal_topology_unique_vertices()
    print( "There are ", len( part_of_surface_with_invalid_internal_topology_unique_vertices ), " vertices part of a surface with invalid internal topology." )
    for vertex_index in part_of_surface_with_invalid_internal_topology_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a surface with invalid internal topology." )
    return len( part_of_surface_with_invalid_internal_topology_unique_vertices )

def check_part_of_invalid_unique_surface_unique_vertices( brep_inspector ):
    part_of_invalid_unique_surface_unique_vertices = brep_inspector.part_of_invalid_unique_surface_unique_vertices()
    print( "There are ", len( part_of_invalid_unique_surface_unique_vertices ), " vertices part of a unique surface with invalid topology." )
    for vertex_index in part_of_invalid_unique_surface_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a unique surface with invalid topology." )
    return len( part_of_invalid_unique_surface_unique_vertices )

def check_part_of_invalid_multiple_surfaces_unique_vertices( brep_inspector ):
    part_of_invalid_multiple_surfaces_unique_vertices = brep_inspector.part_of_invalid_multiple_surfaces_unique_vertices()
    print( "There are ", len( part_of_invalid_multiple_surfaces_unique_vertices ), " vertices part of invalid multiple surfaces." )
    for vertex_index in part_of_invalid_multiple_surfaces_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of invalid multiple surfaces." )
    return len( part_of_invalid_multiple_surfaces_unique_vertices )

def check_part_of_line_and_not_on_surface_border_unique_vertices( brep_inspector ):
    part_of_line_and_not_on_surface_border_unique_vertices = brep_inspector.part_of_line_and_not_on_surface_border_unique_vertices()
    print( "There are ", len( part_of_line_and_not_on_surface_border_unique_vertices ), " vertices part of invalid multiple surfaces." )
    for vertex_index in part_of_line_and_not_on_surface_border_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a line and a surface but is not on the border of the surface mesh." )
    return len( part_of_line_and_not_on_surface_border_unique_vertices )

def check_part_of_invalid_blocks_unique_vertices( brep_inspector ):
    part_of_invalid_blocks_unique_vertices = brep_inspector.part_of_invalid_blocks_unique_vertices()
    print( "There are ", len( part_of_invalid_blocks_unique_vertices ), " vertices with invalid block topology." )
    for vertex_index in part_of_invalid_blocks_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " has invalid blocks topology." )
    return len( part_of_invalid_blocks_unique_vertices )

def launch_topological_validity_checks( brep_inspector ):
    nb_invalids = check_multiple_corners_unique_vertices( brep_inspector )
    nb_invalids += check_multiple_internals_corner_vertices( brep_inspector )
    nb_invalids += check_not_internal_nor_boundary_corner_vertices( brep_inspector )
    nb_invalids += check_line_corners_without_boundary_status( brep_inspector )
    nb_invalids += check_part_of_not_boundary_nor_internal_line_unique_vertices( brep_inspector )
    nb_invalids += check_part_of_invalid_unique_line_unique_vertices( brep_inspector )
    nb_invalids += check_part_of_lines_but_not_corner_unique_vertices( brep_inspector )
    nb_invalids += check_part_of_not_boundary_nor_internal_surface_unique_vertices( brep_inspector )
    nb_invalids += check_part_of_surface_with_invalid_internal_topology_unique_vertices( brep_inspector )
    nb_invalids += check_part_of_invalid_unique_surface_unique_vertices( brep_inspector )
    nb_invalids += check_part_of_invalid_multiple_surfaces_unique_vertices( brep_inspector )
    nb_invalids += check_part_of_line_and_not_on_surface_border_unique_vertices( brep_inspector )
    nb_invalids += check_part_of_invalid_blocks_unique_vertices( brep_inspector )

    if nb_invalids != check_invalid_components_topology_unique_vertices( brep_inspector ):
        raise ValueError( "[Test] Wrong number of invalid checks." )
    nb_invalids += check_components_linking( brep_inspector )
    nb_invalids += check_unique_vertices_colocation( brep_inspector )
    return nb_invalids

def check_a1_vertices_topology():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_brep = opengeode.load_brep( data_dir + "/model_A1.og_brep" )
    brep_inspector = inspector.BRepInspector( model_brep )
    if brep_inspector.brep_topology_is_valid():
        print( "model_A1 topology is valid." )
    else:
        print( "model_A1 topology is invalid." )
    nb_model_issues = launch_topological_validity_checks( brep_inspector )
    if nb_model_issues != 1254:
        raise ValueError( "[Test] model model_A1 should have 1254 unique vertices with topological problems." )

def check_a1_valid_vertices_topology():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_brep = opengeode.load_brep( data_dir + "/model_A1_valid.og_brep" )
    brep_inspector = inspector.BRepInspector( model_brep )
    if brep_inspector.brep_topology_is_valid():
        print( "model_A1_valid topology is valid." )
    else:
        print( "model_A1_valid topology is invalid." )
    nb_model_issues = launch_topological_validity_checks( brep_inspector )
    if nb_model_issues != 1254:
        raise ValueError( "[Test] model model_A1_valid should have 1254 unique vertices with topological problems." )

def check_mss_vertices_topology():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_brep = og_geosciences.load_structural_model( data_dir + "/mss.og_strm" )
    brep_inspector = inspector.BRepInspector( model_brep )
    if brep_inspector.brep_topology_is_valid():
        print( "model mss topology is valid." )
    else:
        print( "model mss topology is invalid." )
    nb_model_issues = launch_topological_validity_checks( brep_inspector )
    if nb_model_issues != 17:
        raise ValueError( "[Test] model mss.og_strm should have 17 unique vertices with topological problems." )

def check_model_D_vertices_topology():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_brep = opengeode.load_brep( data_dir + "/model_D.og_brep" )
    brep_inspector = inspector.BRepInspector( model_brep )
    if brep_inspector.brep_topology_is_valid():
        print( "model_D topology is valid." )
    else:
        print( "model_D topology is invalid." )
    nb_model_issues = launch_topological_validity_checks( brep_inspector )
    if nb_model_issues != 0:
        raise ValueError( "[Test] model model_D.og_brep should have 0 unique vertices with topological problems." )

if __name__ == '__main__':
    check_a1_vertices_topology()
    check_a1_valid_vertices_topology()
    check_mss_vertices_topology()
    check_model_D_vertices_topology()