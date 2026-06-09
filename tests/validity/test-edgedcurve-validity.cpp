/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>
#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>

#include <geode/inspector/validity/edgedcurve_validity.hpp>
#include <geode/inspector/validity/object_validity.hpp>

void check_invalidity()
{
    auto curve = geode::EdgedCurve3D::create();
    auto builder = geode::EdgedCurveBuilder3D::create( *curve );
    builder->create_vertices( 7 );
    builder->set_point( 0, geode::Point3D{ { 0., 2., 1. } } );
    builder->set_point( 1, geode::Point3D{ { 0., 2., 1. } } );
    builder->set_point( 2, geode::Point3D{ { 0., 0., 0. } } );
    builder->set_point( 3, geode::Point3D{ { 2., 0., 0. } } );
    builder->set_point( 4, geode::Point3D{ { 1., 4., 3. } } );
    builder->set_point( 5, geode::Point3D{ { 2., geode::GLOBAL_EPSILON / 2,
                               geode::GLOBAL_EPSILON / 2 } } );
    builder->set_point(
        6, geode::Point3D{ { geode::GLOBAL_EPSILON / 1.1, 2., 1. } } );

    const auto first_object_validity = geode::is_edged_curve_valid( *curve );
    geode::OpenGeodeInspectorValidityException::test(
        first_object_validity.nb_issues() == 1,
        "EdgedCurve should have 1 invalidity due to colocation, not ",
        first_object_validity.nb_issues() );
    geode::Logger::info( "ObjectValidity: \n", first_object_validity.string() );

    builder->create_edge( 0, 1 );
    builder->create_edge( 1, 2 );
    builder->create_edge( 2, 3 );
    builder->create_edge( 3, 4 );
    builder->create_edge( 4, 5 );
    builder->create_edge( 5, 6 );

    const auto object_validity = geode::is_edged_curve_valid( *curve );
    geode::OpenGeodeInspectorValidityException::test(
        object_validity.nb_issues() == 2,
        "EdgedCurve should have 2 invalidity reasons, not ",
        object_validity.nb_issues() );
    geode::Logger::info( "ObjectValidity: \n", object_validity.string() );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorValidityLibrary::initialize();
        check_invalidity();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
