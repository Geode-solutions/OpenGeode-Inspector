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

#include <tuple>

#include <absl/strings/match.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/surface_mesh_builder.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/builder/section_builder.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/inspection/topology/section_topology.hpp>

namespace
{
    bool has_message( const geode::InspectionIssues< geode::index_t >& issues,
        std::string_view text )
    {
        return absl::StrContains( issues.string(), text );
    }

    void test_non_bijective_link_message()
    {
        geode::Section section;
        geode::SectionBuilder builder{ section };
        const auto& corner = section.corner( builder.add_corner() );
        builder.corner_mesh_builder( corner )->create_point(
            geode::Point2D{ { 0, 0 } } );
        builder.create_unique_vertices( 2 );
        builder.set_unique_vertex( { corner.component_id(), 0 }, 0 );
        // Corrupt the link: the corner vertex now points to unique vertex 1
        corner.mesh()
            .vertex_attribute_manager()
            .find_attribute< geode::VariableAttribute, geode::index_t >(
                section.unique_vertex_attribute_id() )
            ->set_value( 0, 1 );
        const geode::SectionTopologyInspector inspector{ section };
        const auto result = inspector.inspect_section_topology();
        const auto& non_bijective =
            result.unique_vertices_nonbijectively_linked_to_cmv;
        geode::OpenGeodeInspectorInspectionException::test(
            non_bijective.nb_issues() == 1,
            "[non bijective] Wrong number of non bijective issues" );
        geode::OpenGeodeInspectorInspectionException::test(
            !has_message( non_bijective, "inexistant" )
                && has_message( non_bijective, "linked to unique vertex 1" ),
            "[non bijective] Wrong message: ", non_bijective.string() );
    }

    void test_link_beyond_component_mesh()
    {
        geode::Section section;
        geode::SectionBuilder builder{ section };
        const std::array< geode::Point2D, 4 > points{
            geode::Point2D{ { 0, 0 } }, geode::Point2D{ { 1, 0 } },
            geode::Point2D{ { 0, 1 } }, geode::Point2D{ { 1, 1 } }
        };
        const auto& surface = section.surface( builder.add_surface() );
        auto surface_builder = builder.surface_mesh_builder( surface );
        builder.create_unique_vertices( 4 );
        for( const auto v : geode::LRange{ 4 } )
        {
            const auto& corner = section.corner( builder.add_corner() );
            builder.corner_mesh_builder( corner )->create_point( points[v] );
            builder.set_unique_vertex( { corner.component_id(), 0 }, v );
            surface_builder->create_point( points[v] );
            builder.set_unique_vertex( { surface.component_id(), v }, v );
        }
        surface_builder->create_polygon( { 0, 1, 2 } );
        surface_builder->create_polygon( { 1, 3, 2 } );
        surface_builder->compute_polygon_adjacencies();
        const auto& line = section.line( builder.add_line() );
        auto line_builder = builder.line_mesh_builder( line );
        line_builder->create_point( points[1] );
        line_builder->create_point( points[3] );
        line_builder->create_edge( 0, 1 );
        builder.set_unique_vertex( { line.component_id(), 0 }, 1 );
        builder.set_unique_vertex( { line.component_id(), 1 }, 3 );
        builder.add_line_surface_boundary_relationship( line, surface );
        std::ignore = surface_builder->delete_polygons( { false, true } );
        std::ignore = surface_builder->delete_isolated_vertices();
        const geode::SectionTopologyInspector inspector{ section };
        const auto result = inspector.inspect_section_topology();
        geode::OpenGeodeInspectorInspectionException::test(
            result.unique_vertices_linked_to_inexistant_cmv.nb_issues() == 1,
            "[link beyond mesh] Link to deleted Surface vertex should be "
            "reported: ",
            result.string() );
    }
} // namespace

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        test_non_bijective_link_message();
        test_link_beyond_component_mesh();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
