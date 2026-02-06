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

#pragma once

#include <absl/types/span.h>

#include <geode/basic/algorithm.hpp>
#include <geode/model/mixin/core/component_type.hpp>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    class BRep;
    class Section;
    struct uuid;
    struct ComponentMeshVertex;
    class ComponentID;
    class VertexSet;
} // namespace geode

namespace geode
{
    namespace internal
    {
        [[nodiscard]] bool brep_blocks_are_meshed( const BRep& brep );

        [[nodiscard]] bool section_surfaces_are_meshed(
            const Section& section );

        template < typename Model >
        [[nodiscard]] std::vector< uuid > components_uuids( const Model& model,
            index_t unique_vertex_index,
            const geode::ComponentType& type )
        {
            std::vector< uuid > component_uuids;
            for( const auto& cmv :
                model.component_mesh_vertices( unique_vertex_index ) )
            {
                if( cmv.component_id.type() == type )
                {
                    component_uuids.push_back( cmv.component_id.id() );
                }
            }
            sort_unique( component_uuids );
            return component_uuids;
        }

        template < typename Model, typename Mesh >
        bool model_component_vertices_are_associated_to_unique_vertices(
            const Model& model,
            const ComponentID& component_id,
            const Mesh& component_mesh )
        {
            for( const auto component_vertex :
                Range{ component_mesh.nb_vertices() } )
            {
                if( model.unique_vertex( { component_id, component_vertex } )
                    == NO_ID )
                {
                    return false;
                }
            }
            return true;
        }

        template < typename Model, typename Mesh >
        [[nodiscard]] InspectionIssues< index_t >
            model_component_vertices_not_associated_to_unique_vertices(
                const Model& model,
                const ComponentID& component_id,
                const Mesh& component_mesh )
        {
            InspectionIssues< index_t > result;
            for( const auto vertex_id : Range{ component_mesh.nb_vertices() } )
            {
                ComponentMeshVertex component_mesh_vertex{ component_id,
                    vertex_id };
                if( model.unique_vertex( component_mesh_vertex ) == NO_ID )
                {
                    result.add_issue(
                        vertex_id, absl::StrCat( "vertex '", vertex_id,
                                       " is not linked to a unique vertex." ) );
                }
            }
            return result;
        }
    } // namespace internal
} // namespace geode