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

#include <geode/inspector/inspection/surface_inspector.hpp>

template < geode::index_t dimension >
std::unique_ptr< geode::TriangulatedSurface< dimension > > create_surface()
{
    auto surface = geode::TriangulatedSurface< dimension >::create();
    auto builder =
        geode::TriangulatedSurfaceBuilder< dimension >::create( *surface );
    builder->create_vertices( 4 );
    for( const auto vertex : geode::Range{ 4 } )
    {
        geode::Point< dimension > point;
        point.set_value( 0, vertex % 2 );
        point.set_value( 1, vertex / 2 );
        builder->set_point( vertex, point );
    }
    builder->create_triangle( { 0, 1, 2 } );
    return surface;
}

template < geode::index_t dimension >
void check_no_isolated_vertices()
{
    auto surface = create_surface< dimension >();
    auto builder =
        geode::TriangulatedSurfaceBuilder< dimension >::create( *surface );
    builder->create_triangle( { 1, 3, 2 } );
    builder->compute_polygon_adjacencies();

    const geode::SurfaceMeshIsolation< dimension > isolation_inspector{
        *surface
    };
    geode::OpenGeodeInspectorInspectionException::test(
        !isolation_inspector.mesh_has_isolated_vertices(),
        "[Test] Surface is shown with isolated vertices whereas it has none." );
    geode::OpenGeodeInspectorInspectionException::test(
        isolation_inspector.isolated_vertices().nb_issues() == 0,
        "[Test] Surface has more isolated vertices than it should." );
}

template < geode::index_t dimension >
void check_isolated_vertices()
{
    const auto surface = create_surface< dimension >();

    const geode::SurfaceMeshIsolation< dimension > isolation_inspector{
        *surface
    };
    geode::OpenGeodeInspectorInspectionException::test(
        isolation_inspector.mesh_has_isolated_vertices(),
        "[Test] Surface is shown without isolated vertices whereas it has "
        "one." );
    const auto isolated_vertices = isolation_inspector.isolated_vertices();
    geode::OpenGeodeInspectorInspectionException::test(
        isolated_vertices.nb_issues() == 1,
        "[Test] Surface has wrong number of isolated vertices." );
    geode::OpenGeodeInspectorInspectionException::test(
        isolated_vertices.issues()[0] == 3,
        "[Test] Surface has wrong isolated vertices." );

    const geode::SurfaceMeshInspector< dimension > inspector{ *surface };
    geode::OpenGeodeInspectorInspectionException::test(
        inspector.inspect_surface().isolated_vertices.nb_issues() == 1,
        "[Test] Surface inspection has wrong number of isolated vertices." );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        check_no_isolated_vertices< 2 >();
        check_isolated_vertices< 2 >();
        check_no_isolated_vertices< 3 >();
        check_isolated_vertices< 3 >();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
