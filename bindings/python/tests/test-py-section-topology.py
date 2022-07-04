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
import opengeode_inspector_py_inspector as inspector

def check_components_linking( section_inspector ):
    nb_unlinked_corners = section_inspector.nb_corners_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_corners, " corners not linked to a unique vertex." )
    nb_unlinked_lines = section_inspector.nb_lines_meshed_but_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_lines, " lines meshed but not linked to a unique vertex." )
    nb_unlinked_surfaces = section_inspector.nb_surfaces_meshed_but_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_surfaces, " surfaces meshed but not linked to a unique vertex." )
    return nb_unlinked_corners + nb_unlinked_lines + nb_unlinked_surfaces

def check_unique_vertices_colocation( section_inspector ):
    nb_unique_vertices_linked_to_different_points = section_inspector.nb_unique_vertices_linked_to_different_points()
    print( "There are ", nb_unique_vertices_linked_to_different_points, " vertices linked to different points in space." )
    nb_colocated_unique_vertices = section_inspector.nb_colocated_unique_vertices()
    print( "There are ", nb_colocated_unique_vertices, " vertices linked to different points in space." )
    return nb_unique_vertices_linked_to_different_points + nb_colocated_unique_vertices

def check_invalid_components_topology_unique_vertices( section_inspector ):
    invalid_components_unique_vertices = section_inspector.invalid_components_topology_unique_vertices()
    print( "There are ", len( invalid_components_unique_vertices ), " vertices with invalid components." )
    for vertex_index in invalid_components_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " has invalid components." )
    return len( invalid_components_unique_vertices )

def check_multiple_corners_unique_vertices( section_inspector ):
    multiple_corners_unique_vertices = section_inspector.multiple_corners_unique_vertices()
    print( "There are ", len( multiple_corners_unique_vertices ), " vertices with multiple corners." )
    for vertex_index in multiple_corners_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is associated to multiple corners." )
    return len( multiple_corners_unique_vertices )

def check_multiple_internals_corner_vertices( section_inspector ):
    multiple_internals_corner_vertices = section_inspector.multiple_internals_corner_vertices()
    print( "There are ", len( multiple_internals_corner_vertices ), " vertices with multiple internals." )
    for vertex_index in multiple_internals_corner_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is a corner associated with multiple embeddings." )
    return len( multiple_internals_corner_vertices )

def check_not_internal_nor_boundary_corner_vertices( section_inspector ):
    not_internal_nor_boundary_corner_vertices = section_inspector.not_internal_nor_boundary_corner_vertices()
    print( "There are ", len( not_internal_nor_boundary_corner_vertices ), " corner vertices with no boundary nor internal property." )
    for vertex_index in not_internal_nor_boundary_corner_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is neither internal nor a boundary." )
    return len( not_internal_nor_boundary_corner_vertices )

def check_line_corners_without_boundary_status( section_inspector ):
    line_corners_without_boundary_status = section_inspector.line_corners_without_boundary_status()
    print( "There are ", len( line_corners_without_boundary_status ), " corner vertices part of a line but not its boundary." )
    for vertex_index in line_corners_without_boundary_status:
        print( "[Test] Model unique vertex with index ", vertex_index, " is a corner but has a line for which it is not a boundary." )
    return len( line_corners_without_boundary_status )

def check_part_of_not_boundary_nor_internal_line_unique_vertices( section_inspector ):
    part_of_not_boundary_nor_internal_line_unique_vertices = section_inspector.part_of_not_boundary_nor_internal_line_unique_vertices()
    print( "There are ", len( part_of_not_boundary_nor_internal_line_unique_vertices ), " vertices part of a line which is not boundary not internal." )
    for vertex_index in part_of_not_boundary_nor_internal_line_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a line which is neither boundary nor internal." )
    return len( part_of_not_boundary_nor_internal_line_unique_vertices )

def check_part_of_line_with_invalid_internal_topology_unique_vertices( section_inspector ):
    part_of_line_with_invalid_internal_topology_unique_vertices = section_inspector.part_of_line_with_invalid_internal_topology_unique_vertices()
    print( "There are ", len( part_of_line_with_invalid_internal_topology_unique_vertices ), " vertices part of lines with invalid internal property." )
    for vertex_index in part_of_line_with_invalid_internal_topology_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a line with invalid internal properties." )
    return len( part_of_line_with_invalid_internal_topology_unique_vertices )

def check_part_of_invalid_unique_line_unique_vertices( section_inspector ):
    part_of_invalid_unique_line_unique_vertices = section_inspector.part_of_invalid_unique_line_unique_vertices()
    print( "There are ", len( part_of_invalid_unique_line_unique_vertices ), " vertices part of a unique line with invalid toplogy." )
    for vertex_index in part_of_invalid_unique_line_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a unique line with invalid topological properties." )
    return len( part_of_invalid_unique_line_unique_vertices )

def check_part_of_lines_but_not_corner_unique_vertices( section_inspector ):
    part_of_lines_but_not_corner_unique_vertices = section_inspector.part_of_lines_but_not_corner_unique_vertices()
    print( "There are ", len( part_of_lines_but_not_corner_unique_vertices ), " vertices part of multiple lines but not corner." )
    for vertex_index in part_of_lines_but_not_corner_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of multiple lines but is not a corner." )
    return len( part_of_lines_but_not_corner_unique_vertices )

def check_part_of_invalid_surfaces_unique_vertices( section_inspector ):
    part_of_invalid_surfaces_unique_vertices = section_inspector.part_of_invalid_surfaces_unique_vertices()
    print( "There are ", len( part_of_invalid_surfaces_unique_vertices ), " vertices with invalid surface topology." )
    for vertex_index in part_of_invalid_surfaces_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " has invalid surfaces topology." )
    return len( part_of_invalid_surfaces_unique_vertices )

def check_part_of_line_and_not_on_surface_border_unique_vertices( section_inspector ):
    part_of_line_and_not_on_surface_border_unique_vertices = section_inspector.part_of_line_and_not_on_surface_border_unique_vertices()
    print( "There are ", len( part_of_line_and_not_on_surface_border_unique_vertices ), " vertices part of a line and a surface but not on surface mesh border." )
    for vertex_index in part_of_line_and_not_on_surface_border_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a line and a surface but is not on surface mesh border." )
    return len( part_of_line_and_not_on_surface_border_unique_vertices )

def launch_topological_validity_checks( section_inspector ):
    nb_issues = check_multiple_corners_unique_vertices( section_inspector )
    nb_issues += check_multiple_internals_corner_vertices( section_inspector )
    nb_issues += check_not_internal_nor_boundary_corner_vertices( section_inspector )
    nb_issues += check_line_corners_without_boundary_status( section_inspector )
    nb_issues += check_part_of_not_boundary_nor_internal_line_unique_vertices( section_inspector )
    nb_issues += check_part_of_invalid_unique_line_unique_vertices( section_inspector )
    nb_issues += check_part_of_invalid_unique_line_unique_vertices( section_inspector )
    nb_issues += check_part_of_lines_but_not_corner_unique_vertices( section_inspector )
    nb_issues += check_part_of_invalid_surfaces_unique_vertices( section_inspector )

    if nb_issues != check_invalid_components_topology_unique_vertices( section_inspector ):
        raise ValueError( "[Test] Wrong number of invalid checks." )
    nb_issues += check_components_linking( section_inspector )
    nb_issues += check_unique_vertices_colocation( section_inspector )
    return nb_issues

def check_vertical_lines_vertices_topology():
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../../../tests/data"))
    model_section = opengeode.load_section( data_dir + "/vertical_lines.og_sctn" )
    section_inspector = inspector.SectionInspector( model_section )
    if section_inspector.section_topology_is_valid():
        print( "model vertical_lines topology is valid." )
    else:
        print( "model vertical_lines topology is invalid." )
    nb_issues = launch_topological_validity_checks( section_inspector )
    if nb_issues != 0:
        raise ValueError( "[Test] There should be no issues with vertical_lines.og_sctn" )

if __name__ == '__main__':
    check_vertical_lines_vertices_topology()
