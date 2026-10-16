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

#include <geode/inspector/inspection/criterion/manifold/surface_vertex_manifold.hpp>

#include <absl/algorithm/container.h>
#include <absl/container/inlined_vector.h>

#include <async++.h>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

namespace
{
    bool polygons_around_vertex_are_the_same(
        absl::Span< const geode::PolygonVertex > pav1,
        absl::Span< const geode::PolygonVertex > pav2 )
    {
        if( pav1.size() != pav2.size() )
        {
            return false;
        }
        for( const auto& polygon_vertex : pav1 )
        {
            if( absl::c_find( pav2, polygon_vertex ) == pav2.end() )
            {
                return false;
            }
        }
        return true;
    }

    template < geode::index_t dimension >
    std::vector< geode::PolygonsAroundVertex > polygons_around_vertices(
        const geode::SurfaceMesh< dimension >& mesh )
    {
        std::vector< geode::PolygonsAroundVertex > polygons_list(
            mesh.nb_vertices() );
        for( const auto polygon_id : geode::Range{ mesh.nb_polygons() } )
        {
            for( const auto polygon_vertex_id :
                geode::LRange{ mesh.nb_polygon_vertices( polygon_id ) } )
            {
                polygons_list[mesh.polygon_vertex(
                                  { polygon_id, polygon_vertex_id } )]
                    .push_back( { polygon_id, polygon_vertex_id } );
            }
        }
        return polygons_list;
    }

    enum struct VERTEX_STATUS : char
    {
        manifold,
        non_manifold,
        broken_adjacencies
    };

    // Same walk as SurfaceMesh::polygons_around_vertex, without its cache.
    // Returns std::nullopt on broken adjacencies.
    template < geode::index_t dimension >
    std::optional< std::optional< geode::PolygonVertex > >
        next_polygon_vertex_around( const geode::SurfaceMesh< dimension >& mesh,
            const geode::PolygonVertex& polygon_vertex,
            bool& vertex_is_next )
    {
        const auto exit_vertex =
            vertex_is_next ? polygon_vertex
                           : mesh.previous_polygon_vertex( polygon_vertex );
        const geode::PolygonEdge exit_edge{ exit_vertex };
        const auto adjacent = mesh.polygon_adjacent( exit_edge );
        if( !adjacent )
        {
            return std::optional< geode::PolygonVertex >{};
        }
        if( adjacent.value() >= mesh.nb_polygons() )
        {
            return std::nullopt;
        }
        const auto adjacent_edge = mesh.polygon_adjacent_edge( exit_edge );
        if( !adjacent_edge )
        {
            return std::nullopt;
        }
        if( mesh.polygon_edge_vertex( exit_edge, 0 )
                == mesh.polygon_edge_vertex( adjacent_edge.value(), 0 )
            && mesh.polygon_edge_vertex( exit_edge, 1 )
                   == mesh.polygon_edge_vertex( adjacent_edge.value(), 1 ) )
        {
            vertex_is_next = !vertex_is_next;
        }
        return std::optional< geode::PolygonVertex >{
            vertex_is_next ? mesh.next_polygon_vertex(
                                 geode::PolygonVertex{ adjacent_edge.value() } )
                           : geode::PolygonVertex{ adjacent_edge.value() }
        };
    }

    template < geode::index_t dimension >
    VERTEX_STATUS vertex_status( const geode::SurfaceMesh< dimension >& mesh,
        absl::Span< const geode::PolygonVertex > polygon_vertices )
    {
        if( polygon_vertices.empty() )
        {
            return VERTEX_STATUS::manifold;
        }
        const auto& first = polygon_vertices.front();
        absl::InlinedVector< geode::PolygonVertex, 16 > reached{ first };
        try
        {
            bool vertex_is_next{ false };
            auto current = first;
            bool closed{ false };
            while( true )
            {
                const auto next =
                    next_polygon_vertex_around( mesh, current, vertex_is_next );
                if( !next )
                {
                    return VERTEX_STATUS::broken_adjacencies;
                }
                if( !next.value() )
                {
                    break;
                }
                if( next.value() == first )
                {
                    closed = true;
                    break;
                }
                reached.push_back( next.value().value() );
                if( reached.size() > polygon_vertices.size() )
                {
                    return VERTEX_STATUS::non_manifold;
                }
                current = next.value().value();
            }
            if( !closed )
            {
                vertex_is_next = true;
                current = first;
                while( true )
                {
                    const auto next = next_polygon_vertex_around(
                        mesh, current, vertex_is_next );
                    if( !next )
                    {
                        return VERTEX_STATUS::broken_adjacencies;
                    }
                    if( !next.value() )
                    {
                        break;
                    }
                    reached.push_back( next.value().value() );
                    if( reached.size() > polygon_vertices.size() )
                    {
                        return VERTEX_STATUS::non_manifold;
                    }
                    current = next.value().value();
                }
            }
        }
        catch( const geode::OpenGeodeException& )
        {
            return VERTEX_STATUS::broken_adjacencies;
        }
        return polygons_around_vertex_are_the_same( polygon_vertices, reached )
                   ? VERTEX_STATUS::manifold
                   : VERTEX_STATUS::non_manifold;
    }

    template < geode::index_t dimension >
    std::vector< VERTEX_STATUS > vertices_status(
        const geode::SurfaceMesh< dimension >& mesh )
    {
        const auto polygons_around_vertices_list =
            polygons_around_vertices( mesh );
        std::vector< VERTEX_STATUS > status( mesh.nb_vertices() );
        async::parallel_for(
            async::irange( geode::index_t{ 0 }, mesh.nb_vertices() ),
            [&mesh, &polygons_around_vertices_list, &status](
                geode::index_t vertex_id ) {
                status[vertex_id] = vertex_status(
                    mesh, polygons_around_vertices_list[vertex_id] );
            } );
        return status;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SurfaceMeshVertexManifold< dimension >::Impl
    {
    public:
        Impl( const SurfaceMesh< dimension >& mesh ) : mesh_( mesh ) {}

        bool mesh_vertices_are_manifold() const
        {
            return absl::c_all_of(
                vertices_status( mesh_ ), []( VERTEX_STATUS status ) {
                    return status == VERTEX_STATUS::manifold;
                } );
        }

        InspectionIssues< index_t > non_manifold_vertices() const
        {
            const auto status = vertices_status( mesh_ );
            InspectionIssues< geode::index_t > non_manifold_vertices{
                "non manifold vertices"
            };
            for( const auto vertex_id : geode::Range{ mesh_.nb_vertices() } )
            {
                if( status[vertex_id] == VERTEX_STATUS::non_manifold )
                {
                    non_manifold_vertices.add_issue( vertex_id,
                        absl::StrCat( "vertex ", vertex_id, ", at position [",
                            mesh_.point( vertex_id ).string(), "]" ) );
                }
                else if( status[vertex_id]
                         == VERTEX_STATUS::broken_adjacencies )
                {
                    non_manifold_vertices.add_issue( vertex_id,
                        absl::StrCat( "vertex ", vertex_id, ", at position [",
                            mesh_.point( vertex_id ).string(),
                            "] could not be checked; check issues with "
                            "surface adjacencies" ) );
                }
            }
            return non_manifold_vertices;
        }

    private:
        const SurfaceMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    SurfaceMeshVertexManifold< dimension >::SurfaceMeshVertexManifold(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SurfaceMeshVertexManifold< dimension >::~SurfaceMeshVertexManifold() =
        default;

    template < index_t dimension >
    bool SurfaceMeshVertexManifold< dimension >::mesh_vertices_are_manifold()
        const
    {
        return impl_->mesh_vertices_are_manifold();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SurfaceMeshVertexManifold< dimension >::non_manifold_vertices() const
    {
        return impl_->non_manifold_vertices();
    }

    template class opengeode_inspector_inspection_api
        SurfaceMeshVertexManifold< 2 >;
    template class opengeode_inspector_inspection_api
        SurfaceMeshVertexManifold< 3 >;
} // namespace geode
