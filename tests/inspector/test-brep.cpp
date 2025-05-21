/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/tests_config.hpp>

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/model/helpers/convert_model_meshes.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>

#include <geode/inspector/brep_inspector.hpp>

geode::index_t corners_topological_validity(
    const geode::BRepCornersTopologyInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& corner_issue :
        result.corners_not_linked_to_a_unique_vertex.issues_map() )
    {
        nb_issues += corner_issue.second.nb_issues();
    }
    nb_issues += result.corners_not_meshed.nb_issues();
    nb_issues +=
        result.unique_vertices_liked_to_not_boundary_line_corner.nb_issues();
    nb_issues += result.unique_vertices_linked_to_multiple_corners.nb_issues();
    nb_issues +=
        result.unique_vertices_linked_to_not_internal_nor_boundary_corner
            .nb_issues();

    geode::Logger::info(
        "BRep Corners Topology check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t lines_topological_validity(
    const geode::BRepLinesTopologyInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& issue :
        result.lines_not_linked_to_a_unique_vertex.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    nb_issues += result.lines_not_meshed.nb_issues();
    nb_issues += result.unique_vertices_linked_to_a_line_with_invalid_embeddings
                     .nb_issues();

    nb_issues +=
        result
            .unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
            .nb_issues();
    geode::Logger::info( "BRep Lines Topology check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t surfaces_topological_validity(
    const geode::BRepSurfacesTopologyInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& issue :
        result.surfaces_not_linked_to_a_unique_vertex.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    nb_issues += result.surfaces_not_meshed.nb_issues();
    nb_issues +=
        result.unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
            .nb_issues();
    nb_issues += result.unique_vertices_linked_to_several_and_invalid_surfaces
                     .nb_issues();
    geode::Logger::info(
        "BRep Surfaces Topology check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t blocks_topological_validity(
    const geode::BRepBlocksTopologyInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& issue :
        result.blocks_not_linked_to_a_unique_vertex.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    nb_issues += result.some_blocks_not_meshed.nb_issues();
    nb_issues +=
        result.unique_vertices_part_of_two_blocks_and_no_boundary_surface
            .nb_issues();
    nb_issues +=
        result.unique_vertices_with_incorrect_block_cmvs_count.nb_issues();
    nb_issues +=
        result.unique_vertices_linked_to_a_single_and_invalid_line.nb_issues();
    nb_issues += result.unique_vertices_linked_to_not_internal_nor_boundary_line
                     .nb_issues();
    nb_issues += result.unique_vertices_linked_to_a_single_and_invalid_surface
                     .nb_issues();
    nb_issues +=
        result
            .unique_vertices_linked_to_surface_with_wrong_relationship_with_blocks
            .nb_issues();
    geode::Logger::info(
        "BRep Blocks Topology check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t launch_topological_validity_checks(
    const geode::BRepTopologyInspectionResult& result, bool string )
{
    auto nb_issues = corners_topological_validity( result.corners, string );
    nb_issues += lines_topological_validity( result.lines, string );
    nb_issues += surfaces_topological_validity( result.surfaces, string );
    nb_issues += blocks_topological_validity( result.blocks, string );
    return nb_issues;
}

geode::index_t meshes_adjacencies_validity(
    const geode::BRepMeshesAdjacencyInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& issue :
        result.surfaces_edges_with_wrong_adjacencies.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    for( const auto& issue :
        result.blocks_facets_with_wrong_adjacencies.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    geode::Logger::info(
        "BRep meshes adjacencies check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t meshes_degenerations_validity(
    const geode::BRepMeshesDegenerationInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& degenerated_elements :
        result.degenerated_edges.issues_map() )
    {
        nb_issues += degenerated_elements.second.nb_issues();
    }
    for( const auto& degenerated_elements :
        result.degenerated_polygons.issues_map() )
    {
        nb_issues += degenerated_elements.second.nb_issues();
    }
    for( const auto& degenerated_elements :
        result.degenerated_polyhedra.issues_map() )
    {
        nb_issues += degenerated_elements.second.nb_issues();
    }
    geode::Logger::info(
        "BRep meshes degenerated elements check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t meshes_intersections_validity(
    const geode::ElementsIntersectionsInspectionResult result, bool string )
{
    geode::index_t nb_issues{ 0 };
    nb_issues += result.elements_intersections.nb_issues();

    geode::Logger::info(
        "BRep meshes element intersections check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t meshes_manifolds_validity(
    const geode::BRepMeshesManifoldInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& issue : result.meshes_non_manifold_vertices.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    for( const auto& issue : result.meshes_non_manifold_edges.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    for( const auto& issue : result.meshes_non_manifold_facets.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    for( const auto& issue : result.brep_non_manifold_edges.issues() )
    {
        nb_issues += issue.component_ids.size();
    }
    geode::Logger::info(
        "BRep meshes non manifolds check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t meshes_colocations_validity(
    const geode::MeshesColocationInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& issue : result.colocated_points_groups.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    geode::Logger::info(
        "BRep meshes Colocations check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t meshes_unique_vertices_validity(
    const geode::UniqueVerticesInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& issue : result.colocated_unique_vertices_groups.issues() )
    {
        nb_issues += issue.size();
    }
    nb_issues += result.unique_vertices_linked_to_different_points.nb_issues();

    geode::Logger::info(
        "BRep unique vertices check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t launch_component_meshes_validity_checks(
    const geode::BRepMeshesInspectionResult& result, bool string )
{
    auto nb_issues =
        meshes_adjacencies_validity( result.meshes_adjacencies, string );
    nb_issues =
        meshes_degenerations_validity( result.meshes_degenerations, string );
    nb_issues +=
        meshes_intersections_validity( result.meshes_intersections, string );
    nb_issues +=
        meshes_manifolds_validity( result.meshes_non_manifolds, string );
    nb_issues +=
        meshes_colocations_validity( result.meshes_colocation, string );
    nb_issues += meshes_unique_vertices_validity(
        result.unique_vertices_colocation, string );
    return nb_issues;
}

void check_model_a1( bool string )
{
    const auto model_brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "model_A1.og_brep" ) );
    const geode::BRepInspector brep_inspector{ model_brep };
    const auto result = brep_inspector.inspect_brep();

    geode::Logger::info( "model_A1 topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );

    const auto nb_topological_issues =
        launch_topological_validity_checks( result.topology, string );
    OPENGEODE_EXCEPTION( nb_topological_issues == 5164, "[Test] model_A1 has ",
        nb_topological_issues, " topological problems instead of 5164." );

    const auto nb_component_meshes_issues =
        launch_component_meshes_validity_checks( result.meshes, string );
    OPENGEODE_EXCEPTION( nb_component_meshes_issues == 13494,
        "[Test] model_A1 has ", nb_component_meshes_issues,
        " meshes problems instead of 13494." );
}

void check_model_a1_valid( bool string )
{
    const auto model_brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "model_A1_valid.og_brep" ) );
    const geode::BRepInspector brep_inspector{ model_brep };
    const auto result = brep_inspector.inspect_brep();

    geode::Logger::info( "model_A1_valid topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );

    const auto nb_topological_issues =
        launch_topological_validity_checks( result.topology, string );
    OPENGEODE_EXCEPTION( nb_topological_issues == 5164,
        "[Test] model_A1_valid has ", nb_topological_issues,
        " topological problems instead of 5164." );

    const auto nb_component_meshes_issues =
        launch_component_meshes_validity_checks( result.meshes, string );
    OPENGEODE_EXCEPTION( nb_component_meshes_issues == 13494,
        "[Test] model_A1_valid has ", nb_component_meshes_issues,
        " meshes problems instead of 13494." );
}

void check_model_mss( bool string )
{
    const auto model_brep =
        geode::load_brep( absl::StrCat( geode::DATA_PATH, "mss.og_brep" ) );
    const geode::BRepInspector brep_inspector{ model_brep };
    const auto result = brep_inspector.inspect_brep();

    geode::Logger::info( "model mss topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );

    const auto nb_topological_issues =
        launch_topological_validity_checks( result.topology, string );
    OPENGEODE_EXCEPTION( nb_topological_issues == 34, "[Test] mss has ",
        nb_topological_issues, " topological problems instead of 34." );

    const auto nb_component_meshes_issues =
        launch_component_meshes_validity_checks( result.meshes, string );
    OPENGEODE_EXCEPTION( nb_component_meshes_issues == 0, "[Test] mss has ",
        nb_component_meshes_issues, " meshes problems instead of 0." );
}

void check_model_D( bool string )
{
    const auto model_brep =
        geode::load_brep( absl::StrCat( geode::DATA_PATH, "model_D.og_brep" ) );
    const geode::BRepInspector brep_inspector{ model_brep };
    const auto result = brep_inspector.inspect_brep();

    geode::Logger::info( "model_D topology is ",
        brep_inspector.brep_topology_is_valid() ? "valid." : "invalid." );
    const auto nb_topological_issues =
        launch_topological_validity_checks( result.topology, string );
    OPENGEODE_EXCEPTION( nb_topological_issues == 0, "[Test] model_D has ",
        nb_topological_issues, " topological problems instead of 0." );
    const auto nb_component_meshes_issues =
        launch_component_meshes_validity_checks( result.meshes, string );
    OPENGEODE_EXCEPTION( nb_component_meshes_issues == 0, "[Test] model_D has ",
        nb_component_meshes_issues, " meshes problems instead of 0." );
}

void check_wrong_bsurfaces_model()
{
    const auto model_brep = geode::load_brep( absl::StrCat(
        geode::DATA_PATH, "wrong_boundary_surface_model.og_brep" ) );
    const geode::BRepInspector brep_inspector{ model_brep };
    const auto result = brep_inspector.inspect_brep();
    OPENGEODE_EXCEPTION(
        result.topology.blocks.wrong_block_boundary_surface.nb_issues() == 3,
        absl::StrCat(
            "[Test] Wrong number of wrong block boundary surfaces detected: "
            "should be 3, and it is ",
            result.topology.blocks.wrong_block_boundary_surface.nb_issues(),
            "." ) );
    std::vector< geode::uuid > wrong_bsurf{
        geode::uuid{ "00000000-78d4-4e10-8000-0000cb3a3a27" },
        geode::uuid{ "00000000-7a4e-4a1c-8000-00003732de1f" },
        geode::uuid{ "00000000-980f-49d4-8000-00002f79374e" }
    };
    for( const auto& issue :
        result.topology.blocks.wrong_block_boundary_surface.issues() )
    {
        OPENGEODE_EXCEPTION(
            absl::c_find( wrong_bsurf, issue ) != wrong_bsurf.end(),
            "[Test] Surface (", issue.string(),
            ") is detected as a wrong boundary surface but is not one." );
    }
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_model_a1( false );
        check_model_a1_valid( false );
        check_model_mss( false );
        check_model_D( false );
        check_wrong_bsurfaces_model();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
