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
#include <absl/strings/str_cat.h>

#include <async++.h>

#include <geode/basic/assert.h>
#include <geode/basic/filename.h>
#include <geode/basic/logger.h>

#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>

#include <geode/io/model/common.h>

#include <geode/inspector/brep_inspector.h>

ABSL_FLAG( std::string, input, "/path/my/model.og_brep", "Input model" );

void inspect_brep( const geode::BRep& brep )
{
    const geode::BRepInspector brep_inspector{ brep };
    auto result = brep_inspector.inspect_brep();

    geode::Logger::info(
        result.meshes.unique_vertices_colocation
            .unique_vertices_linked_to_different_points.number(),
        " unique vertices linked to mesh points at "
        "different positions" );
    geode::Logger::info( result.meshes.unique_vertices_colocation
                             .colocated_unique_vertices_groups.number(),
        " groups of colocated unique vertices." );

    geode::Logger::info(
        result.topology.corners.corners_not_linked_to_a_unique_vertex.size(),
        " corners with vertices not linked to a unique vertex." );

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
        result.topology.lines.lines_not_linked_to_a_unique_vertex.size(),
        " lines with vertices not linked to a unique vertex." );

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
        result.topology.surfaces.surfaces_not_linked_to_a_unique_vertex.size(),
        " surfaces with vertices not linked to a unique vertex." );

    geode::Logger::info(
        result.topology.surfaces
            .unique_vertices_linked_to_not_internal_nor_boundary_surface
            .number(),
        " unique vertices part of a surface which "
        "is neither internal nor boundary." );

    geode::Logger::info(
        result.topology.surfaces
            .unique_vertices_linked_to_a_surface_with_invalid_embbedings
            .number(),
        " unique vertices part of a surface with "
        "invalid internal topology." );

    geode::Logger::info(
        result.topology.surfaces
            .unique_vertices_linked_to_a_single_and_invalid_surface.number(),
        " unique vertices part of a unique "
        "surface with invalid topology." );

    geode::Logger::info(
        result.topology.surfaces
            .unique_vertices_linked_to_several_and_invalid_surfaces.number(),
        " unique vertices part of multiple "
        "surfaces with invalid topology." );

    geode::Logger::info(
        result.topology.surfaces
            .unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
            .number(),
        " unique vertices part of a line and a surface but for "
        "which one of the associated vertex on the surface mesh is not "
        "on the mesh border." );

    geode::Logger::info(
        result.topology.blocks.blocks_not_linked_to_a_unique_vertex.size(),
        " blocks with vertices not linked to a unique vertex." );

    geode::Logger::info(
        result.topology.blocks.unique_vertices_with_incorrect_block_cmvs_count
            .number(),
        " unique vertices part of blocks with invalid topology." );

    geode::Logger::info(
        result.meshes.adjacencies.surfaces_edges_with_wrong_adjacencies.size(),
        " number of surfaces with adjacency problems." );
    geode::Logger::info(
        result.meshes.adjacencies.blocks_facets_with_wrong_adjacencies.size(),
        " number of blocks with adjacency problems." );

    geode::Logger::info(
        result.meshes.meshes_colocation.colocated_points_groups.size(),
        " number meshes with group of colocated points." );

    geode::Logger::info( result.meshes.degenerations.elements.size(),
        " number of meshes with degenerated elements." );

    geode::Logger::info(
        result.meshes.manifolds.meshes_non_manifold_vertices.size(),
        " number of meshes with non manifold vertices." );

    geode::Logger::info(
        result.meshes.manifolds.meshes_non_manifold_edges.size(),
        " number of meshes with non manifold edges." );

    geode::Logger::info(
        result.meshes.manifolds.meshes_non_manifold_facets.size(),
        " number of meshes with non manifold facets." );

    geode::Logger::info(
        result.meshes.manifolds.model_non_manifold_edges.size(),
        " number of non manifold model edges." );

    geode::Logger::info(
        result.meshes.intersections.elements_intersections.number(),
        " number of intersections between triangles in a surface." );
}

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage(
            absl::StrCat( "BRep inspector from Geode-solutions.\n",
                "Sample usage:\n", argv[0], " --input my_brep.og_brep\n",
                "Default behavior tests all available criteria, to disable one "
                "use --noXXX, e.g. --nocomponent_linking" ) );
        absl::ParseCommandLine( argc, argv );

        geode::IOModelLibrary::initialize();
        const auto filename = absl::GetFlag( FLAGS_input );

        inspect_brep( geode::load_brep( filename ) );

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
