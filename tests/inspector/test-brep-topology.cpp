/*
 * Copyright (c) 2019 - 2022 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#include <geode/tests_config.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/model/helpers/convert_model_meshes.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>

#include <geode/inspector/topology/brep_topology.h>

void check_components_linking( geode::BRepTopologyInspector& brep_inspector )
{
    const auto nb_unlinked_corners =
        brep_inspector.nb_corners_not_linked_to_a_unique_vertex();
    geode::Logger::info( "There are ", nb_unlinked_corners,
        " corners not linked to a unique vertex." );
    const auto nb_unlinked_lines =
        brep_inspector.nb_lines_not_linked_to_a_unique_vertex();
    geode::Logger::info( "There are ", nb_unlinked_lines,
        " lines not linked to a unique vertex." );
    const auto nb_unlinked_surfaces =
        brep_inspector.nb_surfaces_not_linked_to_a_unique_vertex();
    geode::Logger::info( "There are ", nb_unlinked_surfaces,
        " surfaces not linked to a unique vertex." );
    const auto nb_unlinked_blocks =
        brep_inspector.nb_blocks_not_linked_to_a_unique_vertex();
    geode::Logger::info( "There are ", nb_unlinked_blocks,
        " blocks not linked to a unique vertex." );
}

void check_invalid_components_topology_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto invalid_components_unique_vertices =
        brep_inspector.invalid_components_topology_unique_vertices();
    geode::Logger::info( "There are ",
        invalid_components_unique_vertices.size(),
        " vertices with invalid components." );
    for( const auto vertex_index : invalid_components_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " has invalid components." );
    }
}

void check_multiple_corners_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto multiple_corners_unique_vertices =
        brep_inspector.multiple_corners_unique_vertices();
    geode::Logger::info( "There are ", multiple_corners_unique_vertices.size(),
        " vertices with multiple corners." );
    for( const auto vertex_index : multiple_corners_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " is associated to multiple corners." );
    }
}

void check_multiple_internals_corner_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto multiple_internals_corner_vertices =
        brep_inspector.multiple_internals_corner_vertices();
    geode::Logger::info( "There are ",
        multiple_internals_corner_vertices.size(),
        " vertices with multiple internals." );
    for( const auto vertex_index : multiple_internals_corner_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " is a corner associated with multiple embeddings." );
    }
}

void check_not_internal_nor_boundary_corner_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto not_internal_nor_boundary_corner_vertices =
        brep_inspector.not_internal_nor_boundary_corner_vertices();
    geode::Logger::info( "There are ",
        not_internal_nor_boundary_corner_vertices.size(),
        " corner vertices with no boundary nor internal property." );
    for( const auto vertex_index : not_internal_nor_boundary_corner_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " is neither internal nor a boundary." );
    }
}

void check_line_corners_without_boundary_status(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto line_corners_without_boundary_status =
        brep_inspector.line_corners_without_boundary_status();
    geode::Logger::info( "There are ",
        line_corners_without_boundary_status.size(),
        " corner vertices part of a line but not its boundary." );
    for( const auto vertex_index : line_corners_without_boundary_status )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index,
            " is a corner but has a line for which it is not a boundary." );
    }
}

void check_part_of_not_boundary_nor_internal_line_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto part_of_not_boundary_nor_internal_line_unique_vertices =
        brep_inspector.part_of_not_boundary_nor_internal_line_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_not_boundary_nor_internal_line_unique_vertices.size(),
        " vertices part of a line which is not boundary not internal." );
    for( const auto vertex_index :
        part_of_not_boundary_nor_internal_line_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index,
            " is part of a line which is neither boundary nor internal." );
    }
}

void check_part_of_line_with_invalid_internal_topology_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto part_of_line_with_invalid_internal_topology_unique_vertices =
        brep_inspector
            .part_of_line_with_invalid_internal_topology_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_line_with_invalid_internal_topology_unique_vertices.size(),
        " vertices part of lines with invalid internal property." );
    for( const auto vertex_index :
        part_of_line_with_invalid_internal_topology_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index,
            " is part of a line with invalid internal properties." );
    }
}

void check_part_of_invalid_unique_line_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto part_of_invalid_unique_line_unique_vertices =
        brep_inspector.part_of_invalid_unique_line_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_invalid_unique_line_unique_vertices.size(),
        " vertices part of a unique line with invalid toplogy." );
    for( const auto vertex_index : part_of_invalid_unique_line_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index,
            " is part of a unique line with invalid topological "
            "properties." );
    }
}

void check_part_of_lines_but_not_corner_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto part_of_lines_but_not_corner_unique_vertices =
        brep_inspector.part_of_lines_but_not_corner_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_lines_but_not_corner_unique_vertices.size(),
        " vertices part of multiple lines but not corner." );
    for( const auto vertex_index :
        part_of_lines_but_not_corner_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " is part of multiple lines but is not a corner." );
    }
}

void check_part_of_not_boundary_nor_internal_surface_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto part_of_not_boundary_nor_internal_surface_unique_vertices =
        brep_inspector
            .part_of_not_boundary_nor_internal_surface_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_not_boundary_nor_internal_surface_unique_vertices.size(),
        " vertices part of a surface which is neither internal nor boundary." );
    for( const auto vertex_index :
        part_of_not_boundary_nor_internal_surface_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index,
            " is part of a surface which is neither internal nor boundary." );
    }
}

void check_part_of_surface_with_invalid_internal_topology_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto part_of_surface_with_invalid_internal_topology_unique_vertices =
        brep_inspector
            .part_of_surface_with_invalid_internal_topology_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_surface_with_invalid_internal_topology_unique_vertices.size(),
        " vertices part of a surface with invalid internal topology." );
    for( const auto vertex_index :
        part_of_surface_with_invalid_internal_topology_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index,
            " is part of a surface with invalid internal topology." );
    }
}

void check_part_of_invalid_unique_surface_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto part_of_invalid_unique_surface_unique_vertices =
        brep_inspector.part_of_invalid_unique_surface_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_invalid_unique_surface_unique_vertices.size(),
        " vertices part of a unique surface with invalid topology." );
    for( const auto vertex_index :
        part_of_invalid_unique_surface_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index,
            " is part of a unique surface with invalid topology." );
    }
}

void check_part_of_invalid_multiple_surfaces_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto part_of_invalid_multiple_surfaces_unique_vertices =
        brep_inspector.part_of_invalid_multiple_surfaces_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_invalid_multiple_surfaces_unique_vertices.size(),
        " vertices part of invalid multiple surfaces." );
    for( const auto vertex_index :
        part_of_invalid_multiple_surfaces_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " is part of invalid multiple surfaces." );
    }
}

void check_part_of_invalid_blocks_unique_vertices(
    geode::BRepTopologyInspector& brep_inspector )
{
    const auto part_of_invalid_blocks_unique_vertices =
        brep_inspector.part_of_invalid_blocks_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_invalid_blocks_unique_vertices.size(),
        " vertices with invalid block topology." );
    for( const auto vertex_index : part_of_invalid_blocks_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " has invalid blocks topology." );
    }
}

void launch_topological_validity_checks(
    geode::BRepTopologyInspector& brep_inspector )
{
    check_components_linking( brep_inspector );
    check_invalid_components_topology_unique_vertices( brep_inspector );
    check_multiple_corners_unique_vertices( brep_inspector );
    check_multiple_internals_corner_vertices( brep_inspector );
    check_not_internal_nor_boundary_corner_vertices( brep_inspector );
    check_line_corners_without_boundary_status( brep_inspector );
    check_part_of_not_boundary_nor_internal_line_unique_vertices(
        brep_inspector );
    check_part_of_invalid_unique_line_unique_vertices( brep_inspector );
    check_part_of_invalid_unique_line_unique_vertices( brep_inspector );
    check_part_of_lines_but_not_corner_unique_vertices( brep_inspector );
    check_part_of_not_boundary_nor_internal_surface_unique_vertices(
        brep_inspector );
    check_part_of_surface_with_invalid_internal_topology_unique_vertices(
        brep_inspector );
    check_part_of_invalid_unique_surface_unique_vertices( brep_inspector );
    check_part_of_invalid_multiple_surfaces_unique_vertices( brep_inspector );
    check_part_of_invalid_blocks_unique_vertices( brep_inspector );
}

void check_a1_vertices_topology()
{
    const auto model_brep = geode::load_brep(
        absl::StrCat( geode::data_path, "model_A1.og_brep" ) );
    geode::BRepTopologyInspector brep_inspector{ model_brep };
    geode::Logger::info( "model_A1 topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );
    launch_topological_validity_checks( brep_inspector );
}

void check_a1_valid_vertices_topology()
{
    const auto model_brep = geode::load_brep(
        absl::StrCat( geode::data_path, "model_A1_valid.og_brep" ) );
    geode::BRepTopologyInspector brep_inspector{ model_brep };
    geode::Logger::info( "model_A1_valid topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );
    launch_topological_validity_checks( brep_inspector );
}

int main()
{
    try
    {
        check_a1_vertices_topology();
        check_a1_valid_vertices_topology();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
