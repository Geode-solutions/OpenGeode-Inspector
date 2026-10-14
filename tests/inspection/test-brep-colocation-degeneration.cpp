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

#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/inspection/criterion/colocation/component_meshes_colocation.hpp>

namespace
{
    // Surface with vertices 0 and 1 colocated, unique vertices given per
    // vertex (NO_ID to leave the vertex unlinked)
    geode::BRep build_colocated_surface(
        const std::array< geode::index_t, 4 >& unique_vertices )
    {
        geode::BRep brep;
        geode::BRepBuilder builder{ brep };
        builder.create_unique_vertices( 4 );
        const auto& surface = brep.surface( builder.add_surface() );
        auto mesh_builder = builder.surface_mesh_builder( surface );
        mesh_builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
        mesh_builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
        mesh_builder->create_point( geode::Point3D{ { 1, 0, 0 } } );
        mesh_builder->create_point( geode::Point3D{ { 0, 1, 0 } } );
        mesh_builder->create_polygon( { 0, 2, 3 } );
        mesh_builder->create_polygon( { 1, 3, 2 } );
        for( const auto v : geode::LRange{ 4 } )
        {
            if( unique_vertices[v] != geode::NO_ID )
            {
                builder.set_unique_vertex(
                    { surface.component_id(), v }, unique_vertices[v] );
            }
        }
        return brep;
    }

    geode::index_t nb_colocated_groups( const geode::BRep& brep )
    {
        const geode::BRepComponentMeshesColocation inspector{ brep };
        const auto result = inspector.inspect_meshes_point_colocations();
        geode::index_t nb_groups{ 0 };
        for( const auto& issues : result.colocated_points_groups.issues_map() )
        {
            nb_groups += issues.second.nb_issues();
        }
        return nb_groups;
    }
} // namespace

void check_colocation_without_unique_vertices()
{
    const auto unlinked = build_colocated_surface(
        { geode::NO_ID, geode::NO_ID, geode::NO_ID, geode::NO_ID } );
    geode::OpenGeodeInspectorInspectionException::test(
        nb_colocated_groups( unlinked ) == 1,
        "[Test] Colocated vertices without unique vertex should be reported" );

    const auto same_unique_vertex = build_colocated_surface( { 0, 0, 1, 2 } );
    geode::OpenGeodeInspectorInspectionException::test(
        nb_colocated_groups( same_unique_vertex ) == 0,
        "[Test] Colocated vertices with the same unique vertex should not be "
        "reported" );

    const auto different_unique_vertices =
        build_colocated_surface( { 0, 3, 1, 2 } );
    geode::OpenGeodeInspectorInspectionException::test(
        nb_colocated_groups( different_unique_vertices ) == 1,
        "[Test] Colocated vertices with different unique vertices should be "
        "reported" );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        check_colocation_without_unique_vertices();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
