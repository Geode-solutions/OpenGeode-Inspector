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

void check_section_nested_triangles()
{
    // A small triangle strictly inside a big one, no common vertex
    const SurfaceDefinition< 2 > big{ { geode::Point2D{ { 0., 0. } },
                                          geode::Point2D{ { 10., 0. } },
                                          geode::Point2D{ { 0., 10. } } },
        { { 0, 1, 2 } }, { 0, 1, 2 } };
    const SurfaceDefinition< 2 > inside{ { geode::Point2D{ { 1., 1. } },
                                             geode::Point2D{ { 2., 1. } },
                                             geode::Point2D{ { 1., 2. } } },
        { { 0, 1, 2 } }, { 3, 4, 5 } };
    const auto section = build_section( { big, inside }, {}, 6 );
    check_nb_intersections(
        nb_intersections( section ), 1, "Section triangle inside another" );

    // A small triangle nested in the corner of the big one at vertex 0
    const SurfaceDefinition< 2 > corner{ { geode::Point2D{ { 0., 0. } },
                                             geode::Point2D{ { 3., 1. } },
                                             geode::Point2D{ { 1., 3. } } },
        { { 0, 1, 2 } }, { 0, 3, 4 } };
    const auto section2 = build_section( { big, corner }, {}, 5 );
    check_nb_intersections( nb_intersections( section2 ), 1,
        "Section triangle nested in a corner" );

    // Triangle outside the big one, only sharing vertex 0
    const SurfaceDefinition< 2 > outside{ { geode::Point2D{ { 0., 0. } },
                                              geode::Point2D{ { -3., -1. } },
                                              geode::Point2D{ { -1., -3. } } },
        { { 0, 1, 2 } }, { 0, 3, 4 } };
    const auto section3 = build_section( { big, outside }, {}, 5 );
    check_nb_intersections(
        nb_intersections( section3 ), 0, "Section triangles sharing a vertex" );
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

void check_brep_line_in_surface_plane()
{
    // A line lying in the plane of a triangle and crossing it
    const SurfaceDefinition< 3 > triangle{
        { geode::Point3D{ { 0., 0., 0. } }, geode::Point3D{ { 2., 0., 0. } },
            geode::Point3D{ { 0., 2., 0. } } },
        { { 0, 1, 2 } }, {}
    };
    const LineDefinition< 3 > crossing{ { geode::Point3D{ { -1., 0.5, 0. } },
                                            geode::Point3D{ { 3., 0.5, 0. } } },
        { { 0, 1 } }, {} };
    const auto brep = build_brep( { triangle }, { crossing }, 0 );
    check_nb_intersections( nb_intersections( brep ), 1,
        "BRep line crossing a surface in its plane" );

    // A line in the plane only touching a triangle vertex
    const LineDefinition< 3 > touching{ { geode::Point3D{ { -1., 0., 0. } },
                                            geode::Point3D{ { 0., 0., 0. } } },
        { { 0, 1 } }, {} };
    const auto brep2 = build_brep( { triangle }, { touching }, 0 );
    check_nb_intersections( nb_intersections( brep2 ), 0,
        "BRep line touching a surface vertex in its plane" );
}

void check_collinear_lines()
{
    // Two lines overlapping along the same direction
    const LineDefinition< 2 > line1{ { geode::Point2D{ { 0., 0. } },
                                         geode::Point2D{ { 2., 0. } } },
        { { 0, 1 } }, {} };
    const LineDefinition< 2 > line2{ { geode::Point2D{ { 1., 0. } },
                                         geode::Point2D{ { 3., 0. } } },
        { { 0, 1 } }, {} };
    const auto section = build_section( {}, { line1, line2 }, 0 );
    check_nb_intersections(
        nb_intersections( section ), 1, "Section overlapping lines" );

    // Two collinear lines only touching at an end point
    const LineDefinition< 2 > line3{ { geode::Point2D{ { 2., 0. } },
                                         geode::Point2D{ { 3., 0. } } },
        { { 0, 1 } }, {} };
    const auto section2 = build_section( {}, { line1, line3 }, 0 );
    check_nb_intersections(
        nb_intersections( section2 ), 0, "Section touching collinear lines" );

    // A line folding back on itself in 3D
    const LineDefinition< 3 > folded{ { geode::Point3D{ { 0., 0., 0. } },
                                          geode::Point3D{ { 2., 2., 2. } },
                                          geode::Point3D{ { 1., 1., 1. } } },
        { { 0, 1 }, { 1, 2 } }, {} };
    const auto brep = build_brep( {}, { folded }, 0 );
    check_nb_intersections(
        nb_intersections( brep ), 1, "BRep line folding back on itself" );
}

void check_brep_common_vertex_with_different_coordinates()
{
    // Two triangles sharing unique vertex 0, whose coordinates slightly
    // differ between both surfaces: they only touch at this vertex
    const SurfaceDefinition< 3 > horizontal{
        { geode::Point3D{ { 0., 0., 0. } }, geode::Point3D{ { 1., 0., 0. } },
            geode::Point3D{ { 0., 1., 0. } } },
        { { 0, 1, 2 } }, { 0, 1, 2 }
    };
    const SurfaceDefinition< 3 > vertical{
        { geode::Point3D{ { 1e-9, 1e-9, 0. } },
            geode::Point3D{ { -1., -1., 1. } },
            geode::Point3D{ { -1., -1., -1. } } },
        { { 0, 1, 2 } }, { 0, 3, 4 }
    };
    const auto brep = build_brep( { horizontal, vertical }, {}, 5 );
    check_nb_intersections( nb_intersections( brep ), 0,
        "BRep common vertex with different coordinates" );
}

void check_brep_with_empty_meshes()
{
    // Two crossing triangles plus a surface and a line without vertices
    const SurfaceDefinition< 3 > horizontal{
        { geode::Point3D{ { 0., 0., 0. } }, geode::Point3D{ { 2., 0., 0. } },
            geode::Point3D{ { 0., 2., 0. } } },
        { { 0, 1, 2 } }, {}
    };
    const SurfaceDefinition< 3 > vertical{
        { geode::Point3D{ { 0.5, 0.5, -1. } },
            geode::Point3D{ { 0.5, 0.5, 1. } },
            geode::Point3D{ { 1., 0., 0.5 } } },
        { { 0, 1, 2 } }, {}
    };
    const SurfaceDefinition< 3 > empty_surface{};
    const LineDefinition< 3 > empty_line{};
    const auto brep = build_brep(
        { horizontal, empty_surface, vertical }, { empty_line }, 0 );
    check_nb_intersections(
        nb_intersections( brep ), 1, "BRep with empty meshes" );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        check_section_adjacent_surfaces_different_numbering();
        check_section_nested_triangles();
        check_brep_vertices_without_unique_vertex();
        check_brep_with_empty_meshes();
        check_brep_common_vertex_with_different_coordinates();
        check_collinear_lines();
        check_brep_line_in_surface_plane();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
