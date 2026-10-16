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

#include <geode/inspector/inspection/criterion/manifold/solid_edge_manifold.hpp>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_map.h>
#include <absl/container/inlined_vector.h>

#include <async++.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/detail/vertex_cycle.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace
{
    using Edge = geode::detail::VertexCycle< std::array< geode::index_t, 2 > >;

    bool polyhedra_around_edge_are_the_same(
        absl::Span< const geode::index_t > pae1,
        absl::Span< const geode::index_t > pae2 )
    {
        if( pae1.size() != pae2.size() )
        {
            return false;
        }
        for( const auto& polyhedron_id : pae1 )
        {
            if( absl::c_find( pae2, polyhedron_id ) == pae2.end() )
            {
                return false;
            }
        }
        return true;
    }

    struct EdgePolyhedra
    {
        // Vertices of the edge as first found in a polyhedron
        std::array< geode::index_t, 2 > vertices;
        absl::InlinedVector< geode::index_t, 8 > polyhedra;
    };

    // Unique edges in order of first appearance, with their polyhedra
    template < geode::index_t dimension >
    std::vector< EdgePolyhedra > edges_to_polyhedra_around(
        const geode::SolidMesh< dimension >& mesh )
    {
        std::vector< EdgePolyhedra > edges;
        absl::flat_hash_map< Edge, geode::index_t > edge_ids;
        edge_ids.reserve( 2 * mesh.nb_polyhedra() );
        for( const auto polyhedron_id : geode::Range{ mesh.nb_polyhedra() } )
        {
            for( const auto& polyhedron_edge_vertices :
                mesh.polyhedron_edges_vertices( polyhedron_id ) )
            {
                const auto [it, inserted] = edge_ids.try_emplace(
                    Edge{ polyhedron_edge_vertices }, edges.size() );
                if( inserted )
                {
                    edges.push_back( { polyhedron_edge_vertices, {} } );
                }
                edges[it->second].polyhedra.push_back( polyhedron_id );
            }
        }
        return edges;
    }

    enum struct EDGE_STATUS : char
    {
        manifold,
        non_manifold,
        broken_adjacencies
    };

    template < geode::index_t dimension >
    std::vector< EDGE_STATUS > edges_status(
        const geode::SolidMesh< dimension >& mesh,
        absl::Span< const EdgePolyhedra > edges )
    {
        std::vector< EDGE_STATUS > status( edges.size() );
        async::parallel_for(
            async::irange( geode::index_t{ 0 },
                static_cast< geode::index_t >( edges.size() ) ),
            [&mesh, &edges, &status]( geode::index_t edge_id ) {
                const auto& edge = edges[edge_id];
                try
                {
                    status[edge_id] =
                        polyhedra_around_edge_are_the_same(
                            mesh.polyhedra_around_edge(
                                edge.vertices, edge.polyhedra.front() ),
                            edge.polyhedra )
                            ? EDGE_STATUS::manifold
                            : EDGE_STATUS::non_manifold;
                }
                catch( const geode::OpenGeodeException& )
                {
                    status[edge_id] = EDGE_STATUS::broken_adjacencies;
                }
            } );
        return status;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SolidMeshEdgeManifold< dimension >::Impl
    {
    public:
        Impl( const SolidMesh< dimension >& mesh ) : mesh_( mesh ) {}

        bool mesh_edges_are_manifold() const
        {
            const auto edges = edges_to_polyhedra_around( mesh_ );
            return absl::c_all_of(
                edges_status( mesh_, edges ), []( EDGE_STATUS status ) {
                    return status == EDGE_STATUS::manifold;
                } );
        }

        InspectionIssues< std::array< index_t, 2 > > non_manifold_edges() const
        {
            InspectionIssues< std::array< index_t, 2 > > non_manifold_edges{
                "Non manifold edges"
            };
            const auto edges = edges_to_polyhedra_around( mesh_ );
            const auto status = edges_status( mesh_, edges );
            for( const auto edge_id : Indices{ edges } )
            {
                const auto& vertices = edges[edge_id].vertices;
                if( status[edge_id] == EDGE_STATUS::non_manifold )
                {
                    non_manifold_edges.add_issue( vertices,
                        absl::StrCat( "Non manifold edge between vertices ",
                            vertices[0], " and ", vertices[1] ) );
                }
                else if( status[edge_id] == EDGE_STATUS::broken_adjacencies )
                {
                    non_manifold_edges.add_issue( vertices,
                        absl::StrCat( "Could not check manifold on edge "
                                      "between vertices with index ",
                            vertices[0], " and index ", vertices[1],
                            "; Check issues with solid adjacencies." ) );
                }
            }
            return non_manifold_edges;
        }

    private:
        const SolidMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    SolidMeshEdgeManifold< dimension >::SolidMeshEdgeManifold(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshEdgeManifold< dimension >::~SolidMeshEdgeManifold() = default;

    template < index_t dimension >
    bool SolidMeshEdgeManifold< dimension >::mesh_edges_are_manifold() const
    {
        return impl_->mesh_edges_are_manifold();
    }

    template < index_t dimension >
    InspectionIssues< std::array< index_t, 2 > >
        SolidMeshEdgeManifold< dimension >::non_manifold_edges() const
    {
        return impl_->non_manifold_edges();
    }

    template class opengeode_inspector_inspection_api
        SolidMeshEdgeManifold< 3 >;
} // namespace geode
