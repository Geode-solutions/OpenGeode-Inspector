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

#include <geode/inspector/topology/private/brep_lines_topology_impl.h>

#include <absl/algorithm/container.h>

#include <geode/basic/logger.h>

#include <geode/mesh/core/solid_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

#include <geode/inspector/topology/private/topology_helpers.h>

namespace geode
{
    namespace detail
    {
        BRepLinesTopologyImpl::BRepLinesTopologyImpl(
            const BRep& brep, bool verbose )
            : brep_( brep ), verbose_( verbose )
        {
        }
        BRepLinesTopologyImpl::BRepLinesTopologyImpl( const BRep& brep )
            : BRepLinesTopologyImpl( brep, false )
        {
        }

        bool BRepLinesTopologyImpl::brep_vertex_lines_topology_is_valid(
            index_t unique_vertex_index ) const
        {
            const auto lines = brep_.component_mesh_vertices(
                unique_vertex_index, Line3D::component_type_static() );
            if( lines.empty() )
            {
                return true;
            }
            if( vertex_is_part_of_not_boundary_nor_internal_line(
                    unique_vertex_index )
                || vertex_is_part_of_line_with_invalid_internal_topology(
                    unique_vertex_index )
                || vertex_is_part_of_invalid_unique_line( unique_vertex_index )
                || vertex_has_lines_but_is_not_corner( unique_vertex_index ) )
            {
                return false;
            }
            return true;
        }

        bool BRepLinesTopologyImpl::
            vertex_is_part_of_not_boundary_nor_internal_line(
                const index_t unique_vertex_index ) const
        {
            for( const auto& line : brep_.component_mesh_vertices(
                     unique_vertex_index, Line3D::component_type_static() ) )
            {
                if( brep_.nb_embeddings( line.component_id.id() ) < 1
                    && brep_.nb_incidences( line.component_id.id() ) < 1 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index, " is part of line with uuid '",
                            line.component_id.id().string(),
                            "', which is neither embedded nor incident." );
                    }
                    return true;
                }
            }
            return false;
        }

        bool BRepLinesTopologyImpl::
            vertex_is_part_of_line_with_invalid_internal_topology(
                const index_t unique_vertex_index ) const
        {
            for( const auto line_id :
                components_uuids( brep_.component_mesh_vertices(
                    unique_vertex_index, Line3D::component_type_static() ) ) )
            {
                for( const auto& embedding : brep_.embeddings( line_id ) )
                {
                    if( brep_.Relationships::is_boundary(
                            line_id, embedding.id() ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of line with uuid '",
                                line_id.string(),
                                "', which is both boundary and embedded in "
                                "surface with uuid '",
                                embedding.id().string(), "'." );
                        }
                        return true;
                    }
                    if( embedding.type() == Block3D::component_type_static()
                        && !brep_blocks_are_meshed( brep_ ) )
                    {
                        continue;
                    }
                    if( !absl::c_any_of(
                            brep_.component_mesh_vertices(
                                unique_vertex_index, embedding.type() ),
                            [&embedding]( const ComponentMeshVertex& cmv ) {
                                return cmv.component_id.id() == embedding.id();
                            } ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of line with uuid '",
                                line_id.string(),
                                "', which is embedded in surface with uuid '",
                                embedding.id().string(),
                                "', but the unique vertex is not linked to the "
                                "surface mesh vertices." );
                        }
                        return true;
                    }
                }
            }
            return false;
        }

        bool BRepLinesTopologyImpl::vertex_is_part_of_invalid_unique_line(
            index_t unique_vertex_index ) const
        {
            const auto line_uuids =
                components_uuids( brep_.component_mesh_vertices(
                    unique_vertex_index, Line3D::component_type_static() ) );
            if( line_uuids.size() != 1 )
            {
                return false;
            }
            const auto& line_id = line_uuids[0];
            const auto surface_uuids =
                components_uuids( brep_.component_mesh_vertices(
                    unique_vertex_index, Surface3D::component_type_static() ) );
            const auto block_uuids =
                components_uuids( brep_.component_mesh_vertices(
                    unique_vertex_index, Block3D::component_type_static() ) );
            if( surface_uuids.size() == 1 )
            {
                if( !brep_.Relationships::is_internal(
                        line_id, surface_uuids[0] )
                    && !( brep_.Relationships::nb_embeddings( surface_uuids[0] )
                              > 0
                          && brep_.Relationships::is_boundary(
                              line_id, surface_uuids[0] ) ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of only one line, with uuid '",
                            line_id.string(),
                            "', and only one surface, with uuid '",
                            surface_uuids[0].string(),
                            "', but the line is neither embedded in the "
                            "surface, nor boundary of the surface while the "
                            "surface is embedded in a block." );
                    }
                    return true;
                }
            }
            else if( surface_uuids.empty() )
            {
                if( brep_blocks_are_meshed( brep_ )
                    && !( block_uuids.size() == 1
                          && brep_.Relationships::is_internal(
                              line_id, block_uuids[0] ) ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of only one line, with uuid '",
                            line_id.string(),
                            "', no surfaces, but is either part of no or "
                            "several blocks, or the line is not internal to "
                            "one." );
                    }
                    return true;
                }
            }
            else
            {
                for( const auto& surface_id : surface_uuids )
                {
                    if( !brep_.Relationships::is_boundary( line_id, surface_id )
                        && !brep_.Relationships::is_internal(
                            line_id, surface_id ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of only one line, with uuid '",
                                line_id.string(),
                                "', and multiple surfaces, but the line is "
                                "neither internal nor boundary of surface with "
                                "uuid '",
                                surface_id.string(),
                                "', in which the vertex is." );
                        }
                        return true;
                    }
                }
            }
            return false;
        }

        bool BRepLinesTopologyImpl::vertex_has_lines_but_is_not_corner(
            index_t unique_vertex_index ) const
        {
            if( brep_.component_mesh_vertices(
                         unique_vertex_index, Line3D::component_type_static() )
                        .size()
                    > 1
                && brep_
                       .component_mesh_vertices( unique_vertex_index,
                           Corner3D::component_type_static() )
                       .empty() )
            {
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of multiple lines but is not a corner." );
                }
                return true;
            }
            return false;
        }
    } // namespace detail
} // namespace geode
