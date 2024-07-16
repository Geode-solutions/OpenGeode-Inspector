/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/basic/assert.hpp>
#include <geode/basic/filename.hpp>
#include <geode/basic/logger.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/mesh_factory.hpp>
#include <geode/mesh/core/polygonal_surface.hpp>
#include <geode/mesh/core/regular_grid_surface.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/io/edged_curve_input.hpp>
#include <geode/mesh/io/polygonal_surface_input.hpp>
#include <geode/mesh/io/regular_grid_input.hpp>
#include <geode/mesh/io/triangulated_surface_input.hpp>

#include <geode/io/mesh/common.hpp>

#include <geode/inspector/criterion/intersections/surface_curve_intersections.hpp>

ABSL_FLAG( std::string,
    surface,
    "/path/my/surface.og_tsf3d",
    "Input triangulated surface" );
ABSL_FLAG( std::string, curve, "/path/my/curve.og_edc3d", "Input curve" );

template < geode::index_t dimension >
void inspect_surface_curve(
    const geode::TriangulatedSurface< dimension >& surface,
    const geode::EdgedCurve< dimension >& curve )
{
    const geode::SurfaceCurveIntersections< dimension > inspector{ surface,
        curve };
    const auto result = inspector.intersecting_elements();
    geode::Logger::info( result.string() );
}

int main( int argc, char* argv[] )
{
    try
    {
        absl::SetProgramUsageMessage( absl::StrCat(
            "Surface-Curve intersections inspector from Geode-solutions.\n",
            "Sample usage:\n", argv[0],
            " --surface my_surface.og_tsf3d --curve my_curve.og_edc3d\n" ) );
        absl::ParseCommandLine( argc, argv );

        geode::IOMeshLibrary::initialize();
        const auto filename_surf = absl::GetFlag( FLAGS_surface );
        const auto filename_curv = absl::GetFlag( FLAGS_curve );
        const auto ext_surf =
            geode::to_string( geode::extension_from_filename( filename_surf ) );

        if( geode::TriangulatedSurfaceInputFactory3D::has_creator( ext_surf ) )
        {
            inspect_surface_curve(
                *geode::load_triangulated_surface< 3 >( filename_surf ),
                *geode::load_edged_curve< 3 >( filename_curv ) );
        }
        else if( geode::TriangulatedSurfaceInputFactory2D::has_creator(
                     ext_surf ) )
        {
            inspect_surface_curve(
                *geode::load_triangulated_surface< 2 >( filename_surf ),
                *geode::load_edged_curve< 2 >( filename_curv ) );
        }
        else
        {
            throw geode::OpenGeodeException(
                "Unable to load as TrigulatedSurface file ", filename_surf );
        }

        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
