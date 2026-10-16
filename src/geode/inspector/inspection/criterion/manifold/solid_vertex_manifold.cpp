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

#include <geode/inspector/inspection/criterion/manifold/solid_vertex_manifold.hpp>

#include <absl/algorithm/container.h>
#include <absl/container/inlined_vector.h>

#include <async++.h>

#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

namespace
{
    bool polyhedra_around_vertex_are_the_same(
        absl::Span< const geode::PolyhedronVertex > pav1,
        absl::Span< const geode::PolyhedronVertex > pav2 )
    {
        if( pav1.size() != pav2.size() )
        {
            return false;
        }
        for( const auto& polyhedron_vertex : pav1 )
        {
            if( absl::c_find( pav2, polyhedron_vertex ) == pav2.end() )
            {
                return false;
            }
        }
        return true;
    }

    template < geode::index_t dimension >
    std::vector< geode::PolyhedraAroundVertex > polyhedra_around_vertices(
        const geode::SolidMesh< dimension >& mesh )
    {
        std::vector< geode::PolyhedraAroundVertex >
            polyhedra_around_vertices_list( mesh.nb_vertices() );
        for( const auto polyhedron_id : geode::Range{ mesh.nb_polyhedra() } )
        {
            for( const auto polyhedron_vertex_id :
                geode::LRange{ mesh.nb_polyhedron_vertices( polyhedron_id ) } )
            {
                polyhedra_around_vertices_list[mesh.polyhedron_vertex(
                                                   { polyhedron_id,
                                                       polyhedron_vertex_id } )]
                    .push_back( { polyhedron_id, polyhedron_vertex_id } );
            }
        }
        return polyhedra_around_vertices_list;
    }

    // Propagates around the vertex from its first polyhedron through the
    // adjacencies of the facets containing it, like
    // SolidMesh::polyhedra_around_vertex but without its cache: the vertex is
    // manifold if every polyhedron containing it is reached.
    template < geode::index_t dimension >
    bool vertex_is_manifold( const geode::SolidMesh< dimension >& mesh,
        geode::index_t vertex_id,
        absl::Span< const geode::PolyhedronVertex > polyhedron_vertices )
    {
        if( polyhedron_vertices.empty() )
        {
            return true;
        }
        absl::InlinedVector< geode::PolyhedronVertex, 32 > reached;
        absl::InlinedVector< geode::index_t, 32 > visited;
        absl::InlinedVector< geode::PolyhedronVertex, 32 > to_visit;
        to_visit.push_back( polyhedron_vertices.front() );
        visited.push_back( polyhedron_vertices.front().polyhedron_id );
        while( !to_visit.empty() )
        {
            const auto polyhedron_vertex = to_visit.back();
            to_visit.pop_back();
            reached.push_back( polyhedron_vertex );
            if( reached.size() > polyhedron_vertices.size() )
            {
                return false;
            }
            for( const auto& polyhedron_facet :
                mesh.polyhedron_vertex_facets( polyhedron_vertex ) )
            {
                const auto adjacent =
                    mesh.polyhedron_adjacent( polyhedron_facet );
                if( !adjacent )
                {
                    continue;
                }
                const auto adjacent_id = adjacent.value();
                if( adjacent_id >= mesh.nb_polyhedra() )
                {
                    return false;
                }
                if( absl::c_contains( visited, adjacent_id ) )
                {
                    continue;
                }
                visited.push_back( adjacent_id );
                if( const auto vertex_in_adjacent =
                        mesh.vertex_in_polyhedron( adjacent_id, vertex_id ) )
                {
                    to_visit.emplace_back(
                        adjacent_id, vertex_in_adjacent.value() );
                }
            }
        }
        return polyhedra_around_vertex_are_the_same(
            polyhedron_vertices, reached );
    }

    template < geode::index_t dimension >
    std::vector< char > non_manifold_vertices_flags(
        const geode::SolidMesh< dimension >& mesh )
    {
        const auto polyhedra_around_vertices_list =
            polyhedra_around_vertices( mesh );
        std::vector< char > non_manifold( mesh.nb_vertices(), false );
        async::parallel_for(
            async::irange( geode::index_t{ 0 }, mesh.nb_vertices() ),
            [&mesh, &polyhedra_around_vertices_list, &non_manifold](
                geode::index_t vertex_id ) {
                non_manifold[vertex_id] = !vertex_is_manifold( mesh, vertex_id,
                    polyhedra_around_vertices_list[vertex_id] );
            } );
        return non_manifold;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SolidMeshVertexManifold< dimension >::Impl
    {
    public:
        Impl( const SolidMesh< dimension >& mesh ) : mesh_( mesh ) {}

        bool mesh_vertices_are_manifold() const
        {
            return !absl::c_contains( non_manifold_vertices_flags( mesh_ ), 1 );
        }

        InspectionIssues< index_t > non_manifold_vertices() const
        {
            const auto non_manifold = non_manifold_vertices_flags( mesh_ );
            InspectionIssues< geode::index_t > non_manifold_vertices{
                "non manifold vertices"
            };
            for( const auto vertex_id : geode::Range{ mesh_.nb_vertices() } )
            {
                if( non_manifold[vertex_id] )
                {
                    non_manifold_vertices.add_issue( vertex_id,
                        absl::StrCat( "vertex ", vertex_id, ", at position [",
                            mesh_.point( vertex_id ).string(), "]" ) );
                }
            }
            return non_manifold_vertices;
        }

    private:
        const SolidMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    SolidMeshVertexManifold< dimension >::SolidMeshVertexManifold(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshVertexManifold< dimension >::~SolidMeshVertexManifold() = default;

    template < index_t dimension >
    bool
        SolidMeshVertexManifold< dimension >::mesh_vertices_are_manifold() const
    {
        return impl_->mesh_vertices_are_manifold();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SolidMeshVertexManifold< dimension >::non_manifold_vertices() const
    {
        return impl_->non_manifold_vertices();
    }

    template class opengeode_inspector_inspection_api
        SolidMeshVertexManifold< 3 >;
} // namespace geode
