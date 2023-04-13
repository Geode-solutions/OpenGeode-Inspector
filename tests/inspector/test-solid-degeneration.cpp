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
#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/tetrahedral_solid.h>

#include <geode/inspector/criterion/degeneration/solid_degeneration.h>

void check_non_degeneration()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 5 );
    builder->set_point( 0, { { 0., 0., 0. } } );
    builder->set_point( 1, { { 3., 3., -0.5 } } );
    builder->set_point( 2, { { -0.5, 4., -1. } } );
    builder->set_point( 3, { { 1., 3., 3. } } );
    builder->set_point( 4, { { 1., 2., -3. } } );

    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 0, 1, 4, 2 } );

    const geode::SolidMeshDegeneration3D degeneration_inspector{ *solid };
    OPENGEODE_EXCEPTION( !degeneration_inspector.is_mesh_degenerated(),
        "[Test] Solid is shown degenerated whereas it is not." );
    OPENGEODE_EXCEPTION( degeneration_inspector.nb_degenerated_edges() == 0,
        "[Test] Solid has more degenerated edges than it should." );
    OPENGEODE_EXCEPTION( degeneration_inspector.degenerated_edges().empty(),
        "[Test] Solid has degenerated edges when it should have none." );
}

void check_degeneration_by_colocalisation()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 5 );
    builder->set_point( 0, { { 0., 0., 0. } } );
    builder->set_point( 1, { { 3., 3., 0. } } );
    builder->set_point( 2, { { -0.5, 4., -1. } } );
    builder->set_point( 3, { { 1., 3., 3. } } );
    builder->set_point( 4, { { 3., 3., -geode::global_epsilon / 2 } } );

    builder->create_tetrahedron( { 0, 1, 3, 2 } );
    builder->create_tetrahedron( { 0, 1, 4, 2 } );
    solid->enable_edges();

    const geode::SolidMeshDegeneration3D degeneration_inspector{ *solid };
    OPENGEODE_EXCEPTION( degeneration_inspector.is_mesh_degenerated(),
        "[Test] Solid is shown not degenerated whereas it is." );
    OPENGEODE_EXCEPTION( degeneration_inspector.nb_degenerated_edges() == 1,
        "[Test] Solid has wrong number of degenerated edges." );
    OPENGEODE_EXCEPTION( degeneration_inspector.degenerated_edges()[0]
                             == solid->edges().edge_from_vertices( { 1, 4 } ),
        "[Test] Solid has wrong degenerated edges." );
}

void check_degeneration_by_point_multiple_presence()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 5 );
    builder->set_point( 0, { { 0., 0., 0. } } );
    builder->set_point( 1, { { 3., 3., -0.5 } } );
    builder->set_point( 2, { { -0.5, 4., -1. } } );
    builder->set_point( 3, { { 1., 3., 3. } } );

    builder->create_tetrahedron( { 0, 1, 3, 2 } );
    builder->create_tetrahedron( { 0, 1, 1, 2 } );
    solid->enable_edges();

    const geode::SolidMeshDegeneration3D degeneration_inspector{ *solid };
    OPENGEODE_EXCEPTION( degeneration_inspector.is_mesh_degenerated(),
        "[Test] Solid is not shown degenerated whereas it is." );
    OPENGEODE_EXCEPTION( degeneration_inspector.nb_degenerated_edges() == 1,
        "[Test] Solid has the wrong number of degenerated edges." );
    OPENGEODE_EXCEPTION( degeneration_inspector.degenerated_edges()[0]
                             == solid->edges().edge_from_vertices( { 1, 1 } ),
        "[Test] Solid shows the wrong degenerated edges." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
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
