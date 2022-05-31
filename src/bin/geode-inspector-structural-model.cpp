/*
 * Copyright (c) 2019 - 2022 Geode-solutions. All rights reserved.
 */

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

#include <async++.h>

#include <geode/basic/assert.h>
#include <geode/basic/filename.h>
#include <geode/basic/logger.h>

#include <geode/geosciences/representation/core/structural_model.h>
#include <geode/geosciences/representation/io/structural_model_input.h>

#include <geode/geosciences/detail/common.h>

#include <geode/inspector/brep_inspector.h>

ABSL_FLAG( std::string, input, "/path/my/model.og_strm", "Input model" );
ABSL_FLAG( bool,
    component_linking,
    true,
    "Toggle components linking to unique vertices criterion" );
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
ABSL_FLAG( bool,
    verbose,
    false,
    "Toggle verbose mode for the inspection of topology through unique "
    "vertices" );

void inspect_model( const geode::StructuralModel& model )
{
    const auto verbose = absl::GetFlag( FLAGS_verbose );
    const geode::BRepInspector model_inspector{ model, verbose };
    absl::InlinedVector< async::task< void >, 18 > tasks;
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
                model_inspector
                    .internal_with_multiple_incidences_corner_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices associated to a corner which is internal "
                "but has multiple incidences." );
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
    async::when_all( tasks.begin(), tasks.end() ).wait();
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

        geode::detail::initialize_geosciences_io();
        const auto filename = absl::GetFlag( FLAGS_input );

        inspect_model( geode::load_structural_model( filename ) );

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
