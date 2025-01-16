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
#include <geode/model/representation/core/section.hpp>
#include <geode/model/representation/io/section_input.hpp>

#include <geode/geosciences_io/model/common.hpp>

#include <geode/inspector/section_inspector.hpp>

geode::index_t corners_topological_validity(
    const geode::SectionCornersTopologyInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& corner_issue :
        result.corners_not_linked_to_a_unique_vertex.issues_map() )
    {
        nb_issues += corner_issue.second.nb_issues();
    }
    nb_issues += result.corners_not_meshed.nb_issues();
    nb_issues +=
        result.unique_vertices_linked_to_not_boundary_line_corner.nb_issues();
    nb_issues += result.unique_vertices_linked_to_multiple_corners.nb_issues();
    nb_issues +=
        result.unique_vertices_linked_to_multiple_internals_corner.nb_issues();
    nb_issues +=
        result.unique_vertices_linked_to_not_internal_nor_boundary_corner
            .nb_issues();

    geode::Logger::info(
        "Section Corners Topology check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t lines_topological_validity(
    const geode::SectionLinesTopologyInspectionResult& result, bool string )
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
        result.unique_vertices_linked_to_a_single_and_invalid_line.nb_issues();
    nb_issues += result.unique_vertices_linked_to_not_internal_nor_boundary_line
                     .nb_issues();
    nb_issues +=
        result
            .unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
            .nb_issues();
    geode::Logger::info(
        "Section Lines Topology check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t surfaces_topological_validity(
    const geode::SectionSurfacesTopologyInspectionResult& result, bool string )
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
    nb_issues +=
        result.unique_vertices_linked_to_a_surface_with_invalid_embbedings
            .nb_issues();
    geode::Logger::info(
        "Section Surfaces Topology check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t launch_topological_validity_checks(
    const geode::SectionTopologyInspectionResult& result, bool string )
{
    auto nb_issues = corners_topological_validity( result.corners, string );
    nb_issues += lines_topological_validity( result.lines, string );
    nb_issues += surfaces_topological_validity( result.surfaces, string );
    return nb_issues;
}

geode::index_t meshes_adjacencies_validity(
    const geode::SectionMeshesAdjacencyInspectionResult& result, bool string )
{
    geode::index_t nb_issues{ 0 };
    for( const auto& issue :
        result.surfaces_edges_with_wrong_adjacencies.issues_map() )
    {
        nb_issues += issue.second.nb_issues();
    }
    geode::Logger::info(
        "Section meshes adjacencies check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t meshes_degenerations_validity(
    const geode::SectionMeshesDegenerationInspectionResult& result,
    bool string )
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
    geode::Logger::info(
        "Section meshes degenerated elements check: ", nb_issues, " issues." );
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
        "Section meshes element intersections check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t meshes_manifolds_validity(
    const geode::SectionMeshesManifoldInspectionResult& result, bool string )
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
    geode::Logger::info(
        "Section meshes non manifolds check: ", nb_issues, " issues." );
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
        "Section meshes Colocations check: ", nb_issues, " issues." );
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
        "Section unique vertices check: ", nb_issues, " issues." );
    if( string )
    {
        geode::Logger::info( absl::StrCat( result.string(), "\n" ) );
    }
    return nb_issues;
}

geode::index_t launch_component_meshes_validity_checks(
    const geode::SectionMeshesInspectionResult& result, bool string )
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

void check_section( bool string )
{
    const auto model_section = geode::load_section(
        absl::StrCat( geode::DATA_PATH, "vertical_lines.og_sctn" ) );
    geode::SectionInspector section_inspector{ model_section };
    auto result = section_inspector.inspect_section();

    geode::Logger::info( "vertical_lines section topology is ",
        section_inspector.section_topology_is_valid() ? "valid." : "invalid." );

    const auto nb_topological_issues =
        launch_topological_validity_checks( result.topology, string );
    OPENGEODE_EXCEPTION( nb_topological_issues == 0, "[Test] model_D has ",
        nb_topological_issues, " topological problems instead of 0." );

    const auto nb_component_meshes_issues =
        launch_component_meshes_validity_checks( result.meshes, string );
    OPENGEODE_EXCEPTION( nb_component_meshes_issues == 0, "[Test] model_D has ",
        nb_component_meshes_issues, " meshes problems instead of 0." );
}

void check_section_test()
{
    const auto model_section =
        geode::load_section( absl::StrCat( geode::DATA_PATH, "test.shp" ) );
    geode::SectionInspector section_inspector{ model_section };
    auto result = section_inspector.inspect_section();

    geode::Logger::info( "vertical_lines section topology is ",
        section_inspector.section_topology_is_valid() ? "valid." : "invalid." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        geode::GeosciencesIOModelLibrary::initialize();
        geode::Logger::set_level( geode::Logger::LEVEL::trace );
        check_section( false );
        check_section_test();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}