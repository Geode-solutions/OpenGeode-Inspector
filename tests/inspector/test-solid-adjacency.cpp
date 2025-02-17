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

void check_adjacency()
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

    const geode::SolidMeshInspector3D adjacency_inspector{ *solid };
    OPENGEODE_EXCEPTION( !adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] Solid shows wrong adjacencies where there are none." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.non_manifold_facets().nb_issues() == 0,
        "[Test] Solid has more wrong adjacencies than it should." );
}

void check_non_adjacency_no_bijection()
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
    builder->set_polyhedron_adjacent( { 0, 0 }, 1 );
    builder->set_polyhedron_adjacent( { 1, 1 }, 0 );
    builder->set_polyhedron_adjacent( { 2, 1 }, 0 );

    const geode::SolidMeshInspector3D adjacency_inspector{ *solid };
    OPENGEODE_EXCEPTION( adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] Solid should have a wrong adjacency due to non-bijection." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polyhedron_facets_with_wrong_adjacency().nb_issues()
            == 1,
        "[Test] Solid should have one wrong adjacency due to "
        "non-bijection." );
    const geode::PolyhedronFacet polyhedron_facet{ 2, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polyhedron_facets_with_wrong_adjacency().issues()[0]
            == polyhedron_facet,
        "[Test] Solid facets show wrong adjacency problems." );
}

void check_non_adjacency_wrong_facet()
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
    builder->set_polyhedron_adjacent( { 1, 0 }, 0 );

    const geode::SolidMeshInspector3D adjacency_inspector{ *solid };
    OPENGEODE_EXCEPTION( adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] Solid should have wrong adjacencies due to wrong facet for "
        "adjacency." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polyhedron_facets_with_wrong_adjacency().nb_issues()
            == 2,
        "[Test] Solid should have two wrong adjacencies due to wrong facet "
        "for adjacency." );
    const geode::PolyhedronFacet polyhedron_facet1{ 0, 0 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polyhedron_facets_with_wrong_adjacency().issues()[0]
            == polyhedron_facet1,
        "[Test] Solid shows wrong first facet with adjacency problems." );
    const geode::PolyhedronFacet polyhedron_facet2{ 1, 0 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polyhedron_facets_with_wrong_adjacency().issues()[1]
            == polyhedron_facet2,
        "[Test] Solid shows wrong second facet with adjacency problems." );
}

void check_non_adjacency_inversed_tetrahedron()
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
    builder->create_tetrahedron( { 1, 4, 3, 2 } );
    builder->set_polyhedron_adjacent( { 0, 0 }, 1 );
    builder->set_polyhedron_adjacent( { 1, 1 }, 0 );

    const geode::SolidMeshInspector3D adjacency_inspector{ *solid };
    OPENGEODE_EXCEPTION( adjacency_inspector.mesh_has_wrong_adjacencies(),
        "[Test] Solid should have wrong adjacencies due to an inversed "
        "tetrahedron." );
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polyhedron_facets_with_wrong_adjacency().nb_issues()
            == 2,
        "[Test] Solid should have two wrong adjacencies due to an inversed "
        "tetrahedron." );
    const geode::PolyhedronFacet polyhedron_facet1{ 0, 0 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polyhedron_facets_with_wrong_adjacency().issues()[0]
            == polyhedron_facet1,
        "[Test] Solid shows wrong first facet with adjacency problems due to "
        "an inversed tetrahedron." );
    const geode::PolyhedronFacet polyhedron_facet2{ 1, 1 };
    OPENGEODE_EXCEPTION(
        adjacency_inspector.polyhedron_facets_with_wrong_adjacency().issues()[1]
            == polyhedron_facet2,
        "[Test] Solid shows wrong second facet with adjacency problems due to "
        "an inversed tetrahedron." );
}

int main()
{
    try
    {
        geode::InspectorInspectorLibrary::initialize();
        check_adjacency();
        check_non_adjacency_no_bijection();
        check_non_adjacency_wrong_facet();
        check_non_adjacency_inversed_tetrahedron();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
