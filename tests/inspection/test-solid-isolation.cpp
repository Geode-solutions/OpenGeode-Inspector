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

#include <geode/mesh/builder/tetrahedral_solid_builder.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>

#include <geode/inspector/inspection/solid_inspector.hpp>

std::unique_ptr< geode::TetrahedralSolid3D > create_solid()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_point( geode::Point3D{ { 0., 0., 2. } } );
    builder->create_point( geode::Point3D{ { 3., .5, 0. } } );
    builder->create_point( geode::Point3D{ { .5, 3., .5 } } );
    builder->create_point( geode::Point3D{ { 3.5, 2.5, -.5 } } );
    builder->create_point( geode::Point3D{ { 2., 1.5, 3. } } );
    builder->create_tetrahedron( { 0, 1, 2, 4 } );
    return solid;
}

void check_no_isolated_vertices()
{
    auto solid = create_solid();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_tetrahedron( { 1, 3, 2, 4 } );
    builder->compute_polyhedron_adjacencies();

    const geode::SolidMeshIsolation3D isolation_inspector{ *solid };
    geode::OpenGeodeInspectorInspectionException::test(
        !isolation_inspector.mesh_has_isolated_vertices(),
        "[Test] Solid is shown with isolated vertices whereas it has none." );
    geode::OpenGeodeInspectorInspectionException::test(
        isolation_inspector.isolated_vertices().nb_issues() == 0,
        "[Test] Solid has more isolated vertices than it should." );
}

void check_isolated_vertices()
{
    const auto solid = create_solid();

    const geode::SolidMeshIsolation3D isolation_inspector{ *solid };
    geode::OpenGeodeInspectorInspectionException::test(
        isolation_inspector.mesh_has_isolated_vertices(),
        "[Test] Solid is shown without isolated vertices whereas it has "
        "one." );
    const auto isolated_vertices = isolation_inspector.isolated_vertices();
    geode::OpenGeodeInspectorInspectionException::test(
        isolated_vertices.nb_issues() == 1,
        "[Test] Solid has wrong number of isolated vertices." );
    geode::OpenGeodeInspectorInspectionException::test(
        isolated_vertices.issues()[0] == 3,
        "[Test] Solid has wrong isolated vertices." );

    const geode::SolidMeshInspector3D inspector{ *solid };
    geode::OpenGeodeInspectorInspectionException::test(
        inspector.inspect_solid().isolated_vertices.nb_issues() == 1,
        "[Test] Solid inspection has wrong number of isolated vertices." );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        check_no_isolated_vertices();
        check_isolated_vertices();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
