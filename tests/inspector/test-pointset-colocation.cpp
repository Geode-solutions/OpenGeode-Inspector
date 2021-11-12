/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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
#include <geode/mesh/builder/point_set_builder.h>
#include <geode/mesh/core/point_set.h>

#include <geode/inspector/criterion/colocation/pointset_colocation.h>

void check_non_colocation2D()
{
    auto pointset = geode::PointSet2D::create();
    auto builder = geode::PointSetBuilder2D::create( *pointset );
    builder->create_vertices( 4 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 2., 0. } } );
    builder->set_point( 2, { { 1., 4. } } );
    builder->set_point( 3, { { 3., 3. } } );

    const geode::PointSetColocation2D colocation_inspector{ *pointset };
    OPENGEODE_EXCEPTION( !colocation_inspector.mesh_has_colocated_points(),
        "[Test] PointSet has colocated points when it should have none." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 0,
        "[Test] PointSet has more colocated points than it should." );
    OPENGEODE_EXCEPTION(
        colocation_inspector.colocated_points_groups().size() == 0,
        "[Test] PointSet points are shown colocated whereas they are not." );
}

void check_colocation2D()
{
    auto pointset = geode::PointSet2D::create();
    auto builder = geode::PointSetBuilder2D::create( *pointset );
    builder->create_vertices( 7 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 0., 2. } } );
    builder->set_point( 2, { { 0., 0. } } );
    builder->set_point( 3, { { 2., 0. } } );
    builder->set_point( 4, { { 1., 4. } } );
    builder->set_point( 5, { { 2., geode::global_epsilon / 2 } } );
    builder->set_point( 6, { { geode::global_epsilon / 1.1, 2. } } );

    const geode::PointSetColocation2D colocation_inspector{ *pointset };
    OPENGEODE_EXCEPTION( colocation_inspector.mesh_has_colocated_points(),
        "[Test] PointSet doesn't have colocated points whereas it should have "
        "several." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 3,
        "[Test] PointSet has wrong number of colocated points." );
    const std::vector< geode::index_t > first_colocated_points_group{ 0, 1, 6 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[0]
                             == first_colocated_points_group,
        "[Test] PointSet has wrong first colocated points group." );
    const std::vector< geode::index_t > second_colocated_points_group{ 3, 5 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[1]
                             == second_colocated_points_group,
        "[Test] PointSet has wrong second colocated points group." );
}

void check_non_colocation3D()
{
    auto pointset = geode::PointSet3D::create();
    auto builder = geode::PointSetBuilder3D::create( *pointset );
    builder->create_vertices( 4 );
    builder->set_point( 0, { { 0., 2., 0. } } );
    builder->set_point( 1, { { 2., 0., 0.5 } } );
    builder->set_point( 2, { { 1., 4., 1. } } );
    builder->set_point( 3, { { 3., 3., 2. } } );

    const geode::PointSetColocation3D colocation_inspector{ *pointset };
    OPENGEODE_EXCEPTION( !colocation_inspector.mesh_has_colocated_points(),
        "[Test] (3D) PointSet has colocated points when it should have none." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 0,
        "[Test] (3D) PointSet has more colocated points than it should." );
    OPENGEODE_EXCEPTION(
        colocation_inspector.colocated_points_groups().size() == 0,
        "[Test] (3D) PointSet points are shown colocated whereas they are "
        "not." );
}

void check_colocation3D()
{
    auto pointset = geode::PointSet3D::create();
    auto builder = geode::PointSetBuilder3D::create( *pointset );
    builder->create_vertices( 7 );
    builder->set_point( 0, { { 0., 2., 1. } } );
    builder->set_point( 1, { { 0., 2., 1. } } );
    builder->set_point( 2, { { 0., 0., 0. } } );
    builder->set_point( 3, { { 2., 0., 0. } } );
    builder->set_point( 4, { { 1., 4., 3. } } );
    builder->set_point(
        5, { { 2., geode::global_epsilon / 2, geode::global_epsilon / 2 } } );
    builder->set_point( 6, { { geode::global_epsilon / 1.1, 2., 1. } } );

    const geode::PointSetColocation3D colocation_inspector{ *pointset };
    OPENGEODE_EXCEPTION( colocation_inspector.mesh_has_colocated_points(),
        "[Test] (3D) PointSet doesn't have colocated points whereas it should "
        "have "
        "several." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 3,
        "[Test] (3D) PointSet has wrong number of colocated points." );
    const std::vector< geode::index_t > first_colocated_points_group{ 0, 1, 6 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[0]
                             == first_colocated_points_group,
        "[Test] (3D) PointSet has wrong first colocated points group." );
    const std::vector< geode::index_t > second_colocated_points_group{ 3, 5 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[1]
                             == second_colocated_points_group,
        "[Test] (3D) PointSet has wrong second colocated points group." );
}

int main()
{
    try
    {
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
