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

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

#include <async++.h>

#include <geode/basic/assert.h>
#include <geode/basic/filename.h>
#include <geode/basic/logger.h>

#include <geode/geosciences/explicit/representation/core/cross_section.h>
#include <geode/geosciences/explicit/representation/io/cross_section_input.h>

#include <geode/geosciences_io/model/common.h>

#include <geode/inspector/section_inspector.h>

ABSL_FLAG( std::string, input, "/path/my/model.og_xsctn", "Input model" );

void inspect_cross_section( const geode::CrossSection& cross_section )
{
    const geode::SectionInspector cross_section_inspector{ cross_section };
    auto result = cross_section_inspector.inspect_section();

    geode::Logger::info(
        result.topology.unique_vertices_not_linked_to_any_component.number(),
        " unique vertices not linked to a component mesh vertex" );

    geode::Logger::info(
        result.meshes.unique_vertices_colocation
            .unique_vertices_linked_to_different_points.number(),
        " unique vertices linked to mesh points at "
        "different positions" );
    geode::Logger::info( result.meshes.unique_vertices_colocation
                             .colocated_unique_vertices_groups.number(),
        " number of group of colocated unique vertices." );

    geode::Logger::info(
        result.topology.corners.unique_vertices_linked_to_multiple_corners
            .number(),
        " unique vertices associated to multiple corners." );

    geode::Logger::info(
        result.topology.corners
            .unique_vertices_linked_to_multiple_internals_corner.number(),
        " unique vertices associated to a corner "
        "with multiple internals." );
    geode::Logger::info(
        result.topology.corners
            .unique_vertices_linked_to_not_internal_nor_boundary_corner
            .number(),
        " unique vertices associated to a corner which is neither "
        "internal nor boundary." );

    geode::Logger::info(
        result.topology.corners
            .unique_vertices_liked_to_not_boundary_line_corner.number(),
        " unique vertices associated to a corner "
        "part of a line but not boundary of it." );

    geode::Logger::info(
        result.topology.lines
            .unique_vertices_linked_to_not_internal_nor_boundary_line.number(),
        " unique vertices part of a line which is "
        "neither internal nor boundary." );
    geode::Logger::info(
        result.topology.lines
            .unique_vertices_linked_to_a_line_with_invalid_embeddings.number(),
        " unique vertices part of a line with "
        "invalid internal topology." );

    geode::Logger::info(
        result.topology.lines
            .unique_vertices_linked_to_a_single_and_invalid_line.number(),
        " unique vertices part of a unique line "
        "with invalid topology." );
    geode::Logger::info(
        result.topology.lines
            .unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
            .number(),
        " unique vertices part of multiple lines but not a corner." );

    geode::Logger::info(
        result.topology.surfaces
            .unique_vertices_linked_to_a_surface_with_invalid_embbedings
            .number(),
        " unique vertices part of surfaces with invalid topology." );
    geode::Logger::info(
        result.topology.surfaces
            .unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
            .number(),
        " unique vertices part of a line and a surface but for "
        "which one of the associated vertex on the surface mesh is not "
        "on the mesh border." );

    geode::Logger::info(
        result.meshes.adjacencies.surfaces_edges_with_wrong_adjacencies.size(),
        " surfaces with adjacency problems in their mesh." );

    geode::Logger::info(
        result.meshes.meshes_colocation.colocated_points_groups.size(),
        " components with groups of colocated points in their mesh." );

    geode::Logger::info( result.meshes.degenerations.elements.size(),
        " components with degenerated elements in their mesh." );

    geode::Logger::info(
        result.meshes.manifolds.meshes_non_manifold_vertices.size(),
        " components with non manifold vertices in their mesh." );

    geode::Logger::info(
        result.meshes.manifolds.meshes_non_manifold_edges.size(),
        " components with non manifold edges in their mesh." );

    geode::Logger::info(
        result.meshes.intersections.elements_intersections.number(),
        " pairs of component triangles intersecting each other." );
}

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage( absl::StrCat(
            "CrossSection inspector from Geode-solutions.\n", "Sample usage:\n",
            argv[0], " --input my_cross_section.og_xsctn\n",
            "Default behavior tests all available criteria, to disable one "
            "use --noXXX, e.g. --nocomponent_linking" ) );
        absl::ParseCommandLine( argc, argv );

        geode::GeosciencesIOModelLibrary::initialize();
        const auto filename = absl::GetFlag( FLAGS_input );

        inspect_cross_section( geode::load_cross_section( filename ) );

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
