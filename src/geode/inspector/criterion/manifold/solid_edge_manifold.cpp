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

#include <geode/inspector/criterion/manifold/solid_edge_manifold.h>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/solid_mesh.h>

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

    template < geode::index_t dimension >
    absl::flat_hash_map< Edge, std::vector< geode::index_t > >
        edges_to_polyhedra_around( const geode::SolidMesh< dimension >& mesh )
    {
        absl::flat_hash_map< Edge, std::vector< geode::index_t > >
            polyhedra_around_edges;
        for( const auto polyhedron_id : geode::Range{ mesh.nb_polyhedra() } )
        {
            for( const auto polyhedron_edge_vertices :
                mesh.polyhedron_edges_vertices( polyhedron_id ) )
            {
                const Edge polyhedron_edge{ polyhedron_edge_vertices };
                if( !polyhedra_around_edges
                         .try_emplace( polyhedron_edge, 1, polyhedron_id )
                         .second )
                {
                    polyhedra_around_edges[polyhedron_edge].push_back(
                        polyhedron_id );
                }
            }
        }
        return polyhedra_around_edges;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SolidMeshEdgeManifold< dimension >::Impl
    {
    public:
        Impl( const SolidMesh< dimension >& mesh, bool verbose )
            : mesh_( mesh ),
              verbose_( verbose ),
              polyhedra_around_edges_( edges_to_polyhedra_around( mesh ) )
        {
        }

        bool mesh_edges_are_manifold() const
        {
            absl::flat_hash_set< Edge > checked_edges;
            for( const auto polyhedron_id : Range{ mesh_.nb_polyhedra() } )
            {
                for( const auto polyhedron_edge_vertices :
                    mesh_.polyhedron_edges_vertices( polyhedron_id ) )
                {
                    Edge polyhedron_edge{ polyhedron_edge_vertices };
                    if( !checked_edges.insert( polyhedron_edge ).second )
                    {
                        continue;
                    }
                    const auto polyhedra_around = mesh_.polyhedra_around_edge(
                        polyhedron_edge_vertices, polyhedron_id );
                    if( !polyhedra_around_edge_are_the_same(
                            mesh_.polyhedra_around_edge(
                                polyhedron_edge_vertices, polyhedron_id ),
                            polyhedra_around_edges_.at( polyhedron_edge ) ) )
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        index_t nb_non_manifold_edges() const
        {
            index_t nb_non_manifold_edges{ 0 };
            absl::flat_hash_set< Edge > checked_edges;
            for( const auto polyhedron_id : Range{ mesh_.nb_polyhedra() } )
            {
                for( const auto polyhedron_edge_vertices :
                    mesh_.polyhedron_edges_vertices( polyhedron_id ) )
                {
                    Edge polyhedron_edge{ polyhedron_edge_vertices };
                    if( !checked_edges.insert( polyhedron_edge ).second )
                    {
                        continue;
                    }
                    if( !polyhedra_around_edge_are_the_same(
                            mesh_.polyhedra_around_edge(
                                polyhedron_edge_vertices, polyhedron_id ),
                            polyhedra_around_edges_.at( polyhedron_edge ) ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Edge between vertices with index ",
                                polyhedron_edge_vertices[0], " and index ",
                                polyhedron_edge_vertices[1],
                                ", is not manifold." );
                        }
                        nb_non_manifold_edges++;
                    }
                }
            }
            return nb_non_manifold_edges;
        }

        std::vector< std::array< index_t, 2 > > non_manifold_edges() const
        {
            std::vector< std::array< index_t, 2 > > non_manifold_edges;
            absl::flat_hash_set< Edge > checked_edges;
            for( const auto polyhedron_id : Range{ mesh_.nb_polyhedra() } )
            {
                for( const auto polyhedron_edge_vertices :
                    mesh_.polyhedron_edges_vertices( polyhedron_id ) )
                {
                    Edge polyhedron_edge{ polyhedron_edge_vertices };
                    if( !checked_edges.insert( polyhedron_edge ).second )
                    {
                        continue;
                    }
                    if( !polyhedra_around_edge_are_the_same(
                            mesh_.polyhedra_around_edge(
                                polyhedron_edge_vertices, polyhedron_id ),
                            polyhedra_around_edges_.at( polyhedron_edge ) ) )
                    {
                        if( verbose_ )
                        {
                            Logger::info( "Edge between vertices with index ",
                                polyhedron_edge_vertices[0], " and index ",
                                polyhedron_edge_vertices[1],
                                ", is not manifold." );
                        }
                        non_manifold_edges.push_back(
                            polyhedron_edge_vertices );
                    }
                }
            }
            return non_manifold_edges;
        }

    private:
        const SolidMesh< dimension >& mesh_;
        DEBUG_CONST bool verbose_;
        DEBUG_CONST
        absl::flat_hash_map< Edge, std::vector< geode::index_t > >
            polyhedra_around_edges_;
    };

    template < index_t dimension >
    SolidMeshEdgeManifold< dimension >::SolidMeshEdgeManifold(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh, false )
    {
    }

    template < index_t dimension >
    SolidMeshEdgeManifold< dimension >::SolidMeshEdgeManifold(
        const SolidMesh< dimension >& mesh, bool verbose )
        : impl_( mesh, verbose )
    {
    }

    template < index_t dimension >
    SolidMeshEdgeManifold< dimension >::~SolidMeshEdgeManifold()
    {
    }

    template < index_t dimension >
    bool SolidMeshEdgeManifold< dimension >::mesh_edges_are_manifold() const
    {
        return impl_->mesh_edges_are_manifold();
    }

    template < index_t dimension >
    index_t SolidMeshEdgeManifold< dimension >::nb_non_manifold_edges() const
    {
        return impl_->nb_non_manifold_edges();
    }

    template < index_t dimension >
    std::vector< std::array< index_t, 2 > >
        SolidMeshEdgeManifold< dimension >::non_manifold_edges() const
    {
        return impl_->non_manifold_edges();
    }

    template class opengeode_inspector_inspector_api SolidMeshEdgeManifold< 3 >;
} // namespace geode
