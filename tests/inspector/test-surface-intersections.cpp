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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/inspector/criterion/intersections/surface_intersections.hpp>

void check_intersections2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 5 );
    builder->set_point( 0, geode::Point2D{ { 0., 0. } } );
    builder->set_point( 1, geode::Point2D{ { 3., 0. } } );
    builder->set_point( 2, geode::Point2D{ { 0., 4. } } );
    builder->set_point( 3, geode::Point2D{ { 3., 4. } } );
    builder->set_point( 4, geode::Point2D{ { -1., 2. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 0, 1, 3 } );
    builder->create_triangle( { 1, 3, 4 } );
    builder->set_polygon_adjacent( { 0, 0 }, 1 );
    builder->set_polygon_adjacent( { 1, 0 }, 0 );
    builder->set_polygon_adjacent( { 1, 1 }, 2 );
    builder->set_polygon_adjacent( { 2, 0 }, 1 );

    const geode::SurfaceMeshIntersections2D intersections_inspector{ *surface };
    OPENGEODE_EXCEPTION( intersections_inspector.mesh_has_self_intersections(),
        "[Test] 2D Surface should have intersections." );
    const auto inspection = intersections_inspector.intersecting_elements();
    OPENGEODE_EXCEPTION( inspection.nb_issues() == 3,
        "[Test] 2D Surface should have 3 intersecting elements pair." );
    bool right_intersections{ true };
    const auto &triangles_inter = inspection.issues();
    if( triangles_inter.size() != 3 || triangles_inter[0].first != 2
        || triangles_inter[0].second != 0 || triangles_inter[1].first != 2
        || triangles_inter[1].second != 1 || triangles_inter[2].first != 0
        || triangles_inter[2].second != 1 )
    {
        right_intersections = false;
    }
    OPENGEODE_EXCEPTION( right_intersections,
        "[Test] 2D Surface has wrong intersecting elements pairs." );
}

void check_intersections3D()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 7 );
    builder->set_point( 0, geode::Point3D{ { 0., 0., 0. } } );
    builder->set_point( 1, geode::Point3D{ { 0., 0., 3. } } );
    builder->set_point( 2, geode::Point3D{ { 0., 2., 0. } } );
    builder->set_point( 3, geode::Point3D{ { 2., 0., 0. } } );
    builder->set_point( 4, geode::Point3D{ { 5., 0., 1.5 } } );
    builder->set_point( 5, geode::Point3D{ { 2., 0.5, 2. } } );
    builder->set_point( 6, geode::Point3D{ { 0., 0., 1.5 } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 0, 2, 3 } );
    builder->create_triangle( { 3, 5, 2 } );
    builder->create_triangle( { 5, 2, 4 } );
    builder->create_triangle( { 4, 2, 6 } );
    builder->set_polygon_adjacent( { 0, 2 }, 1 );
    builder->set_polygon_adjacent( { 1, 0 }, 0 );
    builder->set_polygon_adjacent( { 1, 1 }, 2 );
    builder->set_polygon_adjacent( { 2, 2 }, 1 );
    builder->set_polygon_adjacent( { 2, 1 }, 3 );
    builder->set_polygon_adjacent( { 3, 0 }, 2 );

    const geode::SurfaceMeshIntersections3D intersections_inspector{ *surface };
    OPENGEODE_EXCEPTION( intersections_inspector.mesh_has_self_intersections(),
        "[Test] 3D Surface should have intersections." );
    const auto inspection = intersections_inspector.intersecting_elements();
    OPENGEODE_EXCEPTION( inspection.nb_issues() == 2,
        "[Test] 3D Surface should have 2 intersecting elements pair, not ",
        inspection.nb_issues(), "." );
    bool right_intersections{ true };
    const auto &triangles_inter = inspection.issues();
    if( triangles_inter.size() != 2 || triangles_inter[0].first != 0
        || triangles_inter[0].second != 4 || triangles_inter[1].first != 2
        || triangles_inter[1].second != 4 )
    {
        right_intersections = false;
    }
    OPENGEODE_EXCEPTION( right_intersections,
        "[Test] 3D Surface has wrong intersecting elements pairs." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_intersections2D();
        check_intersections3D();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
