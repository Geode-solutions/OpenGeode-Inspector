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

#include <geode/basic/assert.h>
#include <geode/basic/filename.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/mesh_factory.h>
#include <geode/mesh/io/edged_curve_input.h>

#include <geode/io/mesh/common.h>

#include <geode/inspector/edgedcurve_inspector.h>

ABSL_FLAG( std::string, input, "/path/my/curve.og_edc3d", "Input edged curve" );

template < geode::index_t dimension >
void inspect_edged_curve( const geode::EdgedCurve< dimension >& edged_curve )
{
    const geode::EdgedCurveInspector< dimension > inspector{ edged_curve };
    const auto result = inspector.inspect_edged_curve();

    geode::Logger::info( result.string() );
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

        geode::IOMeshLibrary::initialize();
        const auto filename = absl::GetFlag( FLAGS_input );
        const auto ext =
            geode::to_string( geode::extension_from_filename( filename ) );

        if( geode::EdgedCurveInputFactory3D::has_creator( ext ) )
        {
            inspect_edged_curve( *geode::load_edged_curve< 3 >( filename ) );
        }
        else if( geode::EdgedCurveInputFactory2D::has_creator( ext ) )
        {
            inspect_edged_curve( *geode::load_edged_curve< 2 >( filename ) );
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
