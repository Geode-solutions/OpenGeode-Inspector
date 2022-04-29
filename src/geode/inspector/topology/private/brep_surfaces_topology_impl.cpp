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
    bool line_is_boundary_of_at_least_two_surfaces_or_one_embedding_surface(
        const geode::BRep& brep,
        const geode::MeshComponentVertex& line,
        const std::vector< geode::MeshComponentVertex >& surfaces )
    {
        geode::local_index_t counter{ 0 };
        for( const auto& surface : surfaces )
        {
            if( brep.Relationships::is_boundary(
                    line.component_id.id(), surface.component_id.id() ) )
            {
                if( brep.nb_embeddings( surface.component_id.id() ) > 0 )
                {
                    return true;
                }
                counter++;
                if( counter > 1 )
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool brep_blocks_are_meshed( const geode::BRep& brep )
    {
        for( const auto& block : brep.blocks() )
        {
            if( brep.block( block.component_id().id() ).mesh().nb_vertices()
                == 0 )
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
        BRepSurfacesTopologyImpl::BRepSurfacesTopologyImpl( const BRep& brep )
            : brep_( brep )
        {
        }

        bool BRepSurfacesTopologyImpl::brep_vertex_surfaces_topology_is_valid(
            index_t unique_vertex_index ) const
        {
            const auto surfaces = brep_.mesh_component_vertices(
                unique_vertex_index, Surface3D::component_type_static() );
            if( surfaces.empty() )
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
                const index_t unique_vertex_index, bool verbose ) const
        {
            for( const auto surface : brep_.mesh_component_vertices(
                     unique_vertex_index, Surface3D::component_type_static() ) )
            {
                if( brep_.nb_embeddings( surface.component_id.id() ) < 1
                    && brep_.nb_incidences( surface.component_id.id() ) < 1 )
                {
                    if( verbose )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of surface with uuid '",
                            surface.component_id.id().string(),
                            "', which is neither internal to a block nor a "
                            "boundary of a block." );
                    }
                    return true;
                }
            }
            return false;
        }

        bool BRepSurfacesTopologyImpl::
            vertex_is_part_of_surface_with_invalid_internal_topology(
                const index_t unique_vertex_index, bool verbose ) const
        {
            for( const auto surface : brep_.mesh_component_vertices(
                     unique_vertex_index, Surface3D::component_type_static() ) )
            {
                const auto embeddings =
                    brep_.embeddings( surface.component_id.id() );

                for( const auto embedding : embeddings )
                {
                    if( brep_.Relationships::is_boundary(
                            surface.component_id.id(), embedding.id() ) )
                    {
                        if( verbose )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of surface with uuid '",
                                surface.component_id.id().string(),
                                "', which is both internal and boundary of "
                                "block with uuid '",
                                embedding.id().string(), "'." );
                        }
                        return true;
                    }
                }
            }
            return false;
        }

        bool BRepSurfacesTopologyImpl::vertex_is_part_of_invalid_unique_surface(
            index_t unique_vertex_index, bool verbose ) const
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
                if( verbose )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one surface, but is part of more "
                        "than two blocks." );
                }
                return true;
            }
            if( brep_.nb_embeddings( surface_id ) > 0 )
            {
                if( brep_blocks_are_meshed( brep_ ) )
                {
                    if( blocks.size() != 1 )
                    {
                        if( verbose )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of only one surface, which is "
                                "embedded, but not part of only one block." );
                        }
                        return true;
                    }
                    else if( !brep_.Relationships::is_internal(
                                 surface_id, blocks[0].component_id.id() ) )
                    {
                        if( verbose )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of only one surface, which is "
                                "embedded, and one block, but the surface is "
                                "not internal to the block." );
                        }
                        return true;
                    }
                }
            }
            else
            {
                for( const auto& block : blocks )
                {
                    if( !brep_.Relationships::is_boundary(
                            surface_id, block.component_id.id() ) )
                    {
                        if( verbose )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of only one surface, with uuid'",
                                surface_id.string(),
                                "' which is not embedded, but not boundary "
                                "either of block with uuid '",
                                block.component_id.id().string(),
                                "', in which the vertex is." );
                        }
                        return true;
                    }
                }
            }
            return false;
        }

        bool BRepSurfacesTopologyImpl::
            vertex_is_part_of_invalid_multiple_surfaces(
                index_t unique_vertex_index, bool verbose ) const
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
                if( verbose )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of multiple surfaces, but not part of any "
                        "line." );
                }
                return true;
            }
            if( lines.size() == 1 )
            {
                if( !brep_
                         .mesh_component_vertices( unique_vertex_index,
                             Corner3D::component_type_static() )
                         .empty() )
                {
                    if( verbose )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of multiple surfaces and only one line, "
                            "but is a corner." );
                    }
                    return true;
                }
                for( const auto& surface : surfaces )
                {
                    if( !brep_.Relationships::is_boundary(
                            lines[0].component_id.id(),
                            surface.component_id.id() )
                        && !brep_.Relationships::is_internal(
                            lines[0].component_id.id(),
                            surface.component_id.id() ) )
                    {
                        if( verbose )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of multiple surfaces and only one "
                                "line, with uuid'",
                                lines[0].component_id.id().string(),
                                "', but surface with uuid '",
                                surface.component_id.id().string(),
                                "', in which the vertex is, is neither "
                                "incident nor embedding of the line." );
                        }
                        return true;
                    }
                }
            }
            else
            {
                for( const auto& line : lines )
                {
                    if( brep_.nb_embeddings( line.component_id.id() ) < 1
                        && !line_is_boundary_of_at_least_two_surfaces_or_one_embedding_surface(
                            brep_, line, surfaces ) )
                    {
                        if( verbose )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of multiple surfaces and multiple "
                                "lines, but line with uuid'",
                                lines[0].component_id.id().string(),
                                "' is neither internal, nor a boundary of at "
                                "least two surfaces or one embedding "
                                "surface." );
                        }
                        return true;
                    }
                }
            }
            return false;
        }
    } // namespace detail
} // namespace geode