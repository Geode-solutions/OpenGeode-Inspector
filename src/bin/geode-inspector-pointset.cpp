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

#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/io/point_set_input.h>

#include <geode/io/mesh/common.h>

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
    geode::index_t nb{ 0 };
    for( const auto& pt_group : inspector.colocated_points_groups() )
    {
        nb += pt_group.size();
    }
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

        geode::IOMeshLibrary::initialize();
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
