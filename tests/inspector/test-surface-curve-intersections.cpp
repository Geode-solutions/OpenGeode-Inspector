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

#include <absl/container/flat_hash_set.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/geometry/point.h>
#include <geode/mesh/builder/edged_curve_builder.h>
#include <geode/mesh/builder/triangulated_surface_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/triangulated_surface.h>

#include <geode/inspector/criterion/intersections/surface_curve_intersections.h>

void check_intersections2D()
{
    auto surface = geode::TriangulatedSurface2D::create();
    auto builder = geode::TriangulatedSurfaceBuilder2D::create( *surface );
    builder->create_vertices( 5 );
    builder->set_point( 0, { { 0., 0. } } );
    builder->set_point( 1, { { 3., -1. } } );
    builder->set_point( 2, { { 3., 3. } } );
    builder->set_point( 3, { { 0., -3. } } );
    builder->set_point( 4, { { 3., -3. } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 0, 1, 3 } );
    builder->create_triangle( { 1, 3, 4 } );
    builder->set_polygon_adjacent( { { 0, 0 } }, 1 );
    builder->set_polygon_adjacent( { { 1, 0 } }, 0 );
    builder->set_polygon_adjacent( { { 1, 1 } }, 2 );
    builder->set_polygon_adjacent( { { 2, 0 } }, 1 );

    auto curve = geode::EdgedCurve2D::create();
    auto curve_builder = geode::EdgedCurveBuilder2D::create( *curve );
    curve_builder->create_vertices( 8 );
    curve_builder->set_point( 0, { { 5., 0. } } );
    curve_builder->set_point( 1, { { 3., 0. } } );
    curve_builder->set_point( 2, { { 2., 0. } } );
    curve_builder->set_point( 3, { { 1., 0. } } );
    curve_builder->set_point( 4, { { -1., 0. } } );
    curve_builder->set_point( 5, { { 0., -3. } } );
    curve_builder->set_point( 6, { { 3., -3. } } );
    curve_builder->set_point( 7, { { 1.5, -2. } } );
    curve_builder->create_edge( 0, 1 );
    curve_builder->create_edge( 1, 2 );
    curve_builder->create_edge( 2, 3 );
    curve_builder->create_edge( 3, 4 );
    curve_builder->create_edge( 4, 5 );
    curve_builder->create_edge( 5, 6 );
    curve_builder->create_edge( 6, 7 );

    const geode::SurfaceCurveIntersections2D intersections_inspector{ *surface,
        *curve };
    OPENGEODE_EXCEPTION( intersections_inspector.meshes_have_intersections(),
        "[Test] 2D Surface and Curve should have intersections." );
    OPENGEODE_EXCEPTION(
        intersections_inspector.nb_intersecting_elements_pair() == 7,
        "[Test] 2D Surface and Curve should have 7 intersecting elements "
        "pair, get ",
        intersections_inspector.nb_intersecting_elements_pair() );
    absl::flat_hash_set< std::pair< geode::index_t, geode::index_t > > answer{
        { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 1, 3 }, { 1, 6 }, { 2, 6 }
    };
    for( const auto& inter : intersections_inspector.intersecting_elements() )
    {
        OPENGEODE_EXCEPTION( answer.contains( inter ),
            "[Test] 2D Surface and Curve has at least one wrong intersecting "
            "elements pair: (triangle = ",
            inter.first, ", edge = ", inter.second, ")" );
    }
}

void check_intersections3D()
{
    auto surface = geode::TriangulatedSurface3D::create();
    auto builder = geode::TriangulatedSurfaceBuilder3D::create( *surface );
    builder->create_vertices( 5 );
    builder->set_point( 0, { { 0., 0., 0 } } );
    builder->set_point( 1, { { 3., -1., 0 } } );
    builder->set_point( 2, { { 3., 3., 0 } } );
    builder->set_point( 3, { { 0., -3., 0 } } );
    builder->set_point( 4, { { 3., -3., 0 } } );
    builder->create_triangle( { 0, 1, 2 } );
    builder->create_triangle( { 0, 1, 3 } );
    builder->create_triangle( { 1, 3, 4 } );
    builder->set_polygon_adjacent( { { 0, 0 } }, 1 );
    builder->set_polygon_adjacent( { { 1, 0 } }, 0 );
    builder->set_polygon_adjacent( { { 1, 1 } }, 2 );
    builder->set_polygon_adjacent( { { 2, 0 } }, 1 );

    auto curve = geode::EdgedCurve3D::create();
    auto curve_builder = geode::EdgedCurveBuilder3D::create( *curve );
    curve_builder->create_vertices( 9 );
    curve_builder->set_point( 0, { { 5., 0., 1 } } );
    curve_builder->set_point( 1, { { 3., 0., 0 } } );
    curve_builder->set_point( 2, { { 2., 0., 0 } } );
    curve_builder->set_point( 3, { { 1., 0., 0 } } );
    curve_builder->set_point( 4, { { -1., 0., 0 } } );
    curve_builder->set_point( 5, { { 0., -3., 0 } } );
    curve_builder->set_point( 6, { { 3., -3., 0 } } );
    curve_builder->set_point( 7, { { 1.5, -2., 2 } } );
    curve_builder->set_point( 8, { { 2, -2., -2 } } );
    curve_builder->create_edge( 0, 1 );
    curve_builder->create_edge( 1, 2 );
    curve_builder->create_edge( 2, 3 );
    curve_builder->create_edge( 3, 4 );
    curve_builder->create_edge( 4, 5 );
    curve_builder->create_edge( 5, 6 );
    curve_builder->create_edge( 6, 7 );
    curve_builder->create_edge( 7, 8 );

    const geode::SurfaceCurveIntersections3D intersections_inspector{ *surface,
        *curve };
    OPENGEODE_EXCEPTION( intersections_inspector.meshes_have_intersections(),
        "[Test] 3D Surface and Curve should have intersections." );
    OPENGEODE_EXCEPTION(
        intersections_inspector.nb_intersecting_elements_pair() == 6,
        "[Test] 3D Surface and Curve should have 6 intersecting elements "
        "pair, get ",
        intersections_inspector.nb_intersecting_elements_pair() );
    absl::flat_hash_set< std::pair< geode::index_t, geode::index_t > > answer{
        { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 }, { 1, 3 }, { 1, 6 }, { 2, 7 }
    };
    for( const auto& inter : intersections_inspector.intersecting_elements() )
    {
        OPENGEODE_EXCEPTION( answer.contains( inter ),
            "[Test] 3D Surface and Curve has at least one wrong intersecting "
            "elements pair: (triangle = ",
            inter.first, ", edge = ", inter.second, ")" );
    }
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
