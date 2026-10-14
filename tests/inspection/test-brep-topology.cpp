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

#include <absl/strings/match.h>

#include <geode/basic/assert.hpp>
#include <geode/basic/attribute_manager.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/variable_attribute.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/edged_curve_builder.hpp>
#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/builder/solid_mesh_builder.hpp>
#include <geode/mesh/core/point_set.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/representation/builder/brep_builder.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/inspection/topology/brep_topology.hpp>

namespace
{
    bool has_message( const geode::InspectionIssues< geode::index_t >& issues,
        std::string_view text )
    {
        return absl::StrContains( issues.string(), text );
    }

    void test_non_bijective_link_message()
    {
        geode::BRep brep;
        geode::BRepBuilder builder{ brep };
        const auto& corner = brep.corner( builder.add_corner() );
        builder.corner_mesh_builder( corner )->create_point(
            geode::Point3D{ { 0, 0, 0 } } );
        builder.create_unique_vertices( 2 );
        builder.set_unique_vertex( { corner.component_id(), 0 }, 0 );
        // Corrupt the link: the corner vertex now points to unique vertex 1
        corner.mesh()
            .vertex_attribute_manager()
            .find_attribute< geode::VariableAttribute, geode::index_t >(
                brep.unique_vertex_attribute_id() )
            ->set_value( 0, 1 );
        const geode::BRepTopologyInspector inspector{ brep };
        const auto result = inspector.inspect_brep_topology();
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

    void test_line_embedded_in_block_message()
    {
        geode::BRep brep;
        geode::BRepBuilder builder{ brep };
        const auto& block = brep.block( builder.add_block() );
        auto block_builder = builder.block_mesh_builder( block );
        block_builder->create_point( geode::Point3D{ { 0, 0, 0 } } );
        block_builder->create_point( geode::Point3D{ { 4, 0, 0 } } );
        block_builder->create_point( geode::Point3D{ { 0, 4, 0 } } );
        block_builder->create_point( geode::Point3D{ { 0, 0, 4 } } );
        block_builder->create_polyhedron( { 0, 1, 2, 3 },
            { { 1, 3, 2 }, { 0, 2, 3 }, { 3, 1, 0 }, { 0, 1, 2 } } );
        const auto& line = brep.line( builder.add_line() );
        auto line_builder = builder.line_mesh_builder( line );
        line_builder->create_point( geode::Point3D{ { 1, 1, 1 } } );
        line_builder->create_point( geode::Point3D{ { 1, 1, 2 } } );
        line_builder->create_edge( 0, 1 );
        builder.add_line_block_internal_relationship( line, block );
        builder.create_unique_vertices( 6 );
        for( const auto v : geode::LRange{ 4 } )
        {
            builder.set_unique_vertex( { block.component_id(), v }, v );
        }
        // The line vertices are not linked to block mesh vertices
        builder.set_unique_vertex( { line.component_id(), 0 }, 4 );
        builder.set_unique_vertex( { line.component_id(), 1 }, 5 );
        const geode::BRepTopologyInspector inspector{ brep };
        const auto result = inspector.inspect_brep_topology();
        const auto& issues =
            result.lines
                .unique_vertices_linked_to_a_line_with_invalid_embeddings;
        geode::OpenGeodeInspectorInspectionException::test(
            issues.nb_issues() == 2,
            "[line in block] Wrong number of invalid embedding issues: ",
            issues.string() );
        geode::OpenGeodeInspectorInspectionException::test(
            has_message( issues, "which is embedded in Block" )
                && has_message(
                    issues, "not linked to the Block mesh vertices" ),
            "[line in block] Wrong message: ", issues.string() );
    }

    void test_empty_model_boundary()
    {
        geode::BRep brep;
        geode::BRepBuilder builder{ brep };
        builder.add_model_boundary();
        const auto& corner = brep.corner( builder.add_corner() );
        builder.corner_mesh_builder( corner )->create_point(
            geode::Point3D{ { 0, 0, 0 } } );
        builder.create_unique_vertices( 1 );
        builder.set_unique_vertex( { corner.component_id(), 0 }, 0 );
        const geode::BRepTopologyInspector inspector{ brep };
        const auto result = inspector.inspect_brep_topology();
        geode::OpenGeodeInspectorInspectionException::test(
            result.blocks.model_boundaries_dont_form_a_closed_surface
                    .nb_issues()
                == 0,
            "[empty model boundary] Empty model boundary should not be "
            "reported" );
    }
} // namespace

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        test_non_bijective_link_message();
        test_line_embedded_in_block_message();
        test_empty_model_boundary();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
