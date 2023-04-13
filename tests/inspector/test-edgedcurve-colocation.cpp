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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>
#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/core/edged_curve.h>

#include <geode/inspector/criterion/colocation/edgedcurve_colocation.h>

void check_non_colocation2D()
{
    auto curve = geode::EdgedCurve2D::create();
    auto builder = geode::EdgedCurveBuilder2D::create( *curve );
    builder->create_vertices( 4 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 2., 0. } } );
    builder->set_point( 2, { { 1., 4. } } );
    builder->set_point( 3, { { 3., 3. } } );

    const geode::EdgedCurveColocation2D colocation_inspector{ *curve };
    OPENGEODE_EXCEPTION( !colocation_inspector.mesh_has_colocated_points(),
        "[Test] EdgedCurve has colocated points when it should have none." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 0,
        "[Test] EdgedCurve has more colocated points than it should." );
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups().empty(),
        "[Test] EdgedCurve points are shown colocated whereas they are not." );
}

void check_colocation2D()
{
    auto curve = geode::EdgedCurve2D::create();
    auto builder = geode::EdgedCurveBuilder2D::create( *curve );
    builder->create_vertices( 7 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 0., 2. } } );
    builder->set_point( 2, { { 0., 0. } } );
    builder->set_point( 3, { { 2., 0. } } );
    builder->set_point( 4, { { 1., 4. } } );
    builder->set_point( 5, { { 2., geode::global_epsilon / 2 } } );
    builder->set_point( 6, { { geode::global_epsilon / 1.1, 2. } } );

    const geode::EdgedCurveColocation2D colocation_inspector{ *curve };
    OPENGEODE_EXCEPTION( colocation_inspector.mesh_has_colocated_points(),
        "[Test] EdgedCurve doesn't have colocated points whereas it should "
        "have several." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 5,
        "[Test] EdgedCurve has wrong number of colocated points." );
    const std::vector< geode::index_t > first_colocated_points_group{ 0, 1, 6 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[0]
                             == first_colocated_points_group,
        "[Test] EdgedCurve has wrong first colocated points group." );
    const std::vector< geode::index_t > second_colocated_points_group{ 3, 5 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[1]
                             == second_colocated_points_group,
        "[Test] EdgedCurve has wrong second colocated points group." );
}

void check_non_colocation3D()
{
    auto curve = geode::EdgedCurve3D::create();
    auto builder = geode::EdgedCurveBuilder3D::create( *curve );
    builder->create_vertices( 4 );
    builder->set_point( 0, { { 0., 2., 0. } } );
    builder->set_point( 1, { { 2., 0., 0.5 } } );
    builder->set_point( 2, { { 1., 4., 1. } } );
    builder->set_point( 3, { { 3., 3., 2. } } );

    const geode::EdgedCurveColocation3D colocation_inspector{ *curve };
    OPENGEODE_EXCEPTION( !colocation_inspector.mesh_has_colocated_points(),
        "[Test] (3D) EdgedCurve has colocated points when it should have "
        "none." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 0,
        "[Test] (3D) EdgedCurve has more colocated points than it should." );
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups().empty(),
        "[Test] (3D) EdgedCurve points are shown colocated whereas they are "
        "not." );
}

void check_colocation3D()
{
    auto curve = geode::EdgedCurve3D::create();
    auto builder = geode::EdgedCurveBuilder3D::create( *curve );
    builder->create_vertices( 7 );
    builder->set_point( 0, { { 0., 2., 1. } } );
    builder->set_point( 1, { { 0., 2., 1. } } );
    builder->set_point( 2, { { 0., 0., 0. } } );
    builder->set_point( 3, { { 2., 0., 0. } } );
    builder->set_point( 4, { { 1., 4., 3. } } );
    builder->set_point(
        5, { { 2., geode::global_epsilon / 2, geode::global_epsilon / 2 } } );
    builder->set_point( 6, { { geode::global_epsilon / 1.1, 2., 1. } } );

    const geode::EdgedCurveColocation3D colocation_inspector{ *curve };
    OPENGEODE_EXCEPTION( colocation_inspector.mesh_has_colocated_points(),
        "[Test] (3D) EdgedCurve doesn't have colocated points whereas it "
        "should have several." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 5,
        "[Test] (3D) EdgedCurve has wrong number of colocated points." );
    const std::vector< geode::index_t > first_colocated_points_group{ 0, 1, 6 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[0]
                             == first_colocated_points_group,
        "[Test] (3D) EdgedCurve has wrong first colocated points group." );
    const std::vector< geode::index_t > second_colocated_points_group{ 3, 5 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[1]
                             == second_colocated_points_group,
        "[Test] (3D) EdgedCurve has wrong second colocated points group." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_non_colocation2D();
        check_colocation2D();
        check_non_colocation3D();
        check_colocation3D();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
