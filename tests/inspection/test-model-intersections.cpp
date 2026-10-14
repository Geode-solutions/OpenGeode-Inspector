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
#include <geode/mesh/core/geode/geode_polygonal_surface.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/inspection/criterion/intersections/model_intersections.hpp>

namespace
{
    template < geode::index_t dimension >
    struct SurfaceDefinition
    {
        std::vector< geode::Point< dimension > > points;
        std::vector< std::vector< geode::index_t > > polygons;
        // Unique vertex of each mesh vertex, NO_ID to leave it unset
        std::vector< geode::index_t > unique_vertices;
        bool polygonal{ false };
    };

    template < geode::index_t dimension >
    struct LineDefinition
    {
        std::vector< geode::Point< dimension > > points;
        std::vector< std::array< geode::index_t, 2 > > edges;
        std::vector< geode::index_t > unique_vertices;
    };

    template < typename Model, typename Builder >
    void fill_model( const Model& model,
        Builder& builder,
        absl::Span< const SurfaceDefinition< Model::dim > > surfaces,
        absl::Span< const LineDefinition< Model::dim > > lines,
        geode::index_t nb_unique_vertices )
    {
        builder.create_unique_vertices( nb_unique_vertices );
        for( const auto& definition : surfaces )
        {
            const auto& surface_id =
                definition.polygonal
                    ? builder.add_surface( geode::OpenGeodePolygonalSurface<
                          Model::dim >::impl_name_static() )
                    : builder.add_surface();
            const auto& surface = model.surface( surface_id );
            auto mesh_builder = builder.surface_mesh_builder( surface );
            for( const auto& point : definition.points )
            {
                mesh_builder->create_point( point );
            }
            for( const auto& polygon : definition.polygons )
            {
                mesh_builder->create_polygon( polygon );
            }
            for( const auto v : geode::Indices{ definition.unique_vertices } )
            {
                if( definition.unique_vertices[v] != geode::NO_ID )
                {
                    builder.set_unique_vertex( { surface.component_id(), v },
                        definition.unique_vertices[v] );
                }
            }
        }
        for( const auto& definition : lines )
        {
            const auto& line = model.line( builder.add_line() );
            auto mesh_builder = builder.line_mesh_builder( line );
            for( const auto& point : definition.points )
            {
                mesh_builder->create_point( point );
            }
            for( const auto& edge : definition.edges )
            {
                mesh_builder->create_edge( edge[0], edge[1] );
            }
            for( const auto v : geode::Indices{ definition.unique_vertices } )
            {
                if( definition.unique_vertices[v] != geode::NO_ID )
                {
                    builder.set_unique_vertex( { line.component_id(), v },
                        definition.unique_vertices[v] );
                }
            }
        }
    }

    geode::Section build_section(
        absl::Span< const SurfaceDefinition< 2 > > surfaces,
        absl::Span< const LineDefinition< 2 > > lines,
        geode::index_t nb_unique_vertices )
    {
        geode::Section section;
        geode::SectionBuilder builder{ section };
        fill_model( section, builder, surfaces, lines, nb_unique_vertices );
        return section;
    }

    geode::BRep build_brep( absl::Span< const SurfaceDefinition< 3 > > surfaces,
        absl::Span< const LineDefinition< 3 > > lines,
        geode::index_t nb_unique_vertices )
    {
        geode::BRep brep;
        geode::BRepBuilder builder{ brep };
        fill_model( brep, builder, surfaces, lines, nb_unique_vertices );
        // Lines are only checked against surfaces when embedded in a block
        const auto& block = brep.block( builder.add_block() );
        for( const auto& line : brep.lines() )
        {
            builder.add_line_block_internal_relationship( line, block );
        }
        return brep;
    }

    template < typename Model >
    geode::index_t nb_intersections( const Model& model )
    {
        const geode::ModelMeshesIntersections< Model > inspector{ model };
        return inspector.inspect_intersections()
            .elements_intersections.nb_issues();
    }

    template < typename Model >
    geode::index_t nb_self_intersections( const Model& model )
    {
        const geode::ModelMeshesIntersections< Model > inspector{ model };
        return inspector.inspect_surfaces_self_intersections()
            .elements_intersections.nb_issues();
    }

    void check_nb_intersections( geode::index_t nb_found,
        geode::index_t nb_expected,
        std::string_view test_name )
    {
        geode::OpenGeodeInspectorInspectionException::test(
            nb_found == nb_expected, "[", test_name, "] ", nb_found,
            " intersections found instead of ", nb_expected, "." );
    }
} // namespace

void check_section_adjacent_surfaces_different_numbering()
{
    // Two triangles from two surfaces sharing edge (1,0)-(0,1), the shared
    // vertices having different mesh indices in both surfaces
    const SurfaceDefinition< 2 > surface1{ { geode::Point2D{ { 0., 0. } },
                                               geode::Point2D{ { 1., 0. } },
                                               geode::Point2D{ { 0., 1. } } },
        { { 0, 1, 2 } }, { 0, 1, 2 } };
    const SurfaceDefinition< 2 > surface2{ { geode::Point2D{ { 1., 1. } },
                                               geode::Point2D{ { 0., 1. } },
                                               geode::Point2D{ { 1., 0. } } },
        { { 0, 1, 2 } }, { 3, 2, 1 } };
    const auto section = build_section( { surface1, surface2 }, {}, 4 );
    check_nb_intersections( nb_intersections( section ), 0,
        "Section adjacent surfaces with different numbering" );
}

void check_brep_vertices_without_unique_vertex()
{
    // Two non-intersecting triangles with overlapping bounding boxes, no mesh
    // vertex being linked to a unique vertex
    const SurfaceDefinition< 3 > surface1{
        { geode::Point3D{ { 0., 0., 0. } }, geode::Point3D{ { 1., 0., 0. } },
            geode::Point3D{ { 0., 1., 0. } } },
        { { 0, 1, 2 } }, {}
    };
    const SurfaceDefinition< 3 > surface2{
        { geode::Point3D{ { 0.8, 0.8, -1. } },
            geode::Point3D{ { 0.8, 0.8, 1. } },
            geode::Point3D{ { 1., 1., 0. } } },
        { { 0, 1, 2 } }, {}
    };
    const auto brep = build_brep( { surface1, surface2 }, {}, 0 );
    check_nb_intersections(
        nb_intersections( brep ), 0, "BRep surfaces without unique vertices" );

    // Two adjacent triangles of the same surface without unique vertices
    const SurfaceDefinition< 3 > surface3{
        { geode::Point3D{ { 0., 0., 0. } }, geode::Point3D{ { 1., 0., 0. } },
            geode::Point3D{ { 0., 1., 0. } },
            geode::Point3D{ { 1., 1., 0. } } },
        { { 0, 1, 2 }, { 1, 3, 2 } }, {}
    };
    const auto brep2 = build_brep( { surface3 }, {}, 0 );
    check_nb_intersections(
        nb_intersections( brep2 ), 0, "BRep surface without unique vertices" );
    check_nb_intersections( nb_self_intersections( brep2 ), 0,
        "BRep surface without unique vertices (self)" );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        check_section_adjacent_surfaces_different_numbering();
        check_brep_vertices_without_unique_vertex();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
