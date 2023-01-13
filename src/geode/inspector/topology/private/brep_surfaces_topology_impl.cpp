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

#include <geode/inspector/topology/private/brep_surfaces_topology_impl.h>

#include <absl/algorithm/container.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

#include <geode/inspector/topology/private/topology_helpers.h>

namespace
{
    bool line_is_boundary_of_at_least_two_surfaces_or_one_embedding_surface(
        const geode::BRep& brep,
        const geode::uuid& line_uuid,
        const std::vector< geode::uuid >& surface_uuids )
    {
        geode::local_index_t counter{ 0 };
        for( const auto& surface_id : surface_uuids )
        {
            if( brep.Relationships::is_boundary( line_uuid, surface_id ) )
            {
                if( brep.nb_embeddings( surface_id ) > 0 )
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
} // namespace

namespace geode
{
    namespace detail
    {
        BRepSurfacesTopologyImpl::BRepSurfacesTopologyImpl(
            const BRep& brep, bool verbose )
            : brep_( brep ), verbose_( verbose )
        {
        }
        BRepSurfacesTopologyImpl::BRepSurfacesTopologyImpl( const BRep& brep )
            : BRepSurfacesTopologyImpl( brep, false )
        {
        }

        bool BRepSurfacesTopologyImpl::brep_vertex_surfaces_topology_is_valid(
            index_t unique_vertex_index ) const
        {
            const auto surfaces = brep_.component_mesh_vertices(
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
                    unique_vertex_index )
                || vertex_is_part_of_line_and_not_on_surface_border(
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
            for( const auto surface_id : components_uuids(
                     brep_.component_mesh_vertices( unique_vertex_index,
                         Surface3D::component_type_static() ) ) )
            {
                if( brep_.nb_embeddings( surface_id ) < 1
                    && brep_.nb_incidences( surface_id ) < 1 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of surface with uuid '",
                            surface_id.string(),
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
                const index_t unique_vertex_index ) const
        {
            for( const auto surface_id : components_uuids(
                     brep_.component_mesh_vertices( unique_vertex_index,
                         Surface3D::component_type_static() ) ) )
            {
                for( const auto& embedding : brep_.embeddings( surface_id ) )
                {
                    if( brep_.Relationships::is_boundary(
                            surface_id, embedding.id() ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of surface with uuid '",
                                surface_id.string(),
                                "', which is both internal and boundary of "
                                "block with uuid '",
                                embedding.id().string(), "'." );
                        }
                        return true;
                    }
                    if( brep_blocks_are_meshed( brep_ )
                        && !absl::c_any_of(
                            brep_.component_mesh_vertices( unique_vertex_index,
                                Block3D::component_type_static() ),
                            [&embedding]( const ComponentMeshVertex& cmv ) {
                                return cmv.component_id.id() == embedding.id();
                            } ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of surface with uuid '",
                                surface_id.string(),
                                "', which is embedded in block with uuid '",
                                embedding.id().string(),
                                "', but the unique vertex is not linked to the "
                                "block vertices." );
                        }
                        return true;
                    }
                }
            }
            return false;
        }

        bool BRepSurfacesTopologyImpl::vertex_is_part_of_invalid_unique_surface(
            index_t unique_vertex_index ) const
        {
            const auto surface_uuids =
                components_uuids( brep_.component_mesh_vertices(
                    unique_vertex_index, Surface3D::component_type_static() ) );
            if( surface_uuids.size() != 1 )
            {
                return false;
            }
            const auto& surface_id = surface_uuids[0];
            const auto block_uuids =
                components_uuids( brep_.component_mesh_vertices(
                    unique_vertex_index, Block3D::component_type_static() ) );
            if( block_uuids.size() > 2 )
            {
                if( verbose_ )
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
                    if( block_uuids.size() != 1 )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of only one surface, which is "
                                "embedded, but not part of only one block." );
                        }
                        return true;
                    }
                    else if( !brep_.Relationships::is_internal(
                                 surface_id, block_uuids[0] ) )
                    {
                        if( verbose_ )
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
                for( const auto& block_id : block_uuids )
                {
                    if( !brep_.Relationships::is_boundary(
                            surface_id, block_id ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of only one surface, with uuid'",
                                surface_id.string(),
                                "' which is not embedded, but not boundary "
                                "either of block with uuid '",
                                block_id.string(),
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
                index_t unique_vertex_index ) const
        {
            const auto surface_uuids =
                components_uuids( brep_.component_mesh_vertices(
                    unique_vertex_index, Surface3D::component_type_static() ) );
            if( surface_uuids.size() < 2 )
            {
                return false;
            }
            const auto line_cmvs = brep_.component_mesh_vertices(
                unique_vertex_index, Line3D::component_type_static() );
            const auto line_uuids = components_uuids( line_cmvs );
            if( line_uuids.empty() )
            {
                if( verbose_ )
                {
                    Logger::info( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of multiple surfaces, but not part of any "
                        "line." );
                }
                return true;
            }
            if( line_uuids.size() == 1 )
            {
                if( !brep_
                         .component_mesh_vertices( unique_vertex_index,
                             Corner3D::component_type_static() )
                         .empty()
                    && line_cmvs.size() < 2 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of multiple surfaces and only one line, "
                            "but is a corner." );
                    }
                    return true;
                }
                for( const auto& surface_id : surface_uuids )
                {
                    if( !brep_.Relationships::is_boundary(
                            line_uuids[0], surface_id )
                        && !brep_.Relationships::is_internal(
                            line_uuids[0], surface_id ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of multiple surfaces and only one "
                                "line, with uuid'",
                                line_uuids[0].string(),
                                "', but surface with uuid '",
                                surface_id.string(),
                                "', in which the vertex is, is neither "
                                "incident nor embedding of the line." );
                        }
                        return true;
                    }
                }
            }
            else
            {
                for( const auto& line_id : line_uuids )
                {
                    if( brep_.nb_embeddings( line_id ) < 1
                        && !line_is_boundary_of_at_least_two_surfaces_or_one_embedding_surface(
                            brep_, line_id, surface_uuids ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Unique vertex with index ",
                                unique_vertex_index,
                                " is part of multiple surfaces and multiple "
                                "lines, but line with uuid'",
                                line_id.string(),
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

        bool BRepSurfacesTopologyImpl::
            vertex_is_part_of_line_and_not_on_surface_border(
                index_t unique_vertex_index ) const
        {
            const auto lines = brep_.component_mesh_vertices(
                unique_vertex_index, Line3D::component_type_static() );
            if( lines.empty() )
            {
                return false;
            }
            for( const auto& surface_vertex : brep_.component_mesh_vertices(
                     unique_vertex_index, Surface3D::component_type_static() ) )
            {
                const auto& surface =
                    brep_.surface( surface_vertex.component_id.id() );
                if( !surface.mesh().is_vertex_on_border(
                        surface_vertex.vertex ) )
                {
                    for( const auto& cmv_line : lines )
                    {
                        const auto& line =
                            brep_.line( cmv_line.component_id.id() );
                        if( brep_.is_boundary( line, surface )
                            || brep_.is_internal( line, surface ) )
                        {
                            if( verbose_ )
                            {
                                Logger::info( "Unique vertex with index ",
                                    unique_vertex_index,
                                    " is part of a line and of surface with "
                                    "uuid '",
                                    surface_vertex.component_id.id().string(),
                                    "' but the associated vertex in the "
                                    "surface mesh "
                                    "is not on the mesh border." );
                            }
                            return true;
                        }
                    }
                }
            }
            return false;
        }
    } // namespace detail
} // namespace geode
