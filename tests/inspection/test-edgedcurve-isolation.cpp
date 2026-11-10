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

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>

#include <geode/inspector/inspection/edgedcurve_inspector.hpp>

template < geode::index_t dimension >
std::unique_ptr< geode::EdgedCurve< dimension > > create_curve()
{
    auto curve = geode::EdgedCurve< dimension >::create();
    auto builder = geode::EdgedCurveBuilder< dimension >::create( *curve );
    builder->create_vertices( 4 );
    for( const auto vertex : geode::Range{ 4 } )
    {
        geode::Point< dimension > point;
        point.set_value( 0, vertex );
        point.set_value( 1, vertex % 2 );
        builder->set_point( vertex, point );
    }
    builder->create_edge( 0, 1 );
    builder->create_edge( 1, 3 );
    return curve;
}

template < geode::index_t dimension >
void check_no_isolated_vertices()
{
    auto curve = create_curve< dimension >();
    geode::EdgedCurveBuilder< dimension >::create( *curve )->create_edge(
        3, 2 );

    const geode::EdgedCurveIsolation< dimension > isolation_inspector{ *curve };
    geode::OpenGeodeInspectorInspectionException::test(
        !isolation_inspector.mesh_has_isolated_vertices(),
        "[Test] EdgedCurve is shown with isolated vertices whereas it has "
        "none." );
    geode::OpenGeodeInspectorInspectionException::test(
        isolation_inspector.isolated_vertices().nb_issues() == 0,
        "[Test] EdgedCurve has more isolated vertices than it should." );
}

template < geode::index_t dimension >
void check_isolated_vertices()
{
    const auto curve = create_curve< dimension >();

    const geode::EdgedCurveIsolation< dimension > isolation_inspector{ *curve };
    geode::OpenGeodeInspectorInspectionException::test(
        isolation_inspector.mesh_has_isolated_vertices(),
        "[Test] EdgedCurve is shown without isolated vertices whereas it has "
        "one." );
    const auto isolated_vertices = isolation_inspector.isolated_vertices();
    geode::OpenGeodeInspectorInspectionException::test(
        isolated_vertices.nb_issues() == 1,
        "[Test] EdgedCurve has wrong number of isolated vertices." );
    geode::OpenGeodeInspectorInspectionException::test(
        isolated_vertices.issues()[0] == 2,
        "[Test] EdgedCurve has wrong isolated vertices." );

    const geode::EdgedCurveInspector< dimension > inspector{ *curve };
    geode::OpenGeodeInspectorInspectionException::test(
        inspector.inspect_edged_curve().isolated_vertices.nb_issues() == 1,
        "[Test] EdgedCurve inspection has wrong number of isolated "
        "vertices." );
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
