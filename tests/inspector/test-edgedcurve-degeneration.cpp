/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/inspector/criterion/degeneration/edgedcurve_degeneration.hpp>

void check_non_degeneration2D()
{
    auto curve = geode::EdgedCurve2D::create();
    auto builder = geode::EdgedCurveBuilder2D::create( *curve );
    builder->create_vertices( 3 );
    builder->set_point( 0, geode::Point2D{ { 0., 2. } } );
    builder->set_point( 1, geode::Point2D{ { 2., 0. } } );
    builder->set_point( 2, geode::Point2D{ { 1., 4. } } );

    builder->create_edge( 0, 1 );
    builder->create_edge( 1, 2 );
    builder->create_edge( 2, 0 );

    const geode::EdgedCurveDegeneration2D degeneration_inspector{ *curve };
    OPENGEODE_EXCEPTION( !degeneration_inspector.is_mesh_degenerated(),
        "[Test] EdgedCurve is shown degenerated whereas it is not." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.degenerated_edges().nb_issues() == 0,
        "[Test] EdgedCurve has more degenerated edges than it should." );
}

void check_degeneration_by_colocalisation2D()
{
    auto curve = geode::EdgedCurve2D::create();
    auto builder = geode::EdgedCurveBuilder2D::create( *curve );
    builder->create_vertices( 3 );
    builder->set_point( 0, geode::Point2D{ { 0., 2. } } );
    builder->set_point( 1, geode::Point2D{ { 2., 0. } } );
    builder->set_point(
        2, geode::Point2D{ { 2., geode::GLOBAL_EPSILON / 2 } } );

    builder->create_edge( 0, 1 );
    builder->create_edge( 0, 2 );
    builder->create_edge( 1, 2 );

    const geode::EdgedCurveDegeneration2D degeneration_inspector{ *curve };
    OPENGEODE_EXCEPTION( degeneration_inspector.is_mesh_degenerated(),
        "[Test] EdgedCurve is shown not degenerated whereas it is." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.degenerated_edges().nb_issues() == 1,
        "[Test] EdgedCurve has wrong number of degenerated edges." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.degenerated_edges().issues()[0] == 2,
        "[Test] EdgedCurve has wrong degenerated edges." );
}

void check_non_degeneration3D()
{
    auto curve = geode::EdgedCurve3D::create();
    auto builder = geode::EdgedCurveBuilder3D::create( *curve );
    builder->create_vertices( 3 );
    builder->set_point( 0, geode::Point3D{ { 0., 2., 0. } } );
    builder->set_point( 1, geode::Point3D{ { 2., 0., 0.5 } } );
    builder->set_point( 2, geode::Point3D{ { 1., 4., 1. } } );

    builder->create_edge( 0, 1 );
    builder->create_edge( 1, 2 );
    builder->create_edge( 2, 0 );

    const geode::EdgedCurveDegeneration3D degeneration_inspector{ *curve };
    OPENGEODE_EXCEPTION( !degeneration_inspector.is_mesh_degenerated(),
        "[Test] (3D) EdgedCurve is shown degenerated whereas it is not." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.degenerated_edges().nb_issues() == 0,
        "[Test] (3D) EdgedCurve has more degenerated edges than it should." );
}

void check_degeneration_by_colocalisation3D()
{
    auto curve = geode::EdgedCurve3D::create();
    auto builder = geode::EdgedCurveBuilder3D::create( *curve );
    builder->create_vertices( 3 );
    builder->set_point( 0, geode::Point3D{ { 0., 2., 0. } } );
    builder->set_point( 1, geode::Point3D{ { 2., 0., 0.5 } } );
    builder->set_point( 2, geode::Point3D{ { 2., geode::GLOBAL_EPSILON / 2,
                               0.5 + geode::GLOBAL_EPSILON / 2 } } );

    builder->create_edge( 0, 1 );
    builder->create_edge( 0, 2 );
    builder->create_edge( 1, 2 );

    const geode::EdgedCurveDegeneration3D degeneration_inspector{ *curve };
    OPENGEODE_EXCEPTION( degeneration_inspector.is_mesh_degenerated(),
        "[Test] (3D) EdgedCurve is shown not degenerated whereas it is." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.degenerated_edges().nb_issues() == 1,
        "[Test] (3D) EdgedCurve has wrong number of degenerated edges." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.degenerated_edges().issues()[0] == 2,
        "[Test] (3D) EdgedCurve has wrong degenerated edges." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_non_degeneration2D();
        check_degeneration_by_colocalisation2D();
        check_non_degeneration3D();
        check_degeneration_by_colocalisation3D();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
