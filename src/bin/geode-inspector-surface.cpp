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

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/io/polygonal_surface_input.h>
#include <geode/mesh/io/triangulated_surface_input.h>

#include <geode/io/mesh/detail/common.h>

#include <geode/inspector/criterion/adjacency/surface_adjacency.h>
#include <geode/inspector/criterion/colocation/surface_colocation.h>
#include <geode/inspector/criterion/degeneration/surface_degeneration.h>
#include <geode/inspector/criterion/manifold/surface_edge_manifold.h>
#include <geode/inspector/criterion/manifold/surface_vertex_manifold.h>

ABSL_FLAG( std::string, input, "/path/my/surface.og_tfs3d", "Input surface" );
ABSL_FLAG( bool, adjacency, true, "Toggle adjacency criterion" );
ABSL_FLAG( bool, colocation, true, "Toggle colocation criterion" );
ABSL_FLAG( bool, degeneration, true, "Toggle degeneration criterion" );
ABSL_FLAG( bool, manifold_vertex, true, "Toggle manifold vertex criterion" );
ABSL_FLAG( bool, manifold_edge, true, "Toggle manifold edge criterion" );

void inspect_surface( const geode::SurfaceMesh3D& surface )
{
    absl::InlinedVector< async::task< void >, 5 > tasks;
    if( absl::GetFlag( FLAGS_adjacency ) )
    {
        tasks.emplace_back( async::spawn( [&surface] {
            const geode::SurfaceMeshAdjacency3D adjacency{ surface };
            const auto nb = adjacency.nb_edges_with_wrong_adjacency();
            geode::Logger::info( nb, " edges with wrong adjacency" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_colocation ) )
    {
        tasks.emplace_back( async::spawn( [&surface] {
            const geode::SurfaceMeshColocation3D colocation{ surface };
            const auto nb = colocation.nb_colocated_points();
            geode::Logger::info( nb, " colocated points" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_degeneration ) )
    {
        tasks.emplace_back( async::spawn( [&surface] {
            const geode::SurfaceMeshDegeneration3D degeneration{ surface };
            const auto nb = degeneration.nb_degenerated_edges();
            geode::Logger::info( nb, " degenerated edges" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_manifold_vertex ) )
    {
        tasks.emplace_back( async::spawn( [&surface] {
            const geode::SurfaceMeshVertexManifold3D manifold_vertex{ surface };
            const auto nb = manifold_vertex.nb_non_manifold_vertices();
            geode::Logger::info( nb, " non manifold vertices" );
        } ) );
    }
    if( absl::GetFlag( FLAGS_manifold_edge ) )
    {
        tasks.emplace_back( async::spawn( [&surface] {
            const geode::SurfaceMeshEdgeManifold3D manifold_edge{ surface };
            const auto nb = manifold_edge.nb_non_manifold_edges();
            geode::Logger::info( nb, " non manifold edges" );
        } ) );
    }
    async::when_all( tasks.begin(), tasks.end() ).wait();
}

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage(
            absl::StrCat( "Surface inspector from Geode-solutions.\n",
                "Sample usage:\n", argv[0], " --input my_surface.og_tsf3d\n",
                "Default behavior test all available criteria, to disable one "
                "use --noXXX, e.g. --noadjacency" ) );
        absl::ParseCommandLine( argc, argv );

        geode::detail::initialize_mesh_io();
        const auto filename = absl::GetFlag( FLAGS_input );
        const auto ext =
            geode::to_string( geode::extension_from_filename( filename ) );

        if( geode::TriangulatedSurfaceInputFactory3D::has_creator( ext ) )
        {
            inspect_surface(
                *geode::load_triangulated_surface< 3 >( filename ) );
        }
        else if( geode::PolygonalSurfaceInputFactory3D::has_creator( ext ) )
        {
            inspect_surface(
                *geode::load_triangulated_surface< 3 >( filename ) );
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
