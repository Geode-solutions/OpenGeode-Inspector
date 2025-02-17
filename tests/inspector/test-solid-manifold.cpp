/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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
#include <geode/mesh/core/detail/vertex_cycle.hpp>
#include <geode/mesh/core/tetrahedral_solid.hpp>

#include <geode/inspector/solid_inspector.hpp>

void check_vertex_manifold()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 5 );
    builder->set_point( 0, geode::Point3D{ { 0., 0., 2. } } );
    builder->set_point( 1, geode::Point3D{ { 3., .5, 0. } } );
    builder->set_point( 2, geode::Point3D{ { .5, 3., .5 } } );
    builder->set_point( 3, geode::Point3D{ { 2., 1.5, 3. } } );
    builder->set_point( 4, geode::Point3D{ { 3.5, 2.5, -.5 } } );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 1, 4, 2, 3 } );
    builder->set_polyhedron_adjacent( { 0, 0 }, 1 );
    builder->set_polyhedron_adjacent( { 1, 1 }, 0 );

    const geode::SolidMeshInspector3D manifold_inspector{ *solid };
    OPENGEODE_EXCEPTION( manifold_inspector.mesh_vertices_are_manifold(),
        "[Test] Solid is shown non-manifold whereas it is." );
    OPENGEODE_EXCEPTION(
        manifold_inspector.non_manifold_vertices().nb_issues() == 0,
        "[Test] Solid has more non manifold vertices than it should." );
}

void check_vertex_non_manifold()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 7 );
    builder->set_point( 0, geode::Point3D{ { 0., 0., 2. } } );
    builder->set_point( 1, geode::Point3D{ { 3., .5, 0. } } );
    builder->set_point( 2, geode::Point3D{ { .5, 3., .5 } } );
    builder->set_point( 3, geode::Point3D{ { 2., 1.5, 3. } } );
    builder->set_point( 4, geode::Point3D{ { 3.5, 2.5, -.5 } } );
    builder->set_point( 5, geode::Point3D{ { 3., .5, 0. } } );
    builder->set_point( 6, geode::Point3D{ { .5, 3., .5 } } );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 5, 4, 6, 3 } );

    const geode::SolidMeshInspector3D manifold_inspector{ *solid };
    OPENGEODE_EXCEPTION( !manifold_inspector.mesh_vertices_are_manifold(),
        "[Test] Solid is shown manifold whereas it is not." );
    OPENGEODE_EXCEPTION(
        manifold_inspector.non_manifold_vertices().nb_issues() == 1,
        "[Test] Solid has wrong number of non manifold vertices." );
    OPENGEODE_EXCEPTION(
        manifold_inspector.non_manifold_vertices().issues()[0] == 3,
        "[Test] Solid shows wrong non manifold vertex id." );
}

void check_edge_manifold()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 5 );
    builder->set_point( 0, geode::Point3D{ { 0., 0., 2. } } );
    builder->set_point( 1, geode::Point3D{ { 3., .5, 0. } } );
    builder->set_point( 2, geode::Point3D{ { .5, 3., .5 } } );
    builder->set_point( 3, geode::Point3D{ { 2., 1.5, 3. } } );
    builder->set_point( 4, geode::Point3D{ { 3.5, 2.5, -.5 } } );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 1, 4, 2, 3 } );
    builder->set_polyhedron_adjacent( { 0, 0 }, 1 );
    builder->set_polyhedron_adjacent( { 1, 1 }, 0 );

    const geode::SolidMeshInspector3D manifold_inspector{ *solid };
    OPENGEODE_EXCEPTION( manifold_inspector.mesh_edges_are_manifold(),
        "[Test] Solid is shown non-manifold whereas it is." );
    OPENGEODE_EXCEPTION(
        manifold_inspector.non_manifold_edges().nb_issues() == 0,
        "[Test] Solid has more non manifold edges than it should." );
}

void check_edge_non_manifold()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 6 );
    builder->set_point( 0, geode::Point3D{ { 0., 0., 2. } } );
    builder->set_point( 1, geode::Point3D{ { 3., .5, 0. } } );
    builder->set_point( 2, geode::Point3D{ { .5, 3., .5 } } );
    builder->set_point( 3, geode::Point3D{ { 2., 1.5, 3. } } );
    builder->set_point( 4, geode::Point3D{ { 3.5, 2.5, -.5 } } );
    builder->set_point( 5, geode::Point3D{ { 3., .5, 0. } } );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 5, 4, 2, 3 } );

    const geode::SolidMeshInspector3D manifold_inspector{ *solid };
    OPENGEODE_EXCEPTION( !manifold_inspector.mesh_edges_are_manifold(),
        "[Test] Solid is shown manifold whereas it is not." );
    OPENGEODE_EXCEPTION(
        manifold_inspector.non_manifold_edges().nb_issues() == 1,
        "[Test] Solid has wrong number of non manifold edges." );
    const auto non_manifold_e = manifold_inspector.non_manifold_edges();
    OPENGEODE_EXCEPTION( non_manifold_e.issues()[0][0] == 2
                             && non_manifold_e.issues()[0][1] == 3,
        "[Test] Solid shows wrong non manifold edge id." );
}

void check_facet_manifold()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 5 );
    builder->set_point( 0, geode::Point3D{ { 0., 0., 2. } } );
    builder->set_point( 1, geode::Point3D{ { 3., .5, 0. } } );
    builder->set_point( 2, geode::Point3D{ { .5, 3., .5 } } );
    builder->set_point( 3, geode::Point3D{ { 2., 1.5, 3. } } );
    builder->set_point( 4, geode::Point3D{ { 3.5, 2.5, -.5 } } );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 1, 4, 2, 3 } );

    const geode::SolidMeshInspector3D manifold_inspector{ *solid };
    OPENGEODE_EXCEPTION( manifold_inspector.mesh_facets_are_manifold(),
        "[Test] Solid is shown non-manifold whereas it is." );
    OPENGEODE_EXCEPTION(
        manifold_inspector.non_manifold_facets().nb_issues() == 0,
        "[Test] Solid has more non manifold facets than it should." );
}

void check_facet_non_manifold()
{
    auto solid = geode::TetrahedralSolid3D::create();
    auto builder = geode::TetrahedralSolidBuilder3D::create( *solid );
    builder->create_vertices( 6 );
    builder->set_point( 0, geode::Point3D{ { 0., 0., 2. } } );
    builder->set_point( 1, geode::Point3D{ { 3., .5, 0. } } );
    builder->set_point( 2, geode::Point3D{ { .5, 3., .5 } } );
    builder->set_point( 3, geode::Point3D{ { 2., 1.5, 3. } } );
    builder->set_point( 4, geode::Point3D{ { 3.5, 2.5, -.5 } } );
    builder->set_point( 5, geode::Point3D{ { 4., 3., -.5 } } );
    builder->create_tetrahedron( { 0, 1, 2, 3 } );
    builder->create_tetrahedron( { 1, 4, 2, 3 } );
    builder->create_tetrahedron( { 1, 5, 2, 3 } );

    const geode::SolidMeshInspector3D manifold_inspector{ *solid };
    OPENGEODE_EXCEPTION( !manifold_inspector.mesh_facets_are_manifold(),
        "[Test] Solid is shown manifold whereas it is not." );
    OPENGEODE_EXCEPTION(
        manifold_inspector.non_manifold_facets().nb_issues() == 1,
        "[Test] Solid has wrong number of non manifold facets." );
    const auto non_manifold_f = manifold_inspector.non_manifold_facets();
    OPENGEODE_EXCEPTION( non_manifold_f.issues()[0][0] == 1
                             && non_manifold_f.issues()[0][1] == 2
                             && non_manifold_f.issues()[0][2] == 3,
        "[Test] Solid shows wrong non manifold facet id." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_vertex_manifold();
        check_vertex_non_manifold();
        check_edge_manifold();
        check_edge_non_manifold();
        check_facet_manifold();
        check_facet_non_manifold();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
