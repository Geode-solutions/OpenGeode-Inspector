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

#include <geode/inspector/topology/private/brep_lines_topology_impl.h>

#include <geode/basic/logger.h>

#include <geode/mesh/core/solid_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    bool brep_blocks_are_meshed( const geode::BRep& brep )
    {
        for( const auto& block : brep.blocks() )
        {
            if( block.mesh().nb_vertices() == 0 )
            {
                return false;
            }
        }
        return true;
    }
} // namespace

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
            const auto lines = brep_.mesh_component_vertices(
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
            for( const auto line : brep_.mesh_component_vertices(
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
            for( const auto line : brep_.mesh_component_vertices(
                     unique_vertex_index, Line3D::component_type_static() ) )
            {
                const auto embeddings =
                    brep_.embeddings( line.component_id.id() );

                for( const auto embedding : embeddings )
                {
                    if( brep_.Relationships::is_boundary(
                            line.component_id.id(), embedding.id() ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of line with uuid '",
                                line.component_id.id().string(),
                                "', which is both boundary and embedded in "
                                "surface with uuid '",
                                embedding.id().string(), "'." );
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
            const auto lines = brep_.mesh_component_vertices(
                unique_vertex_index, Line3D::component_type_static() );
            if( lines.size() != 1 )
            {
                return false;
            }
            const auto& line_id = lines[0].component_id.id();
            const auto surfaces = brep_.mesh_component_vertices(
                unique_vertex_index, Surface3D::component_type_static() );
            const auto blocks = brep_.mesh_component_vertices(
                unique_vertex_index, Block3D::component_type_static() );
            if( surfaces.size() == 1 )
            {
                if( !brep_.Relationships::is_internal(
                        line_id, surfaces[0].component_id.id() )
                    && !( brep_.Relationships::nb_embeddings(
                              surfaces[0].component_id.id() )
                              > 0
                          && brep_.Relationships::is_boundary(
                              line_id, surfaces[0].component_id.id() ) ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of only one line, with uuid '",
                            line_id.string(),
                            "', and only one surface, with uuid '",
                            surfaces[0].component_id.id().string(),
                            "', but the line is neither embedded in the "
                            "surface, nor boundary of the surface while the "
                            "surface is embedded in a block." );
                    }
                    return true;
                }
            }
            else if( surfaces.empty() )
            {
                if( brep_blocks_are_meshed( brep_ )
                    && !( blocks.size() == 1
                          && brep_.Relationships::is_internal(
                              line_id, blocks[0].component_id.id() ) ) )
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
            else if( surfaces.size() > 1 )
            {
                for( const auto& surface : surfaces )
                {
                    if( !brep_.Relationships::is_boundary(
                            line_id, surface.component_id.id() )
                        && !brep_.Relationships::is_internal(
                            line_id, surface.component_id.id() ) )
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
                                surface.component_id.id().string(),
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
            if( brep_.mesh_component_vertices(
                         unique_vertex_index, Line3D::component_type_static() )
                        .size()
                    > 1
                && brep_
                       .mesh_component_vertices( unique_vertex_index,
                           Corner3D::component_type_static() )
                       .empty() )
            {
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part multiple lines but is not a corner." );
                }
                return true;
            }
            return false;
        }
    } // namespace detail
} // namespace geode
