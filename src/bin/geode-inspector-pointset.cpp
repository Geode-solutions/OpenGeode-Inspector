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

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/io/point_set_input.h>

#include <geode/io/mesh/detail/common.h>

#include <geode/inspector/pointset_inspector.h>

ABSL_FLAG( std::string, input, "/path/my/pointset.og_pts3d", "Input pointset" );
ABSL_FLAG( bool,
    verbose,
    false,
    "Toggle verbose mode for the inspection of topology through unique "
    "vertices" );

template < geode::index_t dimension >
void inspect_pointset( const geode::PointSet< dimension >& pointset )
{
    const auto verbose = absl::GetFlag( FLAGS_verbose );
    const geode::PointSetInspector< dimension > inspector{ pointset, verbose };
    const auto nb = inspector.nb_colocated_points();
    geode::Logger::info( nb, " colocated points" );
}

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage( absl::StrCat(
            "PointSet inspector from Geode-solutions.\n", "Sample usage:\n",
            argv[0], " --input my_pointset.og_pts3d\n" ) );
        absl::ParseCommandLine( argc, argv );

        geode::detail::initialize_mesh_io();
        const auto filename = absl::GetFlag( FLAGS_input );
        const auto ext =
            geode::to_string( geode::extension_from_filename( filename ) );

        if( geode::PointSetInputFactory3D::has_creator( ext ) )
        {
            inspect_pointset( *geode::load_point_set< 3 >( filename ) );
        }
        else if( geode::PointSetInputFactory2D::has_creator( ext ) )
        {
            inspect_pointset( *geode::load_point_set< 2 >( filename ) );
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
