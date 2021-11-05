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
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/surface_edges.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/inspector/criterion/degeneration.h>

#include <iostream>

void check_non_degeneration()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 2., 0. } } );
    builder->set_point( 2, { { 1., 4. } } );
    builder->set_point( 3, { { 3., 3. } } );

    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 2, 1, 3 } );

    geode::inspector::SurfaceMeshDegeneration2D degeneration_inspector;
    OPENGEODE_EXCEPTION(
        !degeneration_inspector.is_mesh_degenerated( *surface ),
        "[Test] Surface is shown degenerated whereas it is not." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.nb_degenerated_edges( *surface ) == 0,
        "[Test] Surface has more degenerated edges than it should." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.degenerated_edges( *surface ).size() == 0,
        "[Test] Surface has degenerated edges when it should have none." );
}

void check_degeneration_by_colocalisation()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 2., 0. } } );
    builder->set_point( 2, { { 1., 4. } } );
    builder->set_point( 3, { { 2., geode::global_epsilon / 2 } } );

    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 2, 1, 3 } );
    surface->enable_edges();

    geode::inspector::SurfaceMeshDegeneration2D degeneration_inspector;
    OPENGEODE_EXCEPTION( degeneration_inspector.is_mesh_degenerated( *surface ),
        "[Test] Surface is shown not degenerated whereas it is." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.nb_degenerated_edges( *surface ) == 1,
        "[Test] Surface has wrong number of degenerated edges." );
    OPENGEODE_EXCEPTION( degeneration_inspector.degenerated_edges( *surface )[0]
                             == surface->edges().edge_from_vertices( { 1, 3 } ),
        "[Test] Surface has wrong degenerated edges." );
}

void check_degeneration_by_point_multiple_presence()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 2., 0. } } );
    builder->set_point( 2, { { 1., 4. } } );

    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 2, 1 } );
    surface->enable_edges();

    geode::inspector::SurfaceMeshDegeneration2D degeneration_inspector;
    OPENGEODE_EXCEPTION( degeneration_inspector.is_mesh_degenerated( *surface ),
        "[Test] Surface is not shown degenerated whereas it is." );
    OPENGEODE_EXCEPTION(
        degeneration_inspector.nb_degenerated_edges( *surface ) == 1,
        "[Test] Surface has the wrong number of degenerated edges." );
    OPENGEODE_EXCEPTION( degeneration_inspector.degenerated_edges( *surface )[0]
                             == surface->edges().edge_from_vertices( { 1, 1 } ),
        "[Test] Surface shows the wrong degenerated edges." );
}

int main()
{
    try
    {
        check_non_degeneration();
        check_degeneration_by_colocalisation();
        check_degeneration_by_point_multiple_presence();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
