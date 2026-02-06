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

#include <geode/inspector/criterion/negative_elements/solid_negative_elements.hpp>

void check_negative_elements()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 5 );
    builder->set_point( 0, geode::Point3D{ { 0., 0., 0. } } );
    builder->set_point( 1, geode::Point3D{ { 3., 3., -0.5 } } );
    builder->set_point( 2, geode::Point3D{ { -0.5, 4., -1. } } );
    builder->set_point( 3, geode::Point3D{ { 1., 3., 3. } } );
    builder->set_point( 4, geode::Point3D{ { 1., 2., -3. } } );

    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 0, 1, 2, 4 } );

    const geode::SolidMeshNegativeElements3D inspector{ *solid };
    OPENGEODE_EXCEPTION( inspector.mesh_has_negative_elements(),
        "[Test] Solid should have negative elements." );
    const auto issues = inspector.negative_polyhedra();
    OPENGEODE_EXCEPTION( issues.nb_issues() == 1,
        "[Test] Solid should have one negative polyhedron." );
    OPENGEODE_EXCEPTION( issues.issues().at( 0 ) == 1,
        "[Test] Solid negative polyhedron should be 1." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_negative_elements();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
