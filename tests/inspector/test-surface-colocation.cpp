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
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/inspector/criterion/colocation/surface_colocation.hpp>

void check_non_colocation2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, geode::Point2D{ { 0., 2. } } );
    builder->set_point( 1, geode::Point2D{ { 2., 0. } } );
    builder->set_point( 2, geode::Point2D{ { 1., 4. } } );
    builder->set_point( 3, geode::Point2D{ { 3., 3. } } );

    const geode::SurfaceMeshColocation2D colocation_inspector{ *surface };
    OPENGEODE_EXCEPTION( !colocation_inspector.mesh_has_colocated_points(),
        "[Test] Surface has colocated points when it should have none." );
    OPENGEODE_EXCEPTION(
        colocation_inspector.colocated_points_groups().nb_issues() == 0,
        "[Test] Surface has more colocated points than it should." );
}

void check_colocation2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 7 );
    builder->set_point( 0, geode::Point2D{ { 0., 2. } } );
    builder->set_point( 1, geode::Point2D{ { 0., 2. } } );
    builder->set_point( 2, geode::Point2D{ { 0., 0. } } );
    builder->set_point( 3, geode::Point2D{ { 2., 0. } } );
    builder->set_point( 4, geode::Point2D{ { 1., 4. } } );
    builder->set_point(
        5, geode::Point2D{ { 2., geode::GLOBAL_EPSILON / 2 } } );
    builder->set_point(
        6, geode::Point2D{ { geode::GLOBAL_EPSILON / 1.1, 2. } } );

    const geode::SurfaceMeshColocation2D colocation_inspector{ *surface };
    OPENGEODE_EXCEPTION( colocation_inspector.mesh_has_colocated_points(),
        "[Test] Surface doesn't have colocated points whereas it should have "
        "several." );
    const auto colocated_points_groups =
        colocation_inspector.colocated_points_groups();
    OPENGEODE_EXCEPTION( colocated_points_groups.nb_issues() == 2,
        "[Test] Surface has wrong number of colocated groups of "
        "points." );
    auto nb_colocated_points{ 0 };
    for( const auto &group : colocated_points_groups.issues() )
    {
        nb_colocated_points += group.size();
    }
    OPENGEODE_EXCEPTION( nb_colocated_points == 5,
        "[Test] Surface has wrong number of colocated points." );
    const std::vector< geode::index_t > first_colocated_points_group{ 0, 1, 6 };
    const std::vector< geode::index_t > second_colocated_points_group{ 3, 5 };
    OPENGEODE_EXCEPTION(
        colocated_points_groups.issues()[0] == first_colocated_points_group
            || colocated_points_groups.issues()[0]
                   == second_colocated_points_group,
        "[Test] Surface has wrong first colocated points group." );
    OPENGEODE_EXCEPTION(
        colocated_points_groups.issues()[1] == first_colocated_points_group
            || colocated_points_groups.issues()[1]
                   == second_colocated_points_group,
        "[Test] Surface has wrong second colocated points group." );
}

void check_non_colocation3D()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, geode::Point3D{ { 0., 2., 0. } } );
    builder->set_point( 1, geode::Point3D{ { 2., 0., 0.5 } } );
    builder->set_point( 2, geode::Point3D{ { 1., 4., 1. } } );
    builder->set_point( 3, geode::Point3D{ { 3., 3., 2. } } );

    const geode::SurfaceMeshColocation3D colocation_inspector{ *surface };
    OPENGEODE_EXCEPTION( !colocation_inspector.mesh_has_colocated_points(),
        "[Test] (3D) Surface has colocated points when it should have none." );
    OPENGEODE_EXCEPTION(
        colocation_inspector.colocated_points_groups().nb_issues() == 0,
        "[Test] (3D) Surface has more colocated points than it should." );
}

void check_colocation3D()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
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

    const geode::SurfaceMeshColocation3D colocation_inspector{ *surface };
    OPENGEODE_EXCEPTION( colocation_inspector.mesh_has_colocated_points(),
        "[Test] (3D) Surface doesn't have colocated points whereas it should "
        "have several." );
    const auto colocated_points_groups =
        colocation_inspector.colocated_points_groups();
    OPENGEODE_EXCEPTION( colocated_points_groups.nb_issues() == 2,
        "[Test] (3D) Surface has wrong number of colocated groups of "
        "points." );
    auto nb_colocated_points{ 0 };
    for( const auto &group : colocated_points_groups.issues() )
    {
        nb_colocated_points += group.size();
    }
    OPENGEODE_EXCEPTION( nb_colocated_points == 5,
        "[Test] (3D) Surface has wrong number of colocated points." );
    const std::vector< geode::index_t > first_colocated_points_group{ 0, 1, 6 };
    const std::vector< geode::index_t > second_colocated_points_group{ 3, 5 };
    OPENGEODE_EXCEPTION(
        colocated_points_groups.issues()[0] == first_colocated_points_group
            || colocated_points_groups.issues()[0]
                   == second_colocated_points_group,
        "[Test] (3D) Surface has wrong first colocated points group." );
    OPENGEODE_EXCEPTION(
        colocated_points_groups.issues()[1] == first_colocated_points_group
            || colocated_points_groups.issues()[1]
                   == second_colocated_points_group,
        "[Test] (3D) Surface has wrong second colocated points group." );
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
