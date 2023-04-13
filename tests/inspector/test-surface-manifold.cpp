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
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/inspector/criterion/manifold/surface_edge_manifold.h>
#include <geode/inspector/criterion/manifold/surface_vertex_manifold.h>

void check_vertex_manifold2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 4 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 2., 2. } } );
    builder->set_point( 2, { { 2., 5. } } );
    builder->set_point( 3, { { 3., 0. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 3, 2 } );
    builder->set_polygon_adjacent( { { 0, 1 } }, 1 );
    builder->set_polygon_adjacent( { { 1, 2 } }, 0 );

    const geode::SurfaceMeshVertexManifold2D manifold_inspector{ *surface };
    OPENGEODE_EXCEPTION( manifold_inspector.mesh_vertices_are_manifold(),
        "[Test] Surface is shown non-manifold whereas it is." );
    OPENGEODE_EXCEPTION( manifold_inspector.nb_non_manifold_vertices() == 0,
        "[Test] Surface has more non manifold vertices than it should." );
    OPENGEODE_EXCEPTION( manifold_inspector.non_manifold_vertices().empty(),
        "[Test] Surface vertices are shown non manifold whereas they are." );
}

void check_vertex_non_manifold2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 5 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 2., 2. } } );
    builder->set_point( 2, { { 2., 5. } } );
    builder->set_point( 3, { { 3., 0. } } );
    builder->set_point( 4, { { 5., 3. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 3, 4 } );

    const geode::SurfaceMeshVertexManifold2D manifold_inspector{ *surface };
    OPENGEODE_EXCEPTION( !manifold_inspector.mesh_vertices_are_manifold(),
        "[Test] Surface vertices are shown manifold whereas one is not." );
    OPENGEODE_EXCEPTION( manifold_inspector.nb_non_manifold_vertices() == 1,
        "[Test] Surface has wrong number of non manifold vertices." );
    OPENGEODE_EXCEPTION( manifold_inspector.non_manifold_vertices()[0] == 1,
        "[Test] Surface shows wrong non manifold vertex id." );
}

void check_edge_manifold2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 5 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 2., 2. } } );
    builder->set_point( 2, { { 2., 5. } } );
    builder->set_point( 3, { { 3., 0. } } );
    builder->set_point( 4, { { 5., 3. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 4, 2 } );
    builder->create_triangle( { 1, 3, 4 } );
    builder->set_polygon_adjacent( { { 0, 1 } }, 1 );
    builder->set_polygon_adjacent( { { 1, 2 } }, 0 );
    builder->set_polygon_adjacent( { { 1, 0 } }, 2 );
    builder->set_polygon_adjacent( { { 2, 2 } }, 1 );

    const geode::SurfaceMeshEdgeManifold2D manifold_inspector{ *surface };
    OPENGEODE_EXCEPTION( manifold_inspector.mesh_edges_are_manifold(),
        "[Test] Surface is shown non-manifold through edges whereas it is." );
    OPENGEODE_EXCEPTION( manifold_inspector.nb_non_manifold_edges() == 0,
        "[Test] Surface has more non manifold edges than it should." );
    OPENGEODE_EXCEPTION( manifold_inspector.non_manifold_edges().empty(),
        "[Test] Surface edges are shown non manifold whereas they are." );
}

void check_edge_non_manifold2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 5 );
    builder->set_point( 0, { { 0., 2. } } );
    builder->set_point( 1, { { 2., 2. } } );
    builder->set_point( 2, { { 2., 5. } } );
    builder->set_point( 3, { { 3., 0. } } );
    builder->set_point( 4, { { 5., 3. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 1, 4, 2 } );
    builder->create_triangle( { 1, 3, 2 } );
    builder->set_polygon_adjacent( { { 0, 1 } }, 1 );
    builder->set_polygon_adjacent( { { 1, 2 } }, 0 );
    builder->set_polygon_adjacent( { { 2, 2 } }, 0 );

    const geode::SurfaceMeshEdgeManifold2D manifold_inspector{ *surface };
    OPENGEODE_EXCEPTION( !manifold_inspector.mesh_edges_are_manifold(),
        "[Test] Surface is shown manifold through edges whereas it is not." );
    OPENGEODE_EXCEPTION( manifold_inspector.nb_non_manifold_edges() == 1,
        "[Test] Surface has wrong number of non manifold edges." );
    const std::array< geode::index_t, 2 > pt1_pt2_edge{ 1, 2 };
    OPENGEODE_EXCEPTION(
        manifold_inspector.non_manifold_edges()[0] == pt1_pt2_edge,
        "[Test] Surface edges are shown non manifold whereas they are." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_vertex_manifold2D();
        check_vertex_non_manifold2D();
        check_edge_manifold2D();
        check_edge_non_manifold2D();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
