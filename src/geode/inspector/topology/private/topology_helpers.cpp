/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/inspector/topology/private/topology_helpers.h>

#include <geode/basic/algorithm.h>

#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace geode
{
    namespace detail
    {
        bool brep_blocks_are_meshed( const geode::BRep& brep )
        {
            for( const auto& block : brep.blocks() )
            {
                if( block.mesh().nb_polyhedra() == 0 )
                {
                    return false;
                }
            }
            return true;
        }

        bool section_surfaces_are_meshed( const geode::Section& section )
        {
            for( const auto& surface : section.surfaces() )
            {
                if( surface.mesh().nb_polygons() == 0 )
                {
                    return false;
                }
            }
            return true;
        }

        std::vector< uuid > components_uuids(
            absl::Span< const ComponentMeshVertex > components )
        {
            std::vector< uuid > component_uuids;
            component_uuids.reserve( components.size() );
            for( const auto& cmv : components )
            {
                component_uuids.push_back( cmv.component_id.id() );
            }
            sort_unique( component_uuids );
            return component_uuids;
        }

        std::vector< geode::ComponentMeshVertex >
            brep_component_vertices_not_associated_to_unique_vertices(
                const geode::BRep& brep,
                const geode::ComponentID& component_id,
                const geode::VertexSet& component_mesh )
        {
            std::vector< geode::ComponentMeshVertex > result;
            for( const auto component_vertex :
                geode::Range{ component_mesh.nb_vertices() } )
            {
                geode::ComponentMeshVertex mesh_vertex{ component_id,
                    component_vertex };
                if( brep.unique_vertex( mesh_vertex ) == geode::NO_ID )
                {
                    result.push_back( std::move( mesh_vertex ) );
                }
            }
            return result;
        }
    } // namespace detail
} // namespace geode