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

#include <geode/geosciences/explicit/representation/core/structural_model.h>
#include <geode/geosciences/explicit/representation/io/structural_model_input.h>

#include <geode/geosciences_io/model/common.h>

#include <geode/inspector/brep_inspector.h>

ABSL_FLAG( std::string, input, "/path/my/model.og_strm", "Input model" );
ABSL_FLAG( bool,
    component_linking,
    true,
    "Toggle components linking to unique vertices criterion" );
ABSL_FLAG( bool,
    unique_vertices_colocation,
    true,
    "Toggle inspection of unique vertices colocation" );
ABSL_FLAG( bool,
    corners,
    true,
    "Toggle inspection of corner topology through unique vertices" );
ABSL_FLAG( bool,
    lines,
    true,
    "Toggle inspection of lines topology through unique vertices" );
ABSL_FLAG( bool,
    surfaces,
    true,
    "Toggle inspection of surfaces topology through unique vertices" );
ABSL_FLAG( bool,
    blocks,
    true,
    "Toggle inspection of block topology through unique vertices" );
ABSL_FLAG( bool, adjacency, true, "Toggle adjacency criterion for components" );
ABSL_FLAG(
    bool, colocation, true, "Toggle colocation criterion for components" );
ABSL_FLAG(
    bool, degeneration, true, "Toggle degeneration criterion for components" );
ABSL_FLAG( bool, manifold, true, "Toggle manifold criterion for components" );
ABSL_FLAG( bool,
    verbose,
    false,
    "Toggle verbose mode for the inspection of topology through unique "
    "vertices" );

void inspect_model( const geode::StructuralModel& model )
{
    const auto verbose = absl::GetFlag( FLAGS_verbose );
    const geode::BRepInspector model_inspector{ model, verbose };
    absl::InlinedVector< async::task< void >, 29 > tasks;
    if( absl::GetFlag( FLAGS_component_linking ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb_corners =
                model_inspector.nb_corners_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_corners, " corners not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb_lines =
                model_inspector
                    .nb_lines_meshed_but_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_lines, " lines meshed but not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb_surfaces =
                model_inspector
                    .nb_surfaces_meshed_but_not_linked_to_a_unique_vertex();
            geode::Logger::info( nb_surfaces,
                " surfaces meshed but not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb_blocks =
                model_inspector
                    .nb_blocks_meshed_but_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_blocks, " blocks meshed but not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb_unlinked_uv =
                model_inspector
                    .nb_unique_vertices_not_linked_to_a_component_vertex();
            geode::Logger::info( nb_unlinked_uv,
                " unique vertices not linked to a component mesh vertex" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_unique_vertices_colocation ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb_unique_vertices =
                model_inspector.nb_unique_vertices_linked_to_different_points();
            geode::Logger::info( nb_unique_vertices,
                " unique vertices linked to mesh points at "
                "different positions" );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb_unique_vertices =
                model_inspector.nb_colocated_unique_vertices();
            geode::Logger::info(
                nb_unique_vertices, " unique vertices which are colocated" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_corners ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.multiple_corners_unique_vertices().size();
            geode::Logger::info(
                nb, " unique vertices associated to multiple corners." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.multiple_internals_corner_vertices().size();
            geode::Logger::info( nb, " unique vertices associated to a corner "
                                     "with multiple internals." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.not_internal_nor_boundary_corner_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices associated to a corner which is neither "
                "internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.line_corners_without_boundary_status().size();
            geode::Logger::info( nb, " unique vertices associated to a corner "
                                     "part of a line but not boundary of it." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_lines ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector
                    .part_of_not_boundary_nor_internal_line_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a line which is "
                                     "neither internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector
                    .part_of_line_with_invalid_internal_topology_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a line with "
                                     "invalid internal topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.part_of_invalid_unique_line_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a unique line "
                                     "with invalid topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.part_of_lines_but_not_corner_unique_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices part of multiple lines but not a corner." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_surfaces ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector
                    .part_of_not_boundary_nor_internal_surface_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a surface which "
                                     "is neither internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector
                    .part_of_surface_with_invalid_internal_topology_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a surface with "
                                     "invalid internal topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.part_of_invalid_unique_surface_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a unique "
                                     "surface with invalid topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector
                    .part_of_invalid_multiple_surfaces_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of multiple "
                                     "surfaces with invalid topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector
                    .part_of_line_and_not_on_surface_border_unique_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices part of a line and a surface but for "
                "which one of the associated vertex on the surface mesh is not "
                "on the mesh border." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_blocks ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.part_of_invalid_blocks_unique_vertices().size();
            geode::Logger::info(
                nb, " unique vertices part of blocks with invalid topology." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_adjacency ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.surfaces_nb_edges_with_wrong_adjacencies()
                    .size();
            geode::Logger::info(
                nb, " surfaces with adjacency problems in their mesh." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.blocks_nb_facets_with_wrong_adjacencies()
                    .size();
            geode::Logger::info(
                nb, " blocks with adjacency problems in their mesh." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_colocation ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.components_nb_colocated_points().size();
            geode::Logger::info(
                nb, " components with colocated points in their mesh." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_degeneration ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.components_nb_degenerated_elements().size();
            geode::Logger::info(
                nb, " components with degenerated edges in their mesh." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_manifold ) )
    {
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.component_meshes_nb_non_manifold_vertices()
                    .size();
            geode::Logger::info(
                nb, " components with non manifold vertices in their mesh." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.component_meshes_non_manifold_edges().size();
            geode::Logger::info(
                nb, " components with non manifold edges in their mesh." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb =
                model_inspector.component_meshes_nb_non_manifold_facets()
                    .size();
            geode::Logger::info(
                nb, " components with non manifold facets in their mesh." );
        } ) );
        tasks.emplace_back( async::spawn( [&model_inspector] {
            const auto nb = model_inspector.model_non_manifold_edges().size();
            geode::Logger::info(
                nb, " components with non manifold model edges." );
        } ) );
    }
    for( auto& task : async::when_all( tasks.begin(), tasks.end() ).get() )
    {
        task.get();
    }
}

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage(
            absl::StrCat( "StructuralModel inspector from Geode-solutions.\n",
                "Sample usage:\n", argv[0],
                " --input my_structural_model.og_strm\n",
                "Default behavior tests all available criteria, to disable one "
                "use --noXXX, e.g. --nocomponent_linking" ) );
        absl::ParseCommandLine( argc, argv );

        geode::GeosciencesIOModelLibrary::initialize();
        const auto filename = absl::GetFlag( FLAGS_input );

        inspect_model( geode::load_structural_model( filename ) );

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
