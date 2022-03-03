/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/inspector/topology/private/brep_surfaces_topology_impl.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    bool line_is_boundary_of_at_least_two_surfaces( const geode::BRep& brep,
        const geode::MeshComponentVertex& line,
        const std::vector< geode::MeshComponentVertex >& surfaces )
    {
        geode::local_index_t counter{ 0 };
        for( const auto& surface : surfaces )
        {
            if( brep.Relationships::is_boundary(
                    line.component_id.id(), surface.component_id.id() ) )
            {
                counter++;
                if( counter > 1 )
                {
                    return true;
                }
            }
        }
        return false;
    }
} // namespace

namespace geode
{
    namespace detail
    {
        BRepSurfacesTopologyImpl::BRepSurfacesTopologyImpl( const BRep& brep )
            : brep_( brep )
        {
        }

        bool BRepSurfacesTopologyImpl::brep_vertex_surfaces_topology_is_valid(
            index_t unique_vertex_index ) const
        {
            const auto lines = brep_.mesh_component_vertices(
                unique_vertex_index, Line3D::component_type_static() );
            if( lines.empty() )
            {
                return true;
            }
            if( vertex_is_part_of_not_boundary_nor_internal_surface(
                    unique_vertex_index )
                || vertex_is_part_of_surface_with_invalid_internal_topology(
                    unique_vertex_index )
                || vertex_is_part_of_invalid_unique_surface(
                    unique_vertex_index )
                || vertex_is_part_of_invalid_multiple_surfaces(
                    unique_vertex_index ) )
            {
                return false;
            }
            return true;
        }

        bool BRepSurfacesTopologyImpl::
            vertex_is_part_of_not_boundary_nor_internal_surface(
                const index_t unique_vertex_index ) const
        {
            for( const auto surface : brep_.mesh_component_vertices(
                     unique_vertex_index, Surface3D::component_type_static() ) )
            {
                if( brep_.nb_embeddings( surface.component_id.id() ) < 1
                    && brep_.nb_incidences( surface.component_id.id() ) < 1 )
                {
                    return true;
                }
            }
            return false;
        }

        bool BRepSurfacesTopologyImpl::
            vertex_is_part_of_surface_with_invalid_internal_topology(
                const index_t unique_vertex_index ) const
        {
            for( const auto surface : brep_.mesh_component_vertices(
                     unique_vertex_index, Surface3D::component_type_static() ) )
            {
                if( brep_.nb_embeddings( surface.component_id.id() ) < 1 )
                {
                    return false;
                }
                else if( brep_.nb_embeddings( surface.component_id.id() ) > 1
                         || brep_.nb_incidences( surface.component_id.id() )
                                > 0 )
                {
                    return true;
                }
            }
            return false;
        }

        bool BRepSurfacesTopologyImpl::vertex_is_part_of_invalid_unique_surface(
            index_t unique_vertex_index ) const
        {
            const auto surfaces = brep_.mesh_component_vertices(
                unique_vertex_index, Surface3D::component_type_static() );
            if( surfaces.size() != 1 )
            {
                return false;
            }
            const auto& surface_id = surfaces[0].component_id.id();
            const auto blocks = brep_.mesh_component_vertices(
                unique_vertex_index, Block3D::component_type_static() );
            if( blocks.size() > 2 )
            {
                return true;
            }
            if( brep_.nb_embeddings( surface_id ) > 0 )
            {
                if( blocks.size() != 1
                    || !brep_.Relationships::is_internal(
                        surface_id, blocks[0].component_id.id() ) )
                {
                    return true;
                }
            }
            else
            {
                for( const auto& block : blocks )
                {
                    if( !brep_.Relationships::is_boundary(
                            surface_id, block.component_id.id() ) )
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        bool BRepSurfacesTopologyImpl::
            vertex_is_part_of_invalid_multiple_surfaces(
                index_t unique_vertex_index ) const
        {
            const auto surfaces = brep_.mesh_component_vertices(
                unique_vertex_index, Surface3D::component_type_static() );
            if( surfaces.size() < 2 )
            {
                return false;
            }
            const auto lines = brep_.mesh_component_vertices(
                unique_vertex_index, Line3D::component_type_static() );
            if( lines.empty() )
            {
                return true;
            }
            if( lines.size() == 1 )
            {
                if( !brep_
                         .mesh_component_vertices( unique_vertex_index,
                             Corner3D::component_type_static() )
                         .empty() )
                {
                    return true;
                }
                for( const auto& surface : surfaces )
                {
                    if( !brep_.Relationships::is_boundary(
                            lines[0].component_id.id(),
                            surface.component_id.id() ) )
                    {
                        return true;
                    }
                }
            }
            else
            {
                for( const auto& line : lines )
                {
                    if( brep_.nb_embeddings( line.component_id.id() ) < 1
                        && !line_is_boundary_of_at_least_two_surfaces(
                            brep_, line, surfaces ) )
                    {
                        return true;
                    }
                }
            }
            return false;
        }
    } // namespace detail
} // namespace geode
