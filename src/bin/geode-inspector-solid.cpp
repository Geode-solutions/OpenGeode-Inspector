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

#include <geode/mesh/core/hybrid_solid.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/polyhedral_solid.h>
#include <geode/mesh/core/tetrahedral_solid.h>
#include <geode/mesh/io/hybrid_solid_input.h>
#include <geode/mesh/io/polyhedral_solid_input.h>
#include <geode/mesh/io/tetrahedral_solid_input.h>

#include <geode/io/mesh/detail/common.h>

#include <geode/inspector/solid_inspector.h>

ABSL_FLAG( std::string, input, "/path/my/solid.og_tso3d", "Input solid" );
ABSL_FLAG( bool, colocation, true, "Toggle colocation criterion" );
ABSL_FLAG( bool, degeneration, true, "Toggle degeneration criterion" );
ABSL_FLAG( bool, manifold_vertex, true, "Toggle manifold vertex criterion" );
ABSL_FLAG( bool, manifold_edge, true, "Toggle manifold edge criterion" );
ABSL_FLAG( bool, manifold_facet, true, "Toggle manifold facet criterion" );
ABSL_FLAG( bool,
    verbose,
    false,
    "Toggle verbose mode for the inspection of topology through unique "
    "vertices" );

template < geode::index_t dimension >
void inspect_solid( const geode::SolidMesh< dimension >& solid )
{
    const auto verbose = absl::GetFlag( FLAGS_verbose );
    absl::InlinedVector< async::task< void >, 5 > tasks;
    const geode::SolidMeshInspector< dimension > inspector{ solid, verbose };
    if( absl::GetFlag( FLAGS_colocation ) )
    {
        tasks.emplace_back( async::spawn( [&inspector] {
            const auto nb = inspector.nb_colocated_points();
            geode::Logger::info( nb, " colocated points" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_degeneration ) )
    {
        tasks.emplace_back( async::spawn( [&inspector] {
            const auto nb = inspector.nb_degenerated_edges();
            geode::Logger::info( nb, " degenerated edges" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_manifold_vertex ) )
    {
        tasks.emplace_back( async::spawn( [&inspector] {
            const auto nb = inspector.nb_non_manifold_vertices();
            geode::Logger::info( nb, " non manifold vertices" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_manifold_edge ) )
    {
        tasks.emplace_back( async::spawn( [&inspector] {
            const auto nb = inspector.nb_non_manifold_edges();
            geode::Logger::info( nb, " non manifold edges" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_manifold_facet ) )
    {
        tasks.emplace_back( async::spawn( [&inspector] {
            const auto nb = inspector.nb_non_manifold_facets();
            geode::Logger::info( nb, " non manifold facets" );
        } ) );
    }
    async::when_all( tasks.begin(), tasks.end() ).wait();
}

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage(
            absl::StrCat( "Solid inspector from Geode-solutions.\n",
                "Sample usage:\n", argv[0], " --input my_solid.og_tsf3d\n",
                "Default behavior test all available criteria, to disable one "
                "use --noXXX, e.g. --noadjacency" ) );
        absl::ParseCommandLine( argc, argv );

        geode::detail::initialize_mesh_io();
        const auto filename = absl::GetFlag( FLAGS_input );
        const auto ext =
            geode::to_string( geode::extension_from_filename( filename ) );

        if( geode::TetrahedralSolidInputFactory3D::has_creator( ext ) )
        {
            inspect_solid( *geode::load_tetrahedral_solid< 3 >( filename ) );
        }
        else if( geode::PolyhedralSolidInputFactory3D::has_creator( ext ) )
        {
            inspect_solid( *geode::load_polyhedral_solid< 3 >( filename ) );
        }
        else if( geode::HybridSolidInputFactory3D::has_creator( ext ) )
        {
            inspect_solid( *geode::load_hybrid_solid< 3 >( filename ) );
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
