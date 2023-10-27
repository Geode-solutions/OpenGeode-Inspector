import os
import opengeode
import opengeode_inspector as inspector
import opengeode_io

def check_components_linking( brep_inspector ):
    nb_unlinked_corners = brep_inspector.nb_corners_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_corners, " corners not linked to a unique vertex." )
    nb_unlinked_lines = brep_inspector.nb_lines_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_lines, " lines not linked to a unique vertex." )
    nb_unlinked_surfaces = brep_inspector.nb_surfaces_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_surfaces, " surfaces not linked to a unique vertex." )
    nb_unlinked_blocks = brep_inspector.nb_blocks_not_linked_to_a_unique_vertex()
    print( "There are ", nb_unlinked_blocks, " blocks not linked to a unique vertex." )

def check_invalid_components_topology_unique_vertices( brep_inspector ):
    invalid_components_unique_vertices = brep_inspector.invalid_components_topology_unique_vertices()
    print( "There are ", len( invalid_components_unique_vertices ), " vertices with invalid components." )
    for vertex_index in invalid_components_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " has invalid components." )

def check_unique_vertices_linked_to_multiple_corners( brep_inspector ):
    unique_vertices_linked_to_multiple_corners = brep_inspector.unique_vertices_linked_to_multiple_corners()
    print( "There are ", len( unique_vertices_linked_to_multiple_corners ), " vertices with multiple corners." )
    for vertex_index in unique_vertices_linked_to_multiple_corners:
        print( "[Test] Model unique vertex with index ", vertex_index, " is associated to multiple corners." )

def check_unique_vertices_linked_to_multiple_internals_corner( brep_inspector ):
    unique_vertices_linked_to_multiple_internals_corner = brep_inspector.unique_vertices_linked_to_multiple_internals_corner()
    print( "There are ", len( unique_vertices_linked_to_multiple_internals_corner ), " vertices with multiple internals." )
    for vertex_index in unique_vertices_linked_to_multiple_internals_corner:
        print( "[Test] Model unique vertex with index ", vertex_index, " is a corner associated with multiple embeddings." )

def check_unique_vertices_linked_to_not_internal_nor_boundary_corner( brep_inspector ):
    unique_vertices_linked_to_not_internal_nor_boundary_corner = brep_inspector.unique_vertices_linked_to_not_internal_nor_boundary_corner()
    print( "There are ", len( unique_vertices_linked_to_not_internal_nor_boundary_corner ), " corner vertices with no boundary nor internal property." )
    for vertex_index in unique_vertices_linked_to_not_internal_nor_boundary_corner:
        print( "[Test] Model unique vertex with index ", vertex_index, " is neither internal nor a boundary." )

def check_unique_vertices_liked_to_not_boundary_line_corner( brep_inspector ):
    unique_vertices_liked_to_not_boundary_line_corner = brep_inspector.unique_vertices_liked_to_not_boundary_line_corner()
    print( "There are ", len( unique_vertices_liked_to_not_boundary_line_corner ), " corner vertices part of a line but not its boundary." )
    for vertex_index in unique_vertices_liked_to_not_boundary_line_corner:
        print( "[Test] Model unique vertex with index ", vertex_index, " is a corner but has a line for which it is not a boundary." )

def check_unique_vertices_linked_to_not_internal_nor_boundary_line( brep_inspector ):
    unique_vertices_linked_to_not_internal_nor_boundary_line = brep_inspector.unique_vertices_linked_to_not_internal_nor_boundary_line()
    print( "There are ", len( unique_vertices_linked_to_not_internal_nor_boundary_line ), " vertices part of a line which is not boundary not internal." )
    for vertex_index in unique_vertices_linked_to_not_internal_nor_boundary_line:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a line which is neither boundary nor internal." )

def check_unique_vertices_linked_to_a_line_with_invalid_embeddings( brep_inspector ):
    unique_vertices_linked_to_a_line_with_invalid_embeddings = brep_inspector.unique_vertices_linked_to_a_line_with_invalid_embeddings()
    print( "There are ", len( unique_vertices_linked_to_a_line_with_invalid_embeddings ), " vertices part of lines with invalid internal property." )
    for vertex_index in unique_vertices_linked_to_a_line_with_invalid_embeddings:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a line with invalid internal properties." )

def check_unique_vertices_linked_to_a_single_and_invalid_line( brep_inspector ):
    unique_vertices_linked_to_a_single_and_invalid_line = brep_inspector.unique_vertices_linked_to_a_single_and_invalid_line()
    print( "There are ", len( unique_vertices_linked_to_a_single_and_invalid_line ), " vertices part of a unique line with invalid toplogy." )
    for vertex_index in unique_vertices_linked_to_a_single_and_invalid_line:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a unique line with invalid topological properties." )

def check_unique_vertices_linked_to_a_line_but_not_linked_to_a_corner( brep_inspector ):
    unique_vertices_linked_to_a_line_but_not_linked_to_a_corner = brep_inspector.unique_vertices_linked_to_a_line_but_not_linked_to_a_corner()
    print( "There are ", len( unique_vertices_linked_to_a_line_but_not_linked_to_a_corner ), " vertices part of multiple lines but not corner." )
    for vertex_index in unique_vertices_linked_to_a_line_but_not_linked_to_a_corner:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of multiple lines but is not a corner." )

def check_part_of_not_boundary_nor_internal_surface_unique_vertices( brep_inspector ):
    part_of_not_boundary_nor_internal_surface_unique_vertices = brep_inspector.part_of_not_boundary_nor_internal_surface_unique_vertices()
    print( "There are ", len( part_of_not_boundary_nor_internal_surface_unique_vertices ), " vertices part of a surface which is neither internal nor boundary." )
    for vertex_index in part_of_not_boundary_nor_internal_surface_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a surface which is neither internal nor boundary." )

def check_part_of_surface_with_invalid_internal_topology_unique_vertices( brep_inspector ):
    part_of_surface_with_invalid_internal_topology_unique_vertices = brep_inspector.part_of_surface_with_invalid_internal_topology_unique_vertices()
    print( "There are ", len( part_of_surface_with_invalid_internal_topology_unique_vertices ), " vertices part of a surface with invalid internal topology." )
    for vertex_index in part_of_surface_with_invalid_internal_topology_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a surface with invalid internal topology." )

def check_part_of_invalid_unique_surface_unique_vertices( brep_inspector ):
    part_of_invalid_unique_surface_unique_vertices = brep_inspector.part_of_invalid_unique_surface_unique_vertices()
    print( "There are ", len( part_of_invalid_unique_surface_unique_vertices ), " vertices part of a unique surface with invalid topology." )
    for vertex_index in part_of_invalid_unique_surface_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of a unique surface with invalid topology." )

def check_part_of_invalid_multiple_surfaces_unique_vertices( brep_inspector ):
    part_of_invalid_multiple_surfaces_unique_vertices = brep_inspector.part_of_invalid_multiple_surfaces_unique_vertices()
    print( "There are ", len( part_of_invalid_multiple_surfaces_unique_vertices ), " vertices part of invalid multiple surfaces." )
    for vertex_index in part_of_invalid_multiple_surfaces_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " is part of invalid multiple surfaces." )

def check_part_of_invalid_blocks_unique_vertices( brep_inspector ):
    part_of_invalid_blocks_unique_vertices = brep_inspector.part_of_invalid_blocks_unique_vertices()
    print( "There are ", len( part_of_invalid_blocks_unique_vertices ), " vertices with invalid block topology." )
    for vertex_index in part_of_invalid_blocks_unique_vertices:
        print( "[Test] Model unique vertex with index ", vertex_index, " has invalid blocks topology." )

def launch_topological_validity_checks( brep_inspector ):
    check_components_linking( brep_inspector )
    check_invalid_components_topology_unique_vertices( brep_inspector )
    check_unique_vertices_linked_to_multiple_corners( brep_inspector )
    check_unique_vertices_linked_to_multiple_internals_corner( brep_inspector )
    check_unique_vertices_linked_to_not_internal_nor_boundary_corner( brep_inspector )
    check_unique_vertices_liked_to_not_boundary_line_corner( brep_inspector )
    check_unique_vertices_linked_to_not_internal_nor_boundary_line( brep_inspector )
    check_unique_vertices_linked_to_a_single_and_invalid_line( brep_inspector )
    check_unique_vertices_linked_to_a_single_and_invalid_line( brep_inspector )
    check_unique_vertices_linked_to_a_line_but_not_linked_to_a_corner( brep_inspector )
    check_part_of_not_boundary_nor_internal_surface_unique_vertices( brep_inspector )
    check_part_of_surface_with_invalid_internal_topology_unique_vertices( brep_inspector )
    check_part_of_invalid_unique_surface_unique_vertices( brep_inspector )
    check_part_of_invalid_multiple_surfaces_unique_vertices( brep_inspector )
    check_part_of_invalid_blocks_unique_vertices( brep_inspector )

if __name__ == '__main__':
    test_dir = os.path.dirname(__file__)
    data_dir = os.path.abspath(os.path.join(test_dir, "../tests/data"))
    brep = opengeode.load_brep( data_dir + "/model_A1_valid.og_brep" )
    brep_inspector = inspector.BRepTopologyInspector(brep)
    if brep_inspector.brep_topology_is_valid():
        print( "Model topology is valid." )
    else:
        print( "Model topology is invalid." )
    launch_topological_validity_checks( brep_inspector )