/*
 * Copyright (c) 2019 - 2021 Geode-solutions. All rights reserved.
 */

#include <absl/flags/flag.h>
#include <absl/flags/parse.h>
#include <absl/flags/usage.h>

#include <async++.h>

#include <geode/basic/assert.h>
#include <geode/basic/filename.h>
#include <geode/basic/logger.h>

#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>

#include <geode/io/model/detail/common.h>

#include <geode/inspector/topology/brep_topology.h>

ABSL_FLAG( std::string, input, "/path/my/surface.og_tfs3d", "Input surface" );
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

void inspect_brep( const geode::BRep& brep )
{
    const geode::BRepTopologyInspector brep_inspector{ brep };
    absl::InlinedVector< async::task< void >, 17 > tasks;
    if( absl::GetFlag( FLAGS_component_linking ) )
    {
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb_corners =
                brep_inspector.nb_corners_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_corners, " corners not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb_lines =
                brep_inspector.nb_lines_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_lines, " corners not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb_surfaces =
                brep_inspector.nb_surfaces_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_surfaces, " surfaces not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb_blocks =
                brep_inspector.nb_blocks_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_blocks, " blocks not linked to a unique vertex" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_corners ) )
    {
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector.multiple_corners_unique_vertices().size();
            geode::Logger::info(
                nb, " unique vertices associated to multiple corners." );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector.multiple_internals_corner_vertices().size();
            geode::Logger::info( nb, " unique vertices associated to a corner "
                                     "with multiple internals." );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector.not_internal_nor_boundary_corner_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices associated to a corner which is neither "
                "internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector.line_corners_without_boundary_status().size();
            geode::Logger::info( nb, " unique vertices associated to a corner "
                                     "part of a line but not boundary of it." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_lines ) )
    {
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector
                    .part_of_not_boundary_nor_internal_line_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a line which is "
                                     "neither internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector
                    .part_of_line_with_invalid_internal_topology_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a line with "
                                     "invalid internal topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector.part_of_invalid_unique_line_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a unique line "
                                     "with invalid topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector.part_of_lines_but_not_corner_unique_vertices()
                    .size();
            geode::Logger::info( nb,
                " unique vertices part of multiple lines but not a corner." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_surfaces ) )
    {
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector
                    .part_of_not_boundary_nor_internal_surface_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a surface which "
                                     "is neither internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector
                    .part_of_surface_with_invalid_internal_topology_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a surface with "
                                     "invalid internal topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector.part_of_invalid_unique_surface_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a unique "
                                     "surface with invalid topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector
                    .part_of_invalid_multiple_surfaces_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of multiple "
                                     "surfaces with invalid topology." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_blocks ) )
    {
        tasks.emplace_back( async::spawn( [&brep_inspector] {
            const auto nb =
                brep_inspector.part_of_invalid_blocks_unique_vertices().size();
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
            absl::StrCat( "BRep inspector from Geode-solutions.\n",
                "Sample usage:\n", argv[0], " --input my_brep.og_brep3d\n",
                "Default behavior tests all available criteria, to disable one "
                "use --noXXX, e.g. --nocomponent_linking" ) );
        absl::ParseCommandLine( argc, argv );

        geode::detail::initialize_model_io();
        const auto filename = absl::GetFlag( FLAGS_input );
        const auto ext =
            geode::to_string( geode::extension_from_filename( filename ) );

        inspect_brep( geode::load_brep( filename ) );

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
