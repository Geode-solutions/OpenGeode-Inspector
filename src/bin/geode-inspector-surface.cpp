/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/polygonal_surface.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/io/polygonal_surface_input.h>
#include <geode/mesh/io/triangulated_surface_input.h>

#include <geode/io/mesh/detail/common.h>

#include <geode/inspector/surface_inspector.h>

ABSL_FLAG( std::string, input, "/path/my/surface.og_tfs3d", "Input surface" );
ABSL_FLAG( bool, adjacency, true, "Toggle adjacency criterion" );
ABSL_FLAG( bool, colocation, true, "Toggle colocation criterion" );
ABSL_FLAG( bool, degeneration, true, "Toggle degeneration criterion" );
ABSL_FLAG( bool, manifold_vertex, true, "Toggle manifold vertex criterion" );
ABSL_FLAG( bool, manifold_edge, true, "Toggle manifold edge criterion" );
ABSL_FLAG( bool,
    verbose,
    false,
    "Toggle verbose mode for the inspection of topology through unique "
    "vertices" );

template < geode::index_t dimension >
void inspect_surface( const geode::SurfaceMesh< dimension >& surface )
{
    const auto verbose = absl::GetFlag( FLAGS_verbose );
    absl::InlinedVector< async::task< void >, 5 > tasks;
    const geode::SurfaceMeshInspector< dimension > inspector{ surface,
        verbose };
    if( absl::GetFlag( FLAGS_adjacency ) )
    {
        tasks.emplace_back( async::spawn( [&inspector] {
            const auto nb = inspector.nb_edges_with_wrong_adjacency();
            geode::Logger::info( nb, " edges with wrong adjacency" );
        } ) );
    }
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
        else if( geode::TriangulatedSurfaceInputFactory2D::has_creator( ext ) )
        {
            inspect_surface(
                *geode::load_triangulated_surface< 2 >( filename ) );
        }
        else if( geode::PolygonalSurfaceInputFactory2D::has_creator( ext ) )
        {
            inspect_surface(
                *geode::load_triangulated_surface< 2 >( filename ) );
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
