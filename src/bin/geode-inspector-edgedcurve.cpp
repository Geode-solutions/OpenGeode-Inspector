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

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/io/edged_curve_input.h>

#include <geode/io/mesh/detail/common.h>

#include <geode/inspector/criterion/colocation/edgedcurve_colocation.h>
#include <geode/inspector/criterion/degeneration/edgedcurve_degeneration.h>

ABSL_FLAG( std::string, input, "/path/my/curve.og_edc3d", "Input edged curve" );
ABSL_FLAG( bool, colocation, true, "Toggle colocation criterion" );
ABSL_FLAG( bool, degeneration, true, "Toggle degeneration criterion" );

template < geode::index_t dimension >
void inspect_edgedcurve( const geode::EdgedCurve< dimension >& edgedcurve )
{
    absl::InlinedVector< async::task< void >, 2 > tasks;
    if( absl::GetFlag( FLAGS_colocation ) )
    {
        tasks.emplace_back( async::spawn( [&edgedcurve] {
            const geode::EdgedCurveColocation< dimension > colocation{
                edgedcurve
            };
            const auto nb = colocation.nb_colocated_points();
            geode::Logger::info( nb, " colocated points" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_degeneration ) )
    {
        tasks.emplace_back( async::spawn( [&edgedcurve] {
            const geode::EdgedCurveDegeneration< dimension > degeneration{
                edgedcurve
            };
            const auto nb = degeneration.nb_degenerated_edges();
            geode::Logger::info( nb, " degenerated edges" );
        } ) );
    }
    async::when_all( tasks.begin(), tasks.end() ).wait();
}

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage(
            absl::StrCat( "EdgedCurve inspector from Geode-solutions.\n",
                "Sample usage:\n", argv[0], " --input my_edgedcurve.og_edc3d\n",
                "Default behavior test all available criteria, to disable one "
                "use --noXXX, e.g. --nocolocation" ) );
        absl::ParseCommandLine( argc, argv );

        geode::detail::initialize_mesh_io();
        const auto filename = absl::GetFlag( FLAGS_input );
        const auto ext =
            geode::to_string( geode::extension_from_filename( filename ) );

        if( geode::EdgedCurveInputFactory3D::has_creator( ext ) )
        {
            inspect_edgedcurve( *geode::load_edged_curve< 3 >( filename ) );
        }
        else if( geode::EdgedCurveInputFactory2D::has_creator( ext ) )
        {
            inspect_edgedcurve( *geode::load_edged_curve< 2 >( filename ) );
        }
        else
        {
            throw geode::OpenGeodeException( "Unable to load file ", filename );
        }

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
