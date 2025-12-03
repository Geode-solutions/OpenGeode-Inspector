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
        const auto nb_internal_surface_cmvs = count_cmvs(
            unique_vertex_cmvs.surface_cmvs,
            [&block_uuid, &brep]( const auto& cmv ) {
                return brep.is_internal( brep.surface( cmv.component_id.id() ),
                    brep.block( block_uuid ) );
            } );
        const auto nb_boundary_surface_cmvs = count_cmvs(
            unique_vertex_cmvs.surface_cmvs,
            [&block_uuid, &brep]( const auto& cmv ) {
                return brep.is_boundary( brep.surface( cmv.component_id.id() ),
                    brep.block( block_uuid ) );
            } );
        const auto nb_boundary_line_cmvs =
            count_cmvs( unique_vertex_cmvs.line_cmvs,
                [&block_uuid, &brep]( const auto& cmv ) {
                    for( const auto& block_boundary :
                        brep.boundaries( brep.block( block_uuid ) ) )
                    {
                        for( const auto& surface_boundary :
                            brep.boundaries( block_boundary ) )
                        {
                            if( surface_boundary.id() == cmv.component_id.id() )
                            {
                                return true;
                            }
                        }
                        for( const auto& surface_internal :
                            brep.internal_lines( block_boundary ) )
                        {
                            if( surface_internal.id() == cmv.component_id.id() )
                            {
                                return true;
                            }
                        }
                    }
                    return false;
                } );
        if( unique_vertex_cmvs.corner_cmvs.size() == 1
            && nb_internal_surface_cmvs == 0 )
        {
            if( nb_boundary_line_cmvs == 1 )
            {
                return std::make_pair( 1,
                    nb_block_cmvs == 1
                        ? std::nullopt
                        : std::make_optional( absl::StrCat( "unique vertex ",
                              unique_vertex_id, " at position [",
                              brep.block( unique_vertex_cmvs.block_cmvs[0]
                                              .component_id.id() )
                                  .mesh()
                                  .point(
                                      unique_vertex_cmvs.block_cmvs[0].vertex )
                                  .string(),
                              "] is part of Block ",
                              brep.block( block_uuid ).name(), " (",
                              block_uuid.string(),
                              ") and exactly one Corner and one Line but has ",
                              nb_block_cmvs,
                              " Block mesh vertices (should be "
                              "1)." ) ) );
            }
            const auto predicted_nb_block_cmvs =
                nb_boundary_surface_cmvs + unique_vertex_cmvs.corner_cmvs.size()
                - nb_boundary_line_cmvs;
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
                          "]", " is part of Block ",
                          brep.block( block_uuid ).name(), " (",
                          block_uuid.string(),
                          ") and of a Corner, and of no internal Surface, ",
                          "and of ", nb_boundary_surface_cmvs,
                          " boundary Surface(s), and of ",
                          nb_boundary_line_cmvs,
                          " Line(s) on Block boundaries, with ", nb_block_cmvs,
                          " Block component mesh vertices (should be ",
                          predicted_nb_block_cmvs, ")." ) ) );
        }
        if( nb_internal_surface_cmvs == 0 )
        {
            const auto predicted_nb_block_cmvs =
                nb_boundary_line_cmvs == 0 ? 1 : nb_boundary_surface_cmvs / 2;
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
                          "]", " is part of the Block ",
                          brep.block( block_uuid ).name(), " (",
                          block_uuid.string(),
                          ") and none of its internal Surfaces but has ",
                          nb_block_cmvs, " Block mesh vertices (should be ",
                          predicted_nb_block_cmvs, ")." ) ) );
        }
        // const auto nb_free_line_cmvs = count_cmvs(
        //     unique_vertex_cmvs.line_cmvs, [&brep]( const auto& cmv ) {
        //         return brep.nb_incidences( cmv.component_id.id() ) == 1
        //                && brep.nb_embedding_surfaces(
        //                       brep.line( cmv.component_id.id() ) )
        //                       == 0;
        //     } );
        // auto predicted_nb_block_cmvs =
        //     nb_internal_surface_cmvs < nb_free_line_cmvs + 1
        //         ? static_cast< geode::index_t >( 1 )
        //         : nb_internal_surface_cmvs - nb_free_line_cmvs;
        // if( nb_internal_surface_cmvs - nb_free_line_cmvs == 1 )
        // {
        //     predicted_nb_block_cmvs++;
        // }
        // if( nb_boundary_surface_cmvs > 1
        //     && unique_vertex_cmvs.corner_cmvs.empty() )
        // {
        //     predicted_nb_block_cmvs += ( nb_boundary_surface_cmvs - 2 ) / 2;
        // }
        /// predicted_nb_block_cmvs = 1 + nb_internal_surfaces - nb_free_lines -
        /// nb_lines_boundary_to_strictly_2_internal_surfaces
        absl::flat_hash_set< geode::uuid > uuids_used;
        const auto nb_internal_surfaces = count_cmvs(
            unique_vertex_cmvs.surface_cmvs,
            [&block_uuid, &brep, &uuids_used]( const auto& cmv ) {
                if( !uuids_used.emplace( cmv.component_id.id() ).second )
                {
                    return false;
                }
                return brep.is_internal( brep.surface( cmv.component_id.id() ),
                    brep.block( block_uuid ) );
            } );
        uuids_used.clear();
        /// Lines that are free but incident to the same surface are counted
        /// only once
        const auto nb_free_lines = count_cmvs( unique_vertex_cmvs.line_cmvs,
            [&brep, &uuids_used]( const auto& cmv ) {
                if( !uuids_used.emplace( cmv.component_id.id() ).second )
                {
                    return false;
                }
                if( brep.nb_incidences( cmv.component_id.id() ) != 1 )
                {
                    return false;
                }
                for( const auto& incident_surface :
                    brep.incidences( brep.line( cmv.component_id.id() ) ) )
                {
                    if( !uuids_used.emplace( incident_surface.id() ).second )
                    {
                        return false;
                    }
                }
                return brep.nb_embedding_surfaces(
                           brep.line( cmv.component_id.id() ) )
                       == 0;
            } );
        uuids_used.clear();
        const auto nb_lines_boundary_to_two_internal_lines =
            count_cmvs( unique_vertex_cmvs.line_cmvs,
                [&block_uuid, &brep, &uuids_used]( const auto& cmv ) {
                    if( !uuids_used.emplace( cmv.component_id.id() ).second )
                    {
                        return false;
                    }
                    if( brep.nb_incidences( cmv.component_id.id() ) != 2 )
                    {
                        return false;
                    }
                    for( const auto& incident_surface :
                        brep.incidences( brep.line( cmv.component_id.id() ) ) )
                    {
                        if( !brep.is_internal(
                                incident_surface, brep.block( block_uuid ) ) )
                        {
                            return false;
                        }
                    }
                    return true;
                } );
        geode::index_t predicted_nb_block_cmvs{
            1 + nb_internal_surfaces - nb_free_lines
            - nb_lines_boundary_to_two_internal_lines
        };
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
                      "] is part of Block ", brep.block( block_uuid ).name(),
                      " (", block_uuid.string(), "), and part of ",
                      nb_internal_surfaces,
                      " surfaces internal to that block, ", nb_free_lines,
                      " free lines on different surfaces, and ",
                      nb_lines_boundary_to_two_internal_lines,
                      " lines boundary to strictly two internal lines, with ",
                      nb_block_cmvs, " Block CMVs (should be ",
                      predicted_nb_block_cmvs, ")." ) ) );
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