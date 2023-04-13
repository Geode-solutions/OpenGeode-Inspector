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
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>

#include <geode/geosciences/explicit/common.h>
#include <geode/geosciences/explicit/representation/core/structural_model.h>
#include <geode/geosciences/explicit/representation/io/structural_model_input.h>

#include <geode/inspector/brep_inspector.h>

geode::index_t check_components_linking( geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto components_are_linked =
        brep_inspector.brep_meshed_components_are_linked_to_a_unique_vertex();
    geode::Logger::info( "BRep components are ",
        components_are_linked ? "" : "not ", "all linked to a unique vertex." );
    const auto nb_unlinked_corners =
        brep_inspector.nb_corners_not_linked_to_a_unique_vertex();
    nb_issues += nb_unlinked_corners;
    geode::Logger::info( "There are ", nb_unlinked_corners,
        " corners not linked to a unique vertex." );
    const auto nb_unlinked_lines =
        brep_inspector.nb_lines_meshed_but_not_linked_to_a_unique_vertex();
    nb_issues += nb_unlinked_lines;
    geode::Logger::info( "There are ", nb_unlinked_lines,
        " lines not linked to a unique vertex." );
    const auto nb_unlinked_surfaces =
        brep_inspector.nb_surfaces_meshed_but_not_linked_to_a_unique_vertex();
    nb_issues += nb_unlinked_surfaces;
    geode::Logger::info( "There are ", nb_unlinked_surfaces,
        " surfaces not linked to a unique vertex." );
    const auto nb_unlinked_blocks =
        brep_inspector.nb_blocks_meshed_but_not_linked_to_a_unique_vertex();
    nb_issues += nb_unlinked_blocks;
    geode::Logger::info( "There are ", nb_unlinked_blocks,
        " blocks not linked to a unique vertex." );
    const auto nb_unlinked_uv =
        brep_inspector.nb_unique_vertices_not_linked_to_a_component_vertex();
    nb_issues += nb_unlinked_uv;
    geode::Logger::info( "There are ", nb_unlinked_uv,
        " unique vertices not linked to a component mesh vertex." );
    return nb_issues;
}

geode::index_t check_unique_vertices_colocation(
    geode::BRepInspector& brep_inspector )
{
    const auto nb_unique_vertices_linked_to_different_points =
        brep_inspector.nb_unique_vertices_linked_to_different_points();
    geode::Logger::info( "There are ",
        nb_unique_vertices_linked_to_different_points,
        " vertices linked to different points in space." );
    const auto nb_colocated_unique_vertices =
        brep_inspector.nb_colocated_unique_vertices();
    geode::Logger::info( "There are ", nb_colocated_unique_vertices,
        " unique vertices which are colocated in space." );
    return nb_colocated_unique_vertices
           + nb_unique_vertices_linked_to_different_points;
}

geode::index_t check_invalid_components_topology_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto invalid_components_unique_vertices =
        brep_inspector.invalid_components_topology_unique_vertices();
    geode::Logger::info( "There are ",
        invalid_components_unique_vertices.size(),
        " vertices with invalid components." );
    nb_issues += invalid_components_unique_vertices.size();
    return nb_issues;
}

geode::index_t check_multiple_corners_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto multiple_corners_unique_vertices =
        brep_inspector.multiple_corners_unique_vertices();
    geode::Logger::info( "There are ", multiple_corners_unique_vertices.size(),
        " vertices with multiple corners." );
    nb_issues += multiple_corners_unique_vertices.size();
    return nb_issues;
}

geode::index_t check_multiple_internals_corner_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto multiple_internals_corner_vertices =
        brep_inspector.multiple_internals_corner_vertices();
    geode::Logger::info( "There are ",
        multiple_internals_corner_vertices.size(),
        " vertices with multiple internals." );
    nb_issues += multiple_internals_corner_vertices.size();
    return nb_issues;
}

geode::index_t check_not_internal_nor_boundary_corner_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto not_internal_nor_boundary_corner_vertices =
        brep_inspector.not_internal_nor_boundary_corner_vertices();
    geode::Logger::info( "There are ",
        not_internal_nor_boundary_corner_vertices.size(),
        " corner vertices with no boundary nor internal property." );
    nb_issues += not_internal_nor_boundary_corner_vertices.size();
    return nb_issues;
}

geode::index_t check_line_corners_without_boundary_status(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto line_corners_without_boundary_status =
        brep_inspector.line_corners_without_boundary_status();
    geode::Logger::info( "There are ",
        line_corners_without_boundary_status.size(),
        " corner vertices part of a line but not its boundary." );
    nb_issues += line_corners_without_boundary_status.size();
    return nb_issues;
}

geode::index_t check_part_of_not_boundary_nor_internal_line_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_not_boundary_nor_internal_line_unique_vertices =
        brep_inspector.part_of_not_boundary_nor_internal_line_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_not_boundary_nor_internal_line_unique_vertices.size(),
        " vertices part of a line which is not boundary not internal." );
    nb_issues += part_of_not_boundary_nor_internal_line_unique_vertices.size();
    return nb_issues;
}

geode::index_t
    check_part_of_line_with_invalid_internal_topology_unique_vertices(
        geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_line_with_invalid_internal_topology_unique_vertices =
        brep_inspector
            .part_of_line_with_invalid_internal_topology_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_line_with_invalid_internal_topology_unique_vertices.size(),
        " vertices part of lines with invalid internal property." );
    nb_issues +=
        part_of_line_with_invalid_internal_topology_unique_vertices.size();
    return nb_issues;
}

geode::index_t check_part_of_invalid_unique_line_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_invalid_unique_line_unique_vertices =
        brep_inspector.part_of_invalid_unique_line_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_invalid_unique_line_unique_vertices.size(),
        " vertices part of a unique line with invalid toplogy." );
    nb_issues += part_of_invalid_unique_line_unique_vertices.size();
    return nb_issues;
}

geode::index_t check_part_of_lines_but_not_corner_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_lines_but_not_corner_unique_vertices =
        brep_inspector.part_of_lines_but_not_corner_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_lines_but_not_corner_unique_vertices.size(),
        " vertices part of multiple lines but not corner." );
    nb_issues += part_of_lines_but_not_corner_unique_vertices.size();
    return nb_issues;
}

geode::index_t check_part_of_not_boundary_nor_internal_surface_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_not_boundary_nor_internal_surface_unique_vertices =
        brep_inspector
            .part_of_not_boundary_nor_internal_surface_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_not_boundary_nor_internal_surface_unique_vertices.size(),
        " vertices part of a surface which is neither internal nor boundary." );
    nb_issues +=
        part_of_not_boundary_nor_internal_surface_unique_vertices.size();
    return nb_issues;
}

geode::index_t
    check_part_of_surface_with_invalid_internal_topology_unique_vertices(
        geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_surface_with_invalid_internal_topology_unique_vertices =
        brep_inspector
            .part_of_surface_with_invalid_internal_topology_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_surface_with_invalid_internal_topology_unique_vertices.size(),
        " vertices part of a surface with invalid internal topology." );
    nb_issues +=
        part_of_surface_with_invalid_internal_topology_unique_vertices.size();
    return nb_issues;
}

geode::index_t check_part_of_invalid_unique_surface_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_invalid_unique_surface_unique_vertices =
        brep_inspector.part_of_invalid_unique_surface_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_invalid_unique_surface_unique_vertices.size(),
        " vertices part of a unique surface with invalid topology." );
    nb_issues += part_of_invalid_unique_surface_unique_vertices.size();
    return nb_issues;
}

geode::index_t check_part_of_invalid_multiple_surfaces_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_invalid_multiple_surfaces_unique_vertices =
        brep_inspector.part_of_invalid_multiple_surfaces_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_invalid_multiple_surfaces_unique_vertices.size(),
        " vertices part of invalid multiple surfaces." );
    nb_issues += part_of_invalid_multiple_surfaces_unique_vertices.size();
    return nb_issues;
}

geode::index_t check_part_of_line_and_not_on_surface_border_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_line_and_not_on_surface_border_unique_vertices =
        brep_inspector.part_of_line_and_not_on_surface_border_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_line_and_not_on_surface_border_unique_vertices.size(),
        " vertices part of a line and a surface but not on the surface "
        "border." );
    nb_issues += part_of_line_and_not_on_surface_border_unique_vertices.size();
    return nb_issues;
}

geode::index_t check_part_of_invalid_blocks_unique_vertices(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto part_of_invalid_blocks_unique_vertices =
        brep_inspector.part_of_invalid_blocks_unique_vertices();
    geode::Logger::info( "There are ",
        part_of_invalid_blocks_unique_vertices.size(),
        " vertices with invalid block topology." );
    nb_issues += part_of_invalid_blocks_unique_vertices.size();
    return nb_issues;
}

geode::index_t launch_topological_validity_checks(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    nb_issues += check_multiple_corners_unique_vertices( brep_inspector );
    nb_issues += check_multiple_internals_corner_vertices( brep_inspector );
    nb_issues +=
        check_not_internal_nor_boundary_corner_vertices( brep_inspector );
    nb_issues += check_line_corners_without_boundary_status( brep_inspector );
    nb_issues += check_part_of_not_boundary_nor_internal_line_unique_vertices(
        brep_inspector );
    nb_issues +=
        check_part_of_invalid_unique_line_unique_vertices( brep_inspector );
    nb_issues +=
        check_part_of_lines_but_not_corner_unique_vertices( brep_inspector );
    nb_issues +=
        check_part_of_not_boundary_nor_internal_surface_unique_vertices(
            brep_inspector );
    nb_issues +=
        check_part_of_surface_with_invalid_internal_topology_unique_vertices(
            brep_inspector );
    nb_issues +=
        check_part_of_invalid_unique_surface_unique_vertices( brep_inspector );
    nb_issues += check_part_of_invalid_multiple_surfaces_unique_vertices(
        brep_inspector );
    nb_issues += check_part_of_line_and_not_on_surface_border_unique_vertices(
        brep_inspector );
    nb_issues += check_part_of_invalid_blocks_unique_vertices( brep_inspector );

    OPENGEODE_EXCEPTION(
        nb_issues
            == check_invalid_components_topology_unique_vertices(
                brep_inspector ),
        "[Test] Wrong number of issues." );
    nb_issues += check_components_linking( brep_inspector );
    nb_issues += check_unique_vertices_colocation( brep_inspector );
    return nb_issues;
}

geode::index_t check_components_adjacency(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto surfaces_wrong_adjacencies =
        brep_inspector.surfaces_nb_edges_with_wrong_adjacencies();
    const auto blocks_wrong_adjacencies =
        brep_inspector.blocks_nb_facets_with_wrong_adjacencies();
    if( surfaces_wrong_adjacencies.empty() && blocks_wrong_adjacencies.empty() )
    {
        geode::Logger::info(
            "BRep component meshes have no adjacency problems." );
    }
    for( const auto& comp_wrong_adj : surfaces_wrong_adjacencies )
    {
        geode::Logger::info( "Mesh of surface with uuid ",
            comp_wrong_adj.first.string(), " has ", comp_wrong_adj.second,
            " edges with adjacency problems." );
        nb_issues += comp_wrong_adj.second;
    }
    for( const auto& comp_wrong_adj : blocks_wrong_adjacencies )
    {
        geode::Logger::info( "Mesh of block with uuid ",
            comp_wrong_adj.first.string(), " has ", comp_wrong_adj.second,
            " facets with adjacency problems." );
        nb_issues += comp_wrong_adj.second;
    }
    return nb_issues;
}

geode::index_t check_components_colocation(
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto components_colocated_pts =
        brep_inspector.components_nb_colocated_points();
    if( components_colocated_pts.empty() )
    {
        geode::Logger::info(
            "BRep component meshes have no colocated points." );
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
    geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto components_degenerated_elements =
        brep_inspector.components_nb_degenerated_elements();
    if( components_degenerated_elements.empty() )
    {
        geode::Logger::info( "BRep component meshes are not degenerated." );
    }
    for( const auto& degenerated : components_degenerated_elements )
    {
        geode::Logger::info( "Mesh of component with uuid ",
            degenerated.first.string(), " has ", degenerated.second,
            " degenerated elements." );
        nb_issues += degenerated.second;
    }
    return nb_issues;
}

geode::index_t check_components_manifold( geode::BRepInspector& brep_inspector )
{
    geode::index_t nb_issues{ 0 };
    const auto components_nb_non_manifold_vertices =
        brep_inspector.component_meshes_nb_non_manifold_vertices();
    const auto components_nb_non_manifold_edges =
        brep_inspector.component_meshes_nb_non_manifold_edges();
    const auto components_nb_non_manifold_facets =
        brep_inspector.component_meshes_nb_non_manifold_facets();
    if( components_nb_non_manifold_vertices.empty()
        && components_nb_non_manifold_edges.empty()
        && components_nb_non_manifold_facets.empty() )
    {
        geode::Logger::info( "BRep component meshes are manifold." );
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
    for( const auto& non_manifold_facets : components_nb_non_manifold_facets )
    {
        geode::Logger::info( "Mesh of surface with uuid ",
            non_manifold_facets.first.string(), " has ",
            non_manifold_facets.second, " non manifold facets." );
        nb_issues += non_manifold_facets.second;
    }
    return nb_issues;
}

geode::index_t check_components_intersections(
    geode::BRepInspector& brep_inspector )
{
    const auto nb_surfaces_intersections =
        brep_inspector.nb_intersecting_surfaces_elements_pair();
    if( nb_surfaces_intersections == 0 )
    {
        geode::Logger::info( "BRep meshes have no intersection problems." );
    }
    else
    {
        geode::Logger::info( "There are ", nb_surfaces_intersections,
            " pairs of intersecting triangles in the BRep." );
    }
    return nb_surfaces_intersections;
}

geode::index_t launch_component_meshes_validity_checks(
    geode::BRepInspector& brep_inspector )
{
    auto nb_issues = check_components_adjacency( brep_inspector );
    nb_issues += check_components_colocation( brep_inspector );
    nb_issues += check_components_degeneration( brep_inspector );
    nb_issues += check_components_manifold( brep_inspector );
    nb_issues += check_components_intersections( brep_inspector );

    return nb_issues;
}

void check_model_a1()
{
    const auto model_brep = geode::load_brep(
        absl::StrCat( geode::data_path, "model_A1.og_brep" ) );
    geode::BRepInspector brep_inspector{ model_brep };
    geode::Logger::info( "model_A1 topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );
    const auto nb_topological_issues =
        launch_topological_validity_checks( brep_inspector );
    OPENGEODE_EXCEPTION( nb_topological_issues == 1998, "[Test] model_A1 has ",
        nb_topological_issues,
        " instead of 1998 unique vertices with topological problems." );
    const auto nb_component_meshes_issues =
        launch_component_meshes_validity_checks( brep_inspector );
    OPENGEODE_EXCEPTION( nb_component_meshes_issues == 11759,
        "[Test] model_A1 should have 11759 issues in its component meshes "
        "(pairs of component meshes triangles intersecting)." );
}

void check_model_a1_valid()
{
    const auto model_brep = geode::load_brep(
        absl::StrCat( geode::data_path, "model_A1_valid.og_brep" ) );
    geode::BRepInspector brep_inspector{ model_brep };
    geode::Logger::info( "model_A1_valid topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );
    const auto nb_topological_issues =
        launch_topological_validity_checks( brep_inspector );
    OPENGEODE_EXCEPTION( nb_topological_issues == 1998,
        "[Test] model_A1_valid has ", nb_topological_issues,
        " instead of 1998 unique vertices with topological problems." );
    const auto nb_component_meshes_issues =
        launch_component_meshes_validity_checks( brep_inspector );
    OPENGEODE_EXCEPTION( nb_component_meshes_issues == 11759,
        "[Test] model_A1 should have 11759 issues in its component meshes "
        "(pairs of component meshes triangles intersecting)." );
}

void check_model_mss()
{
    const auto model_brep = geode::load_structural_model(
        absl::StrCat( geode::data_path, "mss.og_strm" ) );
    geode::BRepInspector brep_inspector{ model_brep };
    geode::Logger::info( "model mss topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );
    const auto nb_topological_issues =
        launch_topological_validity_checks( brep_inspector );
    OPENGEODE_EXCEPTION( nb_topological_issues == 17,
        "[Test] model mss.og_strm should have 17 unique "
        "vertices with topological problems." );
    const auto nb_component_meshes_issues =
        launch_component_meshes_validity_checks( brep_inspector );
    OPENGEODE_EXCEPTION( nb_component_meshes_issues == 0,
        "[Test] model mss.og_strm should have no issues in its component "
        "meshes." );
}

void check_model_D()
{
    const auto model_brep =
        geode::load_brep( absl::StrCat( geode::data_path, "model_D.og_brep" ) );
    geode::BRepInspector brep_inspector{ model_brep };
    geode::Logger::info( "model_D topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );
    const auto nb_topological_issues =
        launch_topological_validity_checks( brep_inspector );
    OPENGEODE_EXCEPTION( nb_topological_issues == 0,
        "[Test] model model_D.og_brep should have 0 unique "
        "vertices with topological problems." );
    const auto nb_component_meshes_issues =
        launch_component_meshes_validity_checks( brep_inspector );
    OPENGEODE_EXCEPTION( nb_component_meshes_issues == 0,
        "[Test] model_D should have no issues in its component meshes." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        geode::GeosciencesExplicitLibrary::initialize();
        check_model_a1();
        check_model_a1_valid();
        check_model_mss();
        check_model_D();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
