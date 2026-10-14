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

#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/core/point_set.hpp>

#include <geode/model/mixin/core/corner.hpp>
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
} // namespace

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        test_non_bijective_link_message();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
