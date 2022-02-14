/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/tests_config.h>

#include <geode/basic/assert.h>
#include <geode/basic/logger.h>

#include <geode/model/helpers/convert_model_meshes.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/io/brep_input.h>

#include <geode/inspector/topology/brep_vertices_topology.h>

void check_a1_vertices_topology()
{
    auto model_brep = geode::load_brep(
        absl::StrCat( geode::data_path, "model_A1.og_brep" ) );
    geode::BRepVerticesTopology brep_corner_inspector{ model_brep };
    geode::Logger::info( "model_A1 vertices_topology is ",
        brep_corner_inspector.brep_vertices_topology_is_valid() ? "valid."
                                                                : "invalid." );
    auto multiple_corners_unique_vertices =
        brep_corner_inspector.multiple_corners_unique_vertices();
    for( const auto vertex_index : multiple_corners_unique_vertices )
    {
        OPENGEODE_EXCEPTION( "[Test] Model unique vertex with index ",
            vertex_index, "is associated to multiple corners." );
    }
    auto multiple_internals_corner_vertices =
        brep_corner_inspector.multiple_internals_corner_vertices();
    for( const auto vertex_index : multiple_internals_corner_vertices )
    {
        OPENGEODE_EXCEPTION( "[Test] Model unique vertex with index ",
            vertex_index, "is a corner associated with multiple embeddings." );
    }
    auto line_corners_without_boundary_status =
        brep_corner_inspector.line_corners_without_boundary_status();
    for( const auto vertex_index : line_corners_without_boundary_status )
    {
        OPENGEODE_EXCEPTION( "[Test] Model unique vertex with index ",
            vertex_index,
            "is a corner but has a line for which it is not a boundary." );
    }
}

void check_a1_valid_vertices_topology()
{
    auto model_brep = geode::load_brep(
        absl::StrCat( geode::data_path, "model_A1_valid.og_brep" ) );
    geode::BRepVerticesTopology brep_corner_inspector{ model_brep };
    geode::Logger::info( "model_A1_valid vertices_topology is ",
        brep_corner_inspector.brep_vertices_topology_is_valid() ? "valid."
                                                                : "invalid." );
    auto multiple_corners_unique_vertices =
        brep_corner_inspector.multiple_corners_unique_vertices();
    for( const auto vertex_index : multiple_corners_unique_vertices )
    {
        OPENGEODE_EXCEPTION( "[Test] Model unique vertex with index ",
            vertex_index, "is associated to multiple corners." );
    }
    auto multiple_internals_corner_vertices =
        brep_corner_inspector.multiple_internals_corner_vertices();
    for( const auto vertex_index : multiple_internals_corner_vertices )
    {
        OPENGEODE_EXCEPTION( "[Test] Model unique vertex with index ",
            vertex_index, "is a corner associated with multiple embeddings." );
    }
    auto line_corners_without_boundary_status =
        brep_corner_inspector.line_corners_without_boundary_status();
    for( const auto vertex_index : line_corners_without_boundary_status )
    {
        OPENGEODE_EXCEPTION( "[Test] Model unique vertex with index ",
            vertex_index,
            "is a corner but has a line for which it is not a boundary." );
    }
}

int main()
{
    try
    {
        check_a1_vertices_topology();
        check_a1_valid_vertices_topology();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
