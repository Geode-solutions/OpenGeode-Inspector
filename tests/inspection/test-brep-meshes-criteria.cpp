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
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/inspection/criterion/manifold/brep_meshes_manifold.hpp>

namespace
{
    // Two triangles sharing an edge, vertices not linked to unique vertices
    const geode::Surface3D& add_square_surface(
        const geode::BRep& brep, geode::BRepBuilder& builder, double z )
    {
        const auto& surface = brep.surface( builder.add_surface() );
        auto mesh_builder = builder.surface_mesh_builder( surface );
        mesh_builder->create_point( geode::Point3D{ { 0, 0, z } } );
        mesh_builder->create_point( geode::Point3D{ { 1, 0, z } } );
        mesh_builder->create_point( geode::Point3D{ { 0, 1, z } } );
        mesh_builder->create_point( geode::Point3D{ { 1, 1, z } } );
        mesh_builder->create_polygon( { 0, 1, 2 } );
        mesh_builder->create_polygon( { 1, 3, 2 } );
        mesh_builder->compute_polygon_adjacencies();
        return surface;
    }
} // namespace

void check_model_edges_without_unique_vertices()
{
    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    add_square_surface( brep, builder, 0 );
    add_square_surface( brep, builder, 1 );
    const geode::BRepComponentMeshesManifold inspector{ brep };
    const auto result = inspector.inspect_brep_manifold();
    geode::OpenGeodeInspectorInspectionException::test(
        result.brep_non_manifold_edges.nb_issues() == 0,
        "[Test] Edges without unique vertices should not be seen as shared "
        "by several surfaces: ",
        result.brep_non_manifold_edges.string() );
}

void check_single_edge_line_without_unique_vertices()
{
    geode::BRep brep;
    geode::BRepBuilder builder{ brep };
    const auto& surface = add_square_surface( brep, builder, 0 );
    const auto& line = brep.line( builder.add_line() );
    auto mesh_builder = builder.line_mesh_builder( line );
    mesh_builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
    mesh_builder->create_point( geode::Point3D{ { 1, 1, 0 } } );
    mesh_builder->create_edge( 0, 1 );
    builder.add_line_surface_internal_relationship( line, surface );
    builder.create_unique_vertices( 1 );
    const geode::BRepComponentMeshesManifold inspector{ brep };
    const auto result = inspector.inspect_brep_manifold();
    geode::OpenGeodeInspectorInspectionException::test(
        result.brep_non_manifold_edges.nb_issues() == 0,
        "[Test] Single edge line without unique vertices should not be "
        "checked: ",
        result.brep_non_manifold_edges.string() );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        check_model_edges_without_unique_vertices();
        check_single_edge_line_without_unique_vertices();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
