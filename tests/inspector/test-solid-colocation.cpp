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
#include <geode/mesh/builder/tetrahedral_solid_builder.h>
#include <geode/mesh/core/tetrahedral_solid.h>

#include <geode/inspector/criterion/colocation/solid_colocation.h>

void check_non_colocation()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 5 );
    builder->set_point( 0, { { 0., 0., 0. } } );
    builder->set_point( 1, { { 3., 3., -0.5 } } );
    builder->set_point( 2, { { -0.5, 4., -1. } } );
    builder->set_point( 3, { { 1., 3., 3. } } );
    builder->set_point( 4, { { 1., 2., -3. } } );

    const geode::SolidMeshColocation3D colocation_inspector{ *solid };
    OPENGEODE_EXCEPTION( !colocation_inspector.mesh_has_colocated_points(),
        "[Test] Solid has colocated points when it should have none." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 0,
        "[Test] Solid has more colocated points than it should." );
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups().empty(),
        "[Test] Solid points are shown colocated whereas they are "
        "not." );
}

void check_colocation()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 7 );
    builder->set_point( 0, { { 5., 2., 1. } } );
    builder->set_point( 1, { { 5., 2., 1. } } );
    builder->set_point( 2, { { 0.5, 0., 0.5 } } );
    builder->set_point( 3, { { 5., 2., 0. } } );
    builder->set_point( 4, { { 1., 4., 3. } } );
    builder->set_point( 5,
        { { 5., 2. + geode::global_epsilon / 2, geode::global_epsilon / 2 } } );
    builder->set_point( 6, { { 5. + geode::global_epsilon / 1.1, 2., 1. } } );

    const geode::SolidMeshColocation3D colocation_inspector{ *solid };
    OPENGEODE_EXCEPTION( colocation_inspector.mesh_has_colocated_points(),
        "[Test] Solid doesn't have colocated points whereas it should have "
        "several." );
    OPENGEODE_EXCEPTION( colocation_inspector.nb_colocated_points() == 5,
        "[Test] Solid has wrong number of colocated points." );
    const std::vector< geode::index_t > first_colocated_points_group{ 0, 1, 6 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[0]
                             == first_colocated_points_group,
        "[Test] Solid has wrong first colocated points group." );
    const std::vector< geode::index_t > second_colocated_points_group{ 3, 5 };
    OPENGEODE_EXCEPTION( colocation_inspector.colocated_points_groups()[1]
                             == second_colocated_points_group,
        "[Test] Solid has wrong second colocated points group." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_non_colocation();
        check_colocation();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
