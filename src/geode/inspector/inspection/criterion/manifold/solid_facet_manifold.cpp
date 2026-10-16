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

#include <geode/inspector/inspection/criterion/manifold/solid_facet_manifold.hpp>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_map.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/detail/vertex_cycle.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace
{
    using Facet = geode::detail::VertexCycle< geode::PolyhedronFacetVertices >;

    struct FacetCount
    {
        geode::index_t first_occurrence;
        geode::index_t nb_polyhedra;
    };

    // Facets shared by more than two polyhedra, in order of first appearance
    template < geode::index_t dimension >
    [[nodiscard]] std::vector< Facet > non_manifold_facets_list(
        const geode::SolidMesh< dimension >& mesh )
    {
        absl::flat_hash_map< Facet, FacetCount > facet_counts;
        facet_counts.reserve( 3 * mesh.nb_polyhedra() );
        geode::index_t occurrence{ 0 };
        for( const auto polyhedron_id : geode::Range{ mesh.nb_polyhedra() } )
        {
            for( const auto facet_id :
                geode::LRange{ mesh.nb_polyhedron_facets( polyhedron_id ) } )
            {
                auto [it, inserted] = facet_counts.try_emplace(
                    Facet{ mesh.polyhedron_facet_vertices(
                        { polyhedron_id, facet_id } ) },
                    FacetCount{ occurrence, 1 } );
                if( !inserted )
                {
                    it->second.nb_polyhedra++;
                }
                occurrence++;
            }
        }
        std::vector< std::pair< geode::index_t, const Facet* > > non_manifold;
        for( const auto& [facet, count] : facet_counts )
        {
            if( count.nb_polyhedra > 2 )
            {
                non_manifold.emplace_back( count.first_occurrence, &facet );
            }
        }
        absl::c_sort( non_manifold );
        std::vector< Facet > result;
        result.reserve( non_manifold.size() );
        for( const auto& facet : non_manifold )
        {
            result.push_back( *facet.second );
        }
        return result;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SolidMeshFacetManifold< dimension >::Impl
    {
    public:
        explicit Impl( const SolidMesh< dimension >& mesh ) : mesh_( mesh ) {}

        [[nodiscard]] bool mesh_facets_are_manifold() const
        {
            return non_manifold_facets_list( mesh_ ).empty();
        }

        [[nodiscard]] InspectionIssues< PolyhedronFacetVertices >
            non_manifold_facets() const
        {
            InspectionIssues< PolyhedronFacetVertices > non_manifold_facets{
                "non manifold facets"
            };
            for( const auto& facet : non_manifold_facets_list( mesh_ ) )
            {
                std::string message{ "facet made of vertices with index " };
                for( const auto vertex_id : facet.vertices() )
                {
                    absl::StrAppend( &message, vertex_id, ", " );
                }
                absl::StrAppend( &message, "is not manifold." );
                non_manifold_facets.add_issue( facet.vertices(), message );
            }
            return non_manifold_facets;
        }

    private:
        const SolidMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    SolidMeshFacetManifold< dimension >::SolidMeshFacetManifold(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshFacetManifold< dimension >::~SolidMeshFacetManifold() = default;

    template < index_t dimension >
    bool SolidMeshFacetManifold< dimension >::mesh_facets_are_manifold() const
    {
        return impl_->mesh_facets_are_manifold();
    }

    template < index_t dimension >
    InspectionIssues< PolyhedronFacetVertices >
        SolidMeshFacetManifold< dimension >::non_manifold_facets() const
    {
        return impl_->non_manifold_facets();
    }

    template class opengeode_inspector_inspection_api
        SolidMeshFacetManifold< 3 >;
} // namespace geode
