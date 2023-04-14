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

void inspect_cross_section( const geode::CrossSection& cross_section )
{
    const auto verbose = absl::GetFlag( FLAGS_verbose );
    const geode::SectionInspector cross_section_inspector{ cross_section,
        verbose };
    absl::InlinedVector< async::task< void >, 12 > tasks;
    if( absl::GetFlag( FLAGS_component_linking ) )
    {
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb_corners =
                cross_section_inspector
                    .nb_corners_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_corners, " corners not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb_lines =
                cross_section_inspector
                    .nb_lines_meshed_but_not_linked_to_a_unique_vertex();
            geode::Logger::info(
                nb_lines, " lines meshed but not linked to a unique vertex" );
        } ) );
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb_surfaces =
                cross_section_inspector
                    .nb_surfaces_meshed_but_not_linked_to_a_unique_vertex();
            geode::Logger::info( nb_surfaces,
                " surfaces meshed but not linked to a unique vertex" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_corners ) )
    {
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb =
                cross_section_inspector.multiple_corners_unique_vertices()
                    .size();
            geode::Logger::info(
                nb, " unique vertices associated to multiple corners." );
        } ) );
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb =
                cross_section_inspector.multiple_internals_corner_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices associated to a corner "
                                     "with multiple internals." );
        } ) );
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb = cross_section_inspector
                                .not_internal_nor_boundary_corner_vertices()
                                .size();
            geode::Logger::info( nb,
                " unique vertices associated to a corner which is neither "
                "internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb =
                cross_section_inspector.line_corners_without_boundary_status()
                    .size();
            geode::Logger::info( nb, " unique vertices associated to a corner "
                                     "part of a line but not boundary of it." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_lines ) )
    {
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb =
                cross_section_inspector
                    .part_of_not_boundary_nor_internal_line_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a line which is "
                                     "neither internal nor boundary." );
        } ) );
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb =
                cross_section_inspector
                    .part_of_line_with_invalid_internal_topology_unique_vertices()
                    .size();
            geode::Logger::info( nb, " unique vertices part of a line with "
                                     "invalid internal topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb = cross_section_inspector
                                .part_of_invalid_unique_line_unique_vertices()
                                .size();
            geode::Logger::info( nb, " unique vertices part of a unique line "
                                     "with invalid topology." );
        } ) );
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb = cross_section_inspector
                                .part_of_lines_but_not_corner_unique_vertices()
                                .size();
            geode::Logger::info( nb,
                " unique vertices part of multiple lines but not a corner." );
        } ) );
    }
    if( absl::GetFlag( FLAGS_surfaces ) )
    {
        tasks.emplace_back( async::spawn( [&cross_section_inspector] {
            const auto nb = cross_section_inspector
                                .part_of_invalid_surfaces_unique_vertices()
                                .size();
            geode::Logger::info( nb,
                " unique vertices part of surfaces with invalid topology." );
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
