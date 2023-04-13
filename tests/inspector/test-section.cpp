/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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
#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/section_input.h>

#include <geode/inspector/section_inspector.h>

geode::index_t check_components_linking(
    geode::SectionInspector& section_inspector )
{
    const auto components_are_linked =
        section_inspector
            .section_meshed_components_are_linked_to_a_unique_vertex();
    geode::Logger::info( "Section components are ",
        components_are_linked ? "" : "not ", "all linked to a unique vertex." );
    const auto nb_unlinked_corners =
        section_inspector.nb_corners_not_linked_to_a_unique_vertex();
    geode::Logger::info( "There are ", nb_unlinked_corners,
        " corners not linked to a unique vertex." );
    const auto nb_unlinked_lines =
        section_inspector.nb_lines_meshed_but_not_linked_to_a_unique_vertex();
    geode::Logger::info( "There are ", nb_unlinked_lines,
        " lines not linked to a unique vertex." );
    const auto nb_unlinked_surfaces =
        section_inspector
            .nb_surfaces_meshed_but_not_linked_to_a_unique_vertex();
    geode::Logger::info( "There are ", nb_unlinked_surfaces,
        " surfaces not linked to a unique vertex." );
    const auto nb_unlinked_uv =
        section_inspector.nb_unique_vertices_not_linked_to_a_component_vertex();
    geode::Logger::info( "There are ", nb_unlinked_uv,
        " unique vertices not linked to a component mesh vertex." );
    return nb_unlinked_corners + nb_unlinked_lines + nb_unlinked_surfaces
           + nb_unlinked_uv;
}

geode::index_t check_unique_vertices_colocation(
    geode::SectionInspector& section_inspector )
{
    const auto nb_unique_vertices_linked_to_different_points =
        section_inspector.nb_unique_vertices_linked_to_different_points();
    geode::Logger::info( "There are ",
        nb_unique_vertices_linked_to_different_points,
        " vertices linked to different points in space." );
    const auto nb_colocated_unique_vertices =
        section_inspector.nb_colocated_unique_vertices();
    geode::Logger::info( "There are ", nb_colocated_unique_vertices,
        " unique vertices which are colocated in space." );
    return nb_colocated_unique_vertices
           + nb_unique_vertices_linked_to_different_points;
}

geode::index_t check_invalid_components_topology_unique_vertices(
    geode::SectionInspector& section_inspector )
{
    const auto invalid_components_unique_vertices =
        section_inspector.invalid_components_topology_unique_vertices();
    geode::Logger::info( "There are ",
        invalid_components_unique_vertices.size(),
        " vertices with invalid components." );
    for( const auto vertex_index : invalid_components_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " has invalid components." );
    }
    return invalid_components_unique_vertices.size();
}

geode::index_t check_multiple_corners_unique_vertices(
    geode::SectionInspector& section_inspector )
{
    const auto multiple_corners_unique_vertices =
        section_inspector.multiple_corners_unique_vertices();
    geode::Logger::info( "There are ", multiple_corners_unique_vertices.size(),
        " vertices with multiple corners." );
    for( const auto vertex_index : multiple_corners_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " is associated to multiple corners." );
    }
    return multiple_corners_unique_vertices.size();
}

geode::index_t check_multiple_internals_corner_vertices(
    geode::SectionInspector& section_inspector )
{
    const auto multiple_internals_corner_vertices =
        section_inspector.multiple_internals_corner_vertices();
    geode::Logger::info( "There are ",
        multiple_internals_corner_vertices.size(),
        " vertices with multiple internals." );
    for( const auto vertex_index : multiple_internals_corner_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " is a corner associated with multiple embeddings." );
    }
    return multiple_internals_corner_vertices.size();
}

geode::index_t check_not_internal_nor_boundary_corner_vertices(
    geode::SectionInspector& section_inspector )
{
    const auto not_internal_nor_boundary_corner_vertices =
        section_inspector.not_internal_nor_boundary_corner_vertices();
    geode::Logger::info( "There are ",
        not_internal_nor_boundary_corner_vertices.size(),
        " corner vertices with no boundary nor internal property." );
    for( const auto vertex_index : not_internal_nor_boundary_corner_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " is neither internal nor a boundary." );
    }
    return not_internal_nor_boundary_corner_vertices.size();
}

geode::index_t check_line_corners_without_boundary_status(
    geode::SectionInspector& section_inspector )
{
    const auto line_corners_without_boundary_status =
        section_inspector.line_corners_without_boundary_status();
    geode::Logger::info( "There are ",
        line_corners_without_boundary_status.size(),
        " corner vertices part of a line but not its boundary." );
    for( const auto vertex_index : line_corners_without_boundary_status )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index,
            " is a corner but has a line for which it is not a boundary." );
    }
    return line_corners_without_boundary_status.size();
}

geode::index_t check_part_of_not_boundary_nor_internal_line_unique_vertices(
    geode::SectionInspector& section_inspector )
{
    const auto part_of_not_boundary_nor_internal_line_unique_vertices =
        section_inspector
            .part_of_not_boundary_nor_internal_line_unique_vertices();
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
    return part_of_not_boundary_nor_internal_line_unique_vertices.size();
}

geode::index_t
    check_part_of_line_with_invalid_internal_topology_unique_vertices(
        geode::SectionInspector& section_inspector )
{
    const auto part_of_line_with_invalid_internal_topology_unique_vertices =
        section_inspector
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
    return part_of_line_with_invalid_internal_topology_unique_vertices.size();
}

geode::index_t check_part_of_invalid_unique_line_unique_vertices(
    geode::SectionInspector& section_inspector )
{
    const auto part_of_invalid_unique_line_unique_vertices =
        section_inspector.part_of_invalid_unique_line_unique_vertices();
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
    return part_of_invalid_unique_line_unique_vertices.size();
}

geode::index_t check_part_of_lines_but_not_corner_unique_vertices(
    geode::SectionInspector& section_inspector )
{
    const auto part_of_lines_but_not_corner_unique_vertices =
        section_inspector.part_of_lines_but_not_corner_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_lines_but_not_corner_unique_vertices.size(),
        " vertices part of multiple lines but not corner." );
    for( const auto vertex_index :
        part_of_lines_but_not_corner_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " is part of multiple lines but is not a corner." );
    }
    return part_of_lines_but_not_corner_unique_vertices.size();
}

geode::index_t check_part_of_invalid_surfaces_unique_vertices(
    geode::SectionInspector& section_inspector )
{
    const auto part_of_invalid_surfaces_unique_vertices =
        section_inspector.part_of_invalid_surfaces_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_invalid_surfaces_unique_vertices.size(),
        " vertices with invalid surface topology." );
    for( const auto vertex_index : part_of_invalid_surfaces_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index, " has invalid surfaces topology." );
    }
    return part_of_invalid_surfaces_unique_vertices.size();
}

geode::index_t check_part_of_line_and_not_on_surface_border_unique_vertices(
    geode::SectionInspector& section_inspector )
{
    const auto part_of_line_and_not_on_surface_border_unique_vertices =
        section_inspector
            .part_of_line_and_not_on_surface_border_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_line_and_not_on_surface_border_unique_vertices.size(),
        " vertices part of a line and a surface and not on the border of the "
        "surface mesh." );
    for( const auto vertex_index :
        part_of_line_and_not_on_surface_border_unique_vertices )
    {
        geode::Logger::warn( "[Test] Model unique vertex with index ",
            vertex_index,
            " is part of a line and a surface but is not on the border of the "
            "surface mesh." );
    }
    return part_of_line_and_not_on_surface_border_unique_vertices.size();
}

geode::index_t launch_topological_validity_checks(
    geode::SectionInspector& section_inspector )
{
    geode::index_t nb_errors{ 0 };
    nb_errors += check_multiple_corners_unique_vertices( section_inspector );
    nb_errors += check_multiple_internals_corner_vertices( section_inspector );
    nb_errors +=
        check_not_internal_nor_boundary_corner_vertices( section_inspector );
    nb_errors +=
        check_line_corners_without_boundary_status( section_inspector );
    nb_errors += check_part_of_not_boundary_nor_internal_line_unique_vertices(
        section_inspector );
    nb_errors +=
        check_part_of_invalid_unique_line_unique_vertices( section_inspector );
    nb_errors +=
        check_part_of_lines_but_not_corner_unique_vertices( section_inspector );
    nb_errors +=
        check_part_of_invalid_surfaces_unique_vertices( section_inspector );
    nb_errors += check_part_of_line_and_not_on_surface_border_unique_vertices(
        section_inspector );
    nb_errors +=
        check_part_of_line_with_invalid_internal_topology_unique_vertices(
            section_inspector );

    OPENGEODE_EXCEPTION(
        nb_errors
            == check_invalid_components_topology_unique_vertices(
                section_inspector ),
        "[Test] Did not find as many components topology errors." );
    nb_errors += check_components_linking( section_inspector );
    nb_errors += check_unique_vertices_colocation( section_inspector );
    return nb_errors;
}

geode::index_t check_components_adjacency(
    geode::SectionInspector& section_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto surfaces_wrong_adjacencies =
        section_inspector.surfaces_nb_edges_with_wrong_adjacencies();
    if( surfaces_wrong_adjacencies.empty() )
    {
        geode::Logger::info(
            "Section surfaces meshes have no adjacency problems." );
    }
    for( const auto& comp_wrong_adj : surfaces_wrong_adjacencies )
    {
        geode::Logger::info( "Mesh of surface with uuid ",
            comp_wrong_adj.first.string(), " has ", comp_wrong_adj.second,
            " edges with adjacency problems." );
        nb_issues += comp_wrong_adj.second;
    }
    return nb_issues;
}

geode::index_t check_components_colocation(
    geode::SectionInspector& section_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto components_colocated_pts =
        section_inspector.components_nb_colocated_points();
    if( components_colocated_pts.empty() )
    {
        geode::Logger::info(
            "Section component meshes have no colocated points." );
    }
    for( const auto& colocated : components_colocated_pts )
    {
        geode::Logger::info( "Mesh of component with uuid ",
            colocated.first.string(), " has ", colocated.second,
            " colocated points." );
        nb_issues += colocated.second;
    }
    return nb_issues;
}

geode::index_t check_components_degeneration(
    geode::SectionInspector& section_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto components_degenerated_edges =
        section_inspector.components_nb_degenerated_elements();
    if( components_degenerated_edges.empty() )
    {
        geode::Logger::info( "Section component meshes are not degenerated." );
    }
    for( const auto& degenerated : components_degenerated_edges )
    {
        geode::Logger::info( "Mesh of component with uuid ",
            degenerated.first.string(), " has ", degenerated.second,
            " degenerated edges." );
        nb_issues += degenerated.second;
    }
    return nb_issues;
}

geode::index_t check_components_manifold(
    geode::SectionInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto components_nb_non_manifold_vertices =
        brep_inspector.component_meshes_nb_non_manifold_vertices();
    const auto components_nb_non_manifold_edges =
        brep_inspector.component_meshes_nb_non_manifold_edges();
    if( components_nb_non_manifold_vertices.empty()
        && components_nb_non_manifold_edges.empty() )
    {
        geode::Logger::info( "Section component meshes are manifold." );
    }
    for( const auto& non_manifold_vertices :
        components_nb_non_manifold_vertices )
    {
        geode::Logger::info( "Mesh of surface with uuid ",
            non_manifold_vertices.first.string(), " has ",
            non_manifold_vertices.second, " non manifold vertices." );
        nb_issues += non_manifold_vertices.second;
    }
    for( const auto& non_manifold_edges : components_nb_non_manifold_edges )
    {
        geode::Logger::info( "Mesh of surface with uuid ",
            non_manifold_edges.first.string(), " has ",
            non_manifold_edges.second, " non manifold edges." );
        nb_issues += non_manifold_edges.second;
    }
    return nb_issues;
}

geode::index_t check_components_intersections(
    geode::SectionInspector& section_inspector )
{
    const auto nb_surfaces_intersections =
        section_inspector.nb_intersecting_surfaces_elements_pair();
    if( nb_surfaces_intersections == 0 )
    {
        geode::Logger::info( "Section meshes have no intersection problems." );
    }
    else
    {
        geode::Logger::info( "There are ", nb_surfaces_intersections,
            " pairs of intersecting triangles in the Section." );
    }
    return nb_surfaces_intersections;
}

geode::index_t launch_component_meshes_validity_checks(
    geode::SectionInspector& section_inspector )
{
    auto nb_issues = check_components_adjacency( section_inspector );
    nb_issues += check_components_colocation( section_inspector );
    nb_issues += check_components_degeneration( section_inspector );
    nb_issues += check_components_manifold( section_inspector );
    nb_issues += check_components_intersections( section_inspector );
    return nb_issues;
}

void check_section()
{
    const auto model_section = geode::load_section(
        absl::StrCat( geode::data_path, "vertical_lines.og_sctn" ) );
    geode::SectionInspector section_inspector{ model_section };
    geode::Logger::info( "vertical_lines section topology is ",
        section_inspector.section_topology_is_valid() ? "valid." : "invalid." );
    const auto nb_invalids =
        launch_topological_validity_checks( section_inspector );
    OPENGEODE_EXCEPTION( nb_invalids == 0,
        "[Test] Model is supposed to be valid but is shown as invalid." );
    const auto nb_mesh_invalids =
        launch_component_meshes_validity_checks( section_inspector );
    OPENGEODE_EXCEPTION( nb_mesh_invalids == 0,
        "[Test] Model component meshes are supposed to "
        "be valid but are shown as invalid." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_section();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
