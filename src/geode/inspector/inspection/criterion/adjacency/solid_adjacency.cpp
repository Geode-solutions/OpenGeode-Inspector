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

#include <geode/inspector/inspection/criterion/adjacency/solid_adjacency.hpp>

#include <absl/container/inlined_vector.h>

#include <async++.h>

#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/detail/vertex_cycle.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    template < index_t dimension >
    class SolidMeshAdjacency< dimension >::Impl
    {
    public:
        Impl( const SolidMesh< dimension >& mesh ) : mesh_( mesh ) {}

        bool mesh_has_wrong_adjacencies() const
        {
            for( const auto polyhedron_id : Range{ mesh_.nb_polyhedra() } )
            {
                for( const auto facet_id :
                    LRange{ mesh_.nb_polyhedron_facets( polyhedron_id ) } )
                {
                    const PolyhedronFacet polyhedron_facet{ polyhedron_id,
                        facet_id };
                    if( !mesh_.is_polyhedron_facet_on_border( polyhedron_facet )
                        && !mesh_polyhedron_facet_has_right_adjacency(
                            polyhedron_facet ) )
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        InspectionIssues< PolyhedronFacet >
            polyhedron_facets_with_wrong_adjacency() const
        {
            InspectionIssues< PolyhedronFacet > wrong_adjacency_facets{
                absl::StrCat( "Solid ",
                    mesh_.name().value_or( mesh_.id().string() ),
                    " polyhedron facets adjacencies issues" )
            };
            const auto wrong_facets = facets_with_wrong_adjacency();
            for( const auto polyhedron_id : Range{ mesh_.nb_polyhedra() } )
            {
                for( const auto facet_id : wrong_facets[polyhedron_id] )
                {
                    wrong_adjacency_facets.add_issue(
                        PolyhedronFacet{ polyhedron_id, facet_id },
                        absl::StrCat( "facet ", facet_id, " of polyhedron ",
                            polyhedron_id, " has wrong adjacencies" ) );
                }
            }
            return wrong_adjacency_facets;
        }

    private:
        // Facets with wrong adjacency of each polyhedron
        std::vector< absl::InlinedVector< local_index_t, 1 > >
            facets_with_wrong_adjacency() const
        {
            std::vector< absl::InlinedVector< local_index_t, 1 > > wrong_facets(
                mesh_.nb_polyhedra() );
            async::parallel_for(
                async::irange( index_t{ 0 }, mesh_.nb_polyhedra() ),
                [this, &wrong_facets]( index_t polyhedron_id ) {
                    for( const auto facet_id :
                        LRange{ mesh_.nb_polyhedron_facets( polyhedron_id ) } )
                    {
                        const PolyhedronFacet polyhedron_facet{ polyhedron_id,
                            facet_id };
                        if( !mesh_.is_polyhedron_facet_on_border(
                                polyhedron_facet )
                            && !mesh_polyhedron_facet_has_right_adjacency(
                                polyhedron_facet ) )
                        {
                            wrong_facets[polyhedron_id].push_back( facet_id );
                        }
                    }
                } );
            return wrong_facets;
        }

        bool mesh_polyhedron_facet_has_right_adjacency(
            const PolyhedronFacet& polyhedron_facet ) const
        {
            const auto adjacent_polyhedron =
                mesh_.polyhedron_adjacent( polyhedron_facet );
            const auto polyhedron_adj_id = adjacent_polyhedron.value();
            if( polyhedron_adj_id >= mesh_.nb_polyhedra() )
            {
                return false;
            }
            const auto nb_facet_vertices =
                mesh_.nb_polyhedron_facet_vertices( polyhedron_facet );
            std::optional<
                detail::OrientedVertexCycle< PolyhedronFacetVertices > >
                facet_vertices;
            for( const auto facet_id :
                LRange{ mesh_.nb_polyhedron_facets( polyhedron_adj_id ) } )
            {
                const PolyhedronFacet adj_facet{ polyhedron_adj_id, facet_id };
                // Cheap checks before comparing the vertex cycles
                if( mesh_.polyhedron_adjacent( adj_facet )
                        != polyhedron_facet.polyhedron_id
                    || mesh_.nb_polyhedron_facet_vertices( adj_facet )
                           != nb_facet_vertices )
                {
                    continue;
                }
                if( !facet_vertices )
                {
                    facet_vertices.emplace(
                        mesh_.polyhedron_facet_vertices( polyhedron_facet ) );
                }
                const detail::OrientedVertexCycle< PolyhedronFacetVertices >
                    adj_facet_vertices{ mesh_.polyhedron_facet_vertices(
                        adj_facet ) };
                if( facet_vertices->is_opposite( adj_facet_vertices ) )
                {
                    return true;
                }
            }
            return false;
        }

    private:
        const SolidMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    SolidMeshAdjacency< dimension >::SolidMeshAdjacency(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshAdjacency< dimension >::~SolidMeshAdjacency() = default;

    template < index_t dimension >
    bool SolidMeshAdjacency< dimension >::mesh_has_wrong_adjacencies() const
    {
        return impl_->mesh_has_wrong_adjacencies();
    }

    template < index_t dimension >
    InspectionIssues< PolyhedronFacet > SolidMeshAdjacency<
        dimension >::polyhedron_facets_with_wrong_adjacency() const
    {
        return impl_->polyhedron_facets_with_wrong_adjacency();
    }

    template class opengeode_inspector_inspection_api SolidMeshAdjacency< 3 >;
} // namespace geode
