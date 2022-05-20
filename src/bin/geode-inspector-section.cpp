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

#include <geode/model/representation/core/section.h>
#include <geode/model/representation/io/section_input.h>

#include <geode/io/model/detail/common.h>

#include <geode/inspector/section_inspector.h>

ABSL_FLAG( std::string, input, "/path/my/model.og_sctn", "Input model" );
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
    verbose,
    false,
    "Toggle verbose mode for the inspection of topology through unique "
    "vertices" );

void inspect_section( const geode::Section& section )
{
    const auto verbose = absl::GetFlag( FLAGS_verbose );
    const geode::SectionInspector section_inspector{ section, verbose };
    absl::InlinedVector< async::task< void >, 13 > tasks;
    if( absl::GetFlag( FLAGS_component_linking ) )
    {
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb_corners =
                section_inspector.nb_corners_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_corners, " corners not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb_lines =
                section_inspector
                    .nb_lines_meshed_but_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_lines, " lines meshed but not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb_surfaces =
                section_inspector
                    .nb_surfaces_meshed_but_not_linked_to_a_unique_vertex();
            geode::Logger::info( nb_surfaces,
                " surfaces meshed but not linked to a unique vertex" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_corners ) )
    {
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector.multiple_corners_unique_vertices().size();
            geode::Logger::info(
                nb, " unique vertices associated to multiple corners." );
        } ) );
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector.multiple_internals_corner_vertices().size();
            geode::Logger::info( nb, " unique vertices associated to a corner "
                                     "with multiple internals." );
        } ) );
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector.not_internal_nor_boundary_corner_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices associated to a corner which is neither "
                "internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector
                    .internal_with_multiple_incidences_corner_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices associated to a corner which is internal "
                "but has multiple incidences." );
        } ) );
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector.line_corners_without_boundary_status().size();
            geode::Logger::info( nb, " unique vertices associated to a corner "
                                     "part of a line but not boundary of it." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_lines ) )
    {
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector
                    .part_of_not_boundary_nor_internal_line_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a line which is "
                                     "neither internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector
                    .part_of_line_with_invalid_internal_topology_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a line with "
                                     "invalid internal topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector.part_of_invalid_unique_line_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a unique line "
                                     "with invalid topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector.part_of_lines_but_not_corner_unique_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices part of multiple lines but not a corner." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_surfaces ) )
    {
        tasks.emplace_back( async::spawn( [&section_inspector] {
            const auto nb =
                section_inspector.part_of_invalid_surfaces_unique_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices part of surfaces with invalid topology." );
        } ) );
    }
    async::when_all( tasks.begin(), tasks.end() ).wait();
}

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage(
            absl::StrCat( "Section inspector from Geode-solutions.\n",
                "Sample usage:\n", argv[0], " --input my_section.og_sctn\n",
                "Default behavior tests all available criteria, to disable one "
                "use --noXXX, e.g. --nocomponent_linking" ) );
        absl::ParseCommandLine( argc, argv );

        geode::detail::initialize_model_io();
        const auto filename = absl::GetFlag( FLAGS_input );

        inspect_section( geode::load_section( filename ) );

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}