/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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

#include <geode/inspector/topology/internal/expected_nb_cmvs.hpp>

#include <absl/container/flat_hash_set.h>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace
{
    template < typename Condition >
    geode::index_t count_cmvs(
        absl::Span< const geode::ComponentMeshVertex > cmvs,
        const Condition& condition )
    {
        geode::index_t counter{ 0 };
        for( const auto& cmv : cmvs )
        {
            if( condition( cmv ) )
            {
                counter++;
            }
        }
        return counter;
    }

    bool line_is_on_block_boundary( const geode::BRep& brep,
        const geode::Line3D& line,
        const geode::Block3D& block )
    {
        for( const auto& boundary_surface : brep.boundaries( block ) )
        {
            if( brep.is_boundary( line, boundary_surface )
                || brep.is_internal( line, boundary_surface ) )
            {
                return true;
            }
        }
        return false;
    }

    bool line_is_inside_block( const geode::BRep& brep,
        const geode::Line3D& line,
        const geode::Block3D& block )
    {
        for( const auto& incident_surface : brep.incidences( line ) )
        {
            if( !brep.is_internal( incident_surface, block ) )
            {
                return false;
            }
        }
        return true;
    }

    std::pair< geode::index_t, std::optional< std::string > >
        expected_block_cmvs_and_error( const geode::BRep& brep,
            geode::index_t unique_vertex_id,
            const geode::uuid& block_uuid,
            const geode::internal::VertexCMVsByComponent& unique_vertex_cmvs )
    {
        const auto nb_block_cmvs = count_cmvs(
            unique_vertex_cmvs.block_cmvs, [&block_uuid]( const auto& cmv ) {
                return cmv.component_id.id() == block_uuid;
            } );
        const auto& block = brep.block( block_uuid );
        geode::index_t nb_boundary_surface_cmvs{ 0 };
        geode::index_t nb_internal_surface_cmvs{ 0 };
        for( const auto& cmv : unique_vertex_cmvs.surface_cmvs )
        {
            if( brep.is_boundary(
                    brep.surface( cmv.component_id.id() ), block ) )
            {
                nb_boundary_surface_cmvs++;
            }
            else if( brep.is_internal(
                         brep.surface( cmv.component_id.id() ), block ) )
            {
                nb_internal_surface_cmvs++;
            }
        }
        geode::index_t nb_line_on_boundary_cmvs{ 0 };
        geode::index_t nb_line_internal_to_internal_surface_cmvs{ 0 };
        geode::index_t nb_free_line_cmvs{ 0 };
        geode::index_t nb_line_boundary_to_several_internal_surfaces_cmvs{ 0 };
        for( const auto& cmv : unique_vertex_cmvs.line_cmvs )
        {
            const auto& cmv_line = brep.line( cmv.component_id.id() );
            if( brep.nb_embedding_blocks( cmv_line ) != 0 )
            {
                continue;
            }
            if( line_is_on_block_boundary( brep, cmv_line, block ) )
            {
                nb_line_on_boundary_cmvs++;
                continue;
            }
            if( brep.nb_embedding_surfaces( cmv_line ) > 0 )
            {
                for( const auto& incident_surface :
                    brep.embedding_surfaces( cmv_line ) )
                {
                    if( brep.is_internal( incident_surface, block ) )
                    {
                        nb_line_internal_to_internal_surface_cmvs++;
                        break;
                    }
                }
                continue;
            }
            if( !line_is_inside_block( brep, cmv_line, block ) )
            {
                continue;
            }
            if( brep.nb_incidences( cmv.component_id.id() ) == 1 )
            {
                nb_free_line_cmvs++;
                continue;
            }
            nb_line_boundary_to_several_internal_surfaces_cmvs++;
        }
        geode::index_t predicted_nb_block_cmvs{ 1 + nb_internal_surface_cmvs };
        if( nb_boundary_surface_cmvs > 0 )
        {
            predicted_nb_block_cmvs +=
                nb_boundary_surface_cmvs - 1 - nb_line_on_boundary_cmvs;
        }
        const geode::index_t nb_line_cmvs_to_remove{
            nb_line_internal_to_internal_surface_cmvs + nb_free_line_cmvs
            + nb_line_boundary_to_several_internal_surfaces_cmvs
        };
        predicted_nb_block_cmvs -= nb_line_cmvs_to_remove;
        if( nb_line_cmvs_to_remove != 0 || nb_line_on_boundary_cmvs != 0 )
        {
            predicted_nb_block_cmvs += unique_vertex_cmvs.corner_cmvs.size();
        }
        return std::make_pair( predicted_nb_block_cmvs,
            nb_block_cmvs == predicted_nb_block_cmvs
                ? std::nullopt
                : std::make_optional( absl::StrCat( "unique vertex ",
                      unique_vertex_id, " at position [",
                      brep.block( unique_vertex_cmvs.block_cmvs[0]
                                      .component_id.id() )
                          .mesh()
                          .point( unique_vertex_cmvs.block_cmvs[0].vertex )
                          .string(),
                      "] is part of Block ", block.name(), " (",
                      block_uuid.string(), "), and has ",
                      unique_vertex_cmvs.corner_cmvs.size(),
                      " cmvs of corners, ", nb_internal_surface_cmvs,
                      " cmvs of surfaces internal to that block, ",
                      nb_line_internal_to_internal_surface_cmvs,
                      " cmvs of lines internal to one surface internal to that "
                      "block, ",
                      nb_free_line_cmvs, " cmvs of free lines, ",
                      nb_line_boundary_to_several_internal_surfaces_cmvs,
                      " cmvs of lines boundary to several internal surfaces, ",
                      nb_boundary_surface_cmvs,
                      " cmvs of surfaces boundary to the block, and ",
                      nb_line_on_boundary_cmvs,
                      " cmvs of lines on the boundary, with ", nb_block_cmvs,
                      " Block CMVs (expected ", predicted_nb_block_cmvs,
                      ") with valid topology." ) ) );
    }
} // namespace

namespace geode
{
    namespace internal
    {
        VertexCMVsByComponent vertex_cmvs_by_component(
            const BRep& brep, index_t unique_vertex_id )
        {
            VertexCMVsByComponent result;
            for( const auto& cmv :
                brep.component_mesh_vertices( unique_vertex_id ) )
            {
                if( cmv.component_id.type()
                    == Block3D::component_type_static() )
                {
                    result.block_cmvs.push_back( cmv );
                }
                if( cmv.component_id.type()
                    == Surface3D::component_type_static() )
                {
                    result.surface_cmvs.push_back( cmv );
                }
                if( cmv.component_id.type() == Line3D::component_type_static() )
                {
                    result.line_cmvs.push_back( cmv );
                }
                if( cmv.component_id.type()
                    == Corner3D::component_type_static() )
                {
                    result.corner_cmvs.push_back( cmv );
                }
            }
            return result;
        }

        index_t nb_expected_block_cmvs( const BRep& brep,
            index_t unique_vertex_id,
            const geode::uuid& block_uuid,
            const VertexCMVsByComponent& unique_vertex_cmvs )
        {
            return std::get< 0 >( expected_block_cmvs_and_error(
                brep, unique_vertex_id, block_uuid, unique_vertex_cmvs ) );
        }

        std::optional< std::string > wrong_nb_expected_block_cmvs(
            const BRep& brep,
            index_t unique_vertex_id,
            const geode::uuid& block_uuid,
            const VertexCMVsByComponent& unique_vertex_cmvs )
        {
            return std::get< 1 >( expected_block_cmvs_and_error(
                brep, unique_vertex_id, block_uuid, unique_vertex_cmvs ) );
        }
    } // namespace internal
} // namespace geode