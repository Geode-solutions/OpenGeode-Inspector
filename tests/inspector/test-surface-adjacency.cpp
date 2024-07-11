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

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/inspector/criterion/adjacency/surface_adjacency.h>

void check_adjacency2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, geode::Point2D{ { 0., 1. } } );
    builder->set_point( 1, geode::Point2D{ { 3., 0. } } );
    builder->set_point( 2, geode::Point2D{ { 2., 4. } } );
    builder->set_point( 3, geode::Point2D{ { 5., 3. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 2, 1, 3 } );
    builder->set_polygon_adjacent( { 0, 1 }, 1 );
    builder->set_polygon_adjacent( { 1, 0 }, 0 );

    const geode::SurfaceMeshAdjacency2D adjacency_inspector{ *surface };
    OPENGEODE_EXCEPTION( !adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] Surface has wrong adjacencies when it should have none." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().nb_issues()
            == 0,
        "[Test] Surface has more wrong adjacencies on edges than it should." );
}

void check_non_adjacency_no_bijection2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 5 );
    builder->set_point( 0, geode::Point2D{ { 0., 1. } } );
    builder->set_point( 1, geode::Point2D{ { 3., 0. } } );
    builder->set_point( 2, geode::Point2D{ { 2., 4. } } );
    builder->set_point( 3, geode::Point2D{ { 5., 3. } } );
    builder->set_point( 4, geode::Point2D{ { 1., 4. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 2, 1, 3 } );
    builder->create_triangle( { 4, 1, 2 } );
    builder->set_polygon_adjacent( { 0, 1 }, 1 );
    builder->set_polygon_adjacent( { 1, 0 }, 0 );
    builder->set_polygon_adjacent( { 2, 1 }, 1 );

    const geode::SurfaceMeshAdjacency2D adjacency_inspector{ *surface };
    OPENGEODE_EXCEPTION( adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] Surface should have a wrong adjacency due to non-bijection." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().nb_issues()
            == 1,
        "[Test] Surface should have one wrong adjacency due to "
        "non-bijection." );
    const geode::PolygonEdge polygon_edge{ 2, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[0]
            == polygon_edge,
        "[Test] Surface edges show wrong adjacency problems." );
}

void check_non_adjacency_wrong_edge2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, geode::Point2D{ { 0., 1. } } );
    builder->set_point( 1, geode::Point2D{ { 3., 0. } } );
    builder->set_point( 2, geode::Point2D{ { 2., 4. } } );
    builder->set_point( 3, geode::Point2D{ { 5., 3. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 2, 1, 3 } );
    builder->set_polygon_adjacent( { 0, 1 }, 1 );
    builder->set_polygon_adjacent( { 1, 1 }, 0 );

    const geode::SurfaceMeshAdjacency2D adjacency_inspector{ *surface };
    OPENGEODE_EXCEPTION( adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] Surface should have wrong adjacencies due to wrong edge for "
        "adjacency." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().nb_issues()
            == 2,
        "[Test] Surface should have two wrong adjacencies due to wrong edge "
        "for adjacency." );
    const geode::PolygonEdge polygon_edge1{ 0, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[0]
            == polygon_edge1,
        "[Test] Surface shows wrong first edge with adjacency problems." );
    const geode::PolygonEdge polygon_edge2{ 1, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[1]
            == polygon_edge2,
        "[Test] Surface shows wrong second edge with adjacency problems." );
}

void check_non_adjacency_inversed_triangle2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, geode::Point2D{ { 0., 1. } } );
    builder->set_point( 1, geode::Point2D{ { 3., 0. } } );
    builder->set_point( 2, geode::Point2D{ { 2., 4. } } );
    builder->set_point( 3, geode::Point2D{ { 5., 3. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 2, 3 } );
    builder->set_polygon_adjacent( { 0, 1 }, 1 );
    builder->set_polygon_adjacent( { 1, 0 }, 0 );

    const geode::SurfaceMeshAdjacency2D adjacency_inspector{ *surface };
    OPENGEODE_EXCEPTION( adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] Surface should have wrong adjacencies due to an inversed "
        "triangle." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().nb_issues()
            == 2,
        "[Test] Surface should have two wrong adjacencies due to an inversed "
        "triangle." );
    const geode::PolygonEdge polygon_edge1{ 0, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[0]
            == polygon_edge1,
        "[Test] Surface shows wrong first edge with adjacency problems due to "
        "an inversed triangle.." );
    const geode::PolygonEdge polygon_edge2{ 1, 0 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[1]
            == polygon_edge2,
        "[Test] Surface shows wrong second edge with adjacency problems due to "
        "an inversed triangle.." );
}

void check_adjacency3D()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, geode::Point3D{ { 0., 1., 0. } } );
    builder->set_point( 1, geode::Point3D{ { 3., 0., 1. } } );
    builder->set_point( 2, geode::Point3D{ { 2., 4., 2. } } );
    builder->set_point( 3, geode::Point3D{ { 5., 3., 0. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 2, 1, 3 } );
    builder->set_polygon_adjacent( { 0, 1 }, 1 );
    builder->set_polygon_adjacent( { 1, 0 }, 0 );

    const geode::SurfaceMeshAdjacency3D adjacency_inspector{ *surface };
    OPENGEODE_EXCEPTION( !adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] 3D Surface has wrong adjacencies when it should have none." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().nb_issues()
            == 0,
        "[Test] 3D Surface has more wrong adjacencies on edges than it "
        "should." );
}

void check_non_adjacency_no_bijection3D()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 5 );
    builder->set_point( 0, geode::Point3D{ { 0., 1., 0. } } );
    builder->set_point( 1, geode::Point3D{ { 3., 0., 1. } } );
    builder->set_point( 2, geode::Point3D{ { 2., 4., 2. } } );
    builder->set_point( 3, geode::Point3D{ { 5., 3., 0. } } );
    builder->set_point( 4, geode::Point3D{ { 1., 4., 4. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 2, 1, 3 } );
    builder->create_triangle( { 4, 1, 2 } );
    builder->set_polygon_adjacent( { 0, 1 }, 1 );
    builder->set_polygon_adjacent( { 1, 0 }, 0 );
    builder->set_polygon_adjacent( { 2, 1 }, 1 );

    const geode::SurfaceMeshAdjacency3D adjacency_inspector{ *surface };
    OPENGEODE_EXCEPTION( adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] 3D Surface should have a wrong adjacency due to "
        "non-bijection." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().nb_issues()
            == 1,
        "[Test] 3D Surface should have one wrong adjacency due to "
        "non-bijection." );
    const geode::PolygonEdge polygon_edge{ 2, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[0]
            == polygon_edge,
        "[Test] 3D Surface edges show wrong adjacency problems." );
}

void check_non_adjacency_wrong_edge3D()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, geode::Point3D{ { 0., 1., 0. } } );
    builder->set_point( 1, geode::Point3D{ { 3., 0., 1. } } );
    builder->set_point( 2, geode::Point3D{ { 2., 4., 2. } } );
    builder->set_point( 3, geode::Point3D{ { 5., 3., 0. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 2, 1, 3 } );
    builder->set_polygon_adjacent( { 0, 1 }, 1 );
    builder->set_polygon_adjacent( { 1, 1 }, 0 );

    const geode::SurfaceMeshAdjacency3D adjacency_inspector{ *surface };
    OPENGEODE_EXCEPTION( adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] 3D Surface should have wrong adjacencies due to wrong edge for "
        "adjacency." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().nb_issues()
            == 2,
        "[Test] 3D Surface should have two wrong adjacencies due to wrong edge "
        "for adjacency." );
    const geode::PolygonEdge polygon_edge1{ 0, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[0]
            == polygon_edge1,
        "[Test] 3D Surface shows wrong first edge with adjacency problems." );
    const geode::PolygonEdge polygon_edge2{ 1, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[1]
            == polygon_edge2,
        "[Test] Surface shows wrong second edge with adjacency problems." );
}

void check_non_adjacency_inversed_triangle3D()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, geode::Point3D{ { 0., 1., 0. } } );
    builder->set_point( 1, geode::Point3D{ { 3., 0., 1. } } );
    builder->set_point( 2, geode::Point3D{ { 2., 4., 2. } } );
    builder->set_point( 3, geode::Point3D{ { 5., 3., 0. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 2, 3 } );
    builder->set_polygon_adjacent( { 0, 1 }, 1 );
    builder->set_polygon_adjacent( { 1, 0 }, 0 );

    const geode::SurfaceMeshAdjacency3D adjacency_inspector{ *surface };
    OPENGEODE_EXCEPTION( adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] 3D Surface should have wrong adjacencies due to an inversed "
        "triangle." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().nb_issues()
            == 2,
        "[Test] 3D Surface should have two wrong adjacencies due to an "
        "inversed triangle." );
    const geode::PolygonEdge polygon_edge1{ 0, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[0]
            == polygon_edge1,
        "[Test] 3D Surface shows wrong first edge with adjacency problems due "
        "to an inversed triangle.." );
    const geode::PolygonEdge polygon_edge2{ 1, 0 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polygon_edges_with_wrong_adjacency().issues()[1]
            == polygon_edge2,
        "[Test] 3D Surface shows wrong second edge with adjacency problems due "
        "to an inversed triangle.." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_adjacency2D();
        check_non_adjacency_no_bijection2D();
        check_non_adjacency_wrong_edge2D();
        check_non_adjacency_inversed_triangle2D();
        check_adjacency3D();
        check_non_adjacency_no_bijection3D();
        check_non_adjacency_wrong_edge3D();
        check_non_adjacency_inversed_triangle3D();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
