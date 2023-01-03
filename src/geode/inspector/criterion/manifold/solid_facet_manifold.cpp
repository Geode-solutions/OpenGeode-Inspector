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

#include <geode/inspector/criterion/manifold/solid_facet_manifold.h>

#include <absl/container/flat_hash_map.h>
#include <absl/container/flat_hash_set.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/solid_mesh.h>

namespace
{
    using Facet = geode::detail::VertexCycle< geode::PolyhedronFacetVertices >;

    template < geode::index_t dimension >
    absl::flat_hash_map< Facet, geode::index_t >
        facets_to_nb_adjacent_polyhedra(
            const geode::SolidMesh< dimension >& mesh )
    {
        absl::flat_hash_map< Facet, geode::index_t >
            nb_polyhedra_adjacent_to_facets;
        for( const auto polyhedron_id : geode::Range{ mesh.nb_polyhedra() } )
        {
            for( const auto facet_id :
                geode::LRange{ mesh.nb_polyhedron_facets( polyhedron_id ) } )
            {
                const Facet facet{ mesh.polyhedron_facet_vertices(
                    { polyhedron_id, facet_id } ) };
                if( !nb_polyhedra_adjacent_to_facets.try_emplace( facet, 1 )
                         .second )
                {
                    nb_polyhedra_adjacent_to_facets[facet] += 1;
                }
            }
        }
        return nb_polyhedra_adjacent_to_facets;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SolidMeshFacetManifold< dimension >::Impl
    {
    public:
        Impl( const SolidMesh< dimension >& mesh, bool verbose )
            : mesh_( mesh ), verbose_( verbose )
        {
        }

        bool mesh_facets_are_manifold() const
        {
            const auto nb_polyhedra_adjacent_to_facets =
                facets_to_nb_adjacent_polyhedra( mesh_ );
            for( const auto& nb_adjacent_polyhedra :
                nb_polyhedra_adjacent_to_facets )
            {
                if( nb_adjacent_polyhedra.second > 2 )
                {
                    return false;
                }
            }
            return true;
        }

        index_t nb_non_manifold_facets() const
        {
            const auto nb_polyhedra_adjacent_to_facets =
                facets_to_nb_adjacent_polyhedra( mesh_ );
            index_t nb_non_manifold_facets{ 0 };
            for( const auto& nb_adjacent_polyhedra :
                nb_polyhedra_adjacent_to_facets )
            {
                if( nb_adjacent_polyhedra.second > 2 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Facet made of vertices with index",
                            nb_adjacent_polyhedra.first.vertices()[0], ", ",
                            nb_adjacent_polyhedra.first.vertices()[1], ", ",
                            nb_adjacent_polyhedra.first.vertices()[2],
                            ", is not manifold." );
                    }
                    nb_non_manifold_facets++;
                }
            }
            return nb_non_manifold_facets;
        }

        std::vector< PolyhedronFacetVertices > non_manifold_facets() const
        {
            const auto nb_polyhedra_adjacent_to_facets =
                facets_to_nb_adjacent_polyhedra( mesh_ );
            std::vector< PolyhedronFacetVertices > non_manifold_facets;
            for( const auto& nb_adjacent_polyhedra :
                nb_polyhedra_adjacent_to_facets )
            {
                if( nb_adjacent_polyhedra.second > 2 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Facet made of vertices with index",
                            nb_adjacent_polyhedra.first.vertices()[0], ", ",
                            nb_adjacent_polyhedra.first.vertices()[1], ", ",
                            nb_adjacent_polyhedra.first.vertices()[2],
                            ", is not manifold." );
                    }
                    non_manifold_facets.push_back(
                        nb_adjacent_polyhedra.first.vertices() );
                }
            }
            return non_manifold_facets;
        }

    private:
        const SolidMesh< dimension >& mesh_;
        DEBUG_CONST bool verbose_;
    };

    template < index_t dimension >
    SolidMeshFacetManifold< dimension >::SolidMeshFacetManifold(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh, false )
    {
    }

    template < index_t dimension >
    SolidMeshFacetManifold< dimension >::SolidMeshFacetManifold(
        const SolidMesh< dimension >& mesh, bool verbose )
        : impl_( mesh, verbose )
    {
    }

    template < index_t dimension >
    SolidMeshFacetManifold< dimension >::~SolidMeshFacetManifold()
    {
    }

    template < index_t dimension >
    bool SolidMeshFacetManifold< dimension >::mesh_facets_are_manifold() const
    {
        return impl_->mesh_facets_are_manifold();
    }

    template < index_t dimension >
    index_t SolidMeshFacetManifold< dimension >::nb_non_manifold_facets() const
    {
        return impl_->nb_non_manifold_facets();
    }

    template < index_t dimension >
    std::vector< PolyhedronFacetVertices >
        SolidMeshFacetManifold< dimension >::non_manifold_facets() const
    {
        return impl_->non_manifold_facets();
    }

    template class opengeode_inspector_inspector_api
        SolidMeshFacetManifold< 3 >;
} // namespace geode
