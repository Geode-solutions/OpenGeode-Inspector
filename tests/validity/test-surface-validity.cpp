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
#include <geode/mesh/builder/triangulated_surface_builder.hpp>
#include <geode/mesh/core/triangulated_surface.hpp>

#include <geode/inspector/validity/object_validity.hpp>
#include <geode/inspector/validity/surface_validity.hpp>

void check_invalidity()
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

    auto object_validity = geode::is_surface_valid( *surface );
    geode::Logger::info( "ObjectValidity: \n", object_validity.string() );
    geode::OpenGeodeInspectorValidityException::test(
        object_validity.nb_issues() == 1,
        "SurfaceMesh should have 1 invalidity due to colocation, not ",
        object_validity.nb_issues() );

    builder->create_triangle( { 0, 2, 3 } );
    builder->create_triangle( { 2, 4, 5 } );

    object_validity = geode::is_surface_valid( *surface );
    geode::Logger::info( "ObjectValidity: \n", object_validity.string() );
    geode::OpenGeodeInspectorValidityException::test(
        object_validity.nb_issues() == 2,
        "SurfaceMesh should have 2 object_validity due to colocation and "
        "non manifold vertices , not ",
        object_validity.nb_issues(), "." );

    builder->create_triangle( { 2, 5, 3 } );

    object_validity = geode::is_surface_valid( *surface );
    geode::Logger::info( "ObjectValidity: \n", object_validity.string() );
    geode::OpenGeodeInspectorValidityException::test(
        object_validity.nb_issues() == 5,
        "SurfaceMesh should have 5 object_validity due to colocation, "
        "degenerated "
        "edges and polygons, and non manifold edges and vertices, not ",
        object_validity.nb_issues(), "." );

    builder->set_polygon_adjacent( { 0, 1 }, 2 );
    builder->set_polygon_adjacent( { 2, 2 }, 0 );
    builder->set_polygon_adjacent( { 2, 0 }, 1 );
    builder->set_polygon_adjacent( { 1, 2 }, 2 );

    object_validity = geode::is_surface_valid( *surface );
    geode::Logger::info( "ObjectValidity: \n", object_validity.string() );
    geode::OpenGeodeInspectorValidityException::test(
        object_validity.nb_issues() == 3,
        "SurfaceMesh should have 3 object_validity due to colocation, "
        "degenerated edges and polygons, not ",
        object_validity.nb_issues(), "." );

    builder->create_point( geode::Point3D{ { 1., -1., -1. } } );
    builder->create_triangle( { 2, 4, 7 } );

    object_validity = geode::is_surface_valid( *surface );
    geode::Logger::info( "ObjectValidity: \n", object_validity.string() );
    geode::OpenGeodeInspectorValidityException::test(
        object_validity.nb_issues() == 6,
        "SurfaceMesh should have 6 object_validity due to colocation, "
        "degenerated "
        "edges and polygons, non manifold edges and vertices, and "
        "intersections, not ",
        object_validity.nb_issues(), "." );

    builder->set_polygon_adjacent( { 1, 0 }, 3 );
    builder->set_polygon_adjacent( { 3, 0 }, 1 );

    object_validity = geode::is_surface_valid( *surface );
    geode::Logger::info( "ObjectValidity: \n", object_validity.string() );
    geode::OpenGeodeInspectorValidityException::test(
        object_validity.nb_issues() == 5,
        "SurfaceMesh should have 5 object_validity due to adjacencies, "
        "colocation, degenerated edges and polygons, and intersections, not ",
        object_validity.nb_issues(), "." );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorValidityLibrary::initialize();
        geode::Logger::set_level( geode::Logger::LEVEL::debug );
        check_invalidity();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
