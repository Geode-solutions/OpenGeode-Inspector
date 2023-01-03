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

#include <geode/inspector/criterion/adjacency/solid_adjacency.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/solid_mesh.h>

namespace geode
{
    template < index_t dimension >
    class SolidMeshAdjacency< dimension >::Impl
    {
    public:
        Impl( const SolidMesh< dimension >& mesh, bool verbose )
            : mesh_( mesh ), verbose_( verbose )
        {
        }

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

        index_t nb_facets_with_wrong_adjacency() const
        {
            index_t nb_wrong_adjacency_facets{ 0 };
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
                        if( verbose_ )
                        {
                            Logger::info( "Local facet ", facet_id,
                                " of polyhedron ", polyhedron_id,
                                " has wrong adjacencies." );
                        }
                        nb_wrong_adjacency_facets++;
                    }
                }
            }
            return nb_wrong_adjacency_facets;
        }

        std::vector< PolyhedronFacet >
            polyhedron_facets_with_wrong_adjacency() const
        {
            std::vector< PolyhedronFacet > wrong_adjacency_facets;
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
                        if( verbose_ )
                        {
                            Logger::info( "Local facet ", facet_id,
                                " of polyhedron ", polyhedron_id,
                                " has wrong adjacencies." );
                        }
                        wrong_adjacency_facets.push_back( polyhedron_facet );
                    }
                }
            }
            return wrong_adjacency_facets;
        }

    private:
        bool mesh_polyhedron_facet_has_right_adjacency(
            const PolyhedronFacet& polyhedron_facet ) const
        {
            const auto adjacent_polyhedron =
                mesh_.polyhedron_adjacent( polyhedron_facet );
            const auto polyhedron_adj_id = adjacent_polyhedron.value();
            const auto facet_vertices =
                detail::OrientedVertexCycle< PolyhedronFacetVertices >(
                    mesh_.polyhedron_facet_vertices( polyhedron_facet ) );
            for( const auto facet_id :
                LRange{ mesh_.nb_polyhedron_facets( polyhedron_adj_id ) } )
            {
                const PolyhedronFacet adj_facet{ polyhedron_adj_id, facet_id };
                const auto adj_facet_vertices =
                    detail::OrientedVertexCycle< PolyhedronFacetVertices >(
                        mesh_.polyhedron_facet_vertices( adj_facet ) );
                if( facet_vertices.is_opposite( adj_facet_vertices )
                    && mesh_.polyhedron_adjacent( adj_facet )
                           == polyhedron_facet.polyhedron_id )
                {
                    return true;
                }
            }
            return false;
        }

    private:
        const SolidMesh< dimension >& mesh_;
        DEBUG_CONST bool verbose_;
    };

    template < index_t dimension >
    SolidMeshAdjacency< dimension >::SolidMeshAdjacency(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh, false )
    {
    }

    template < index_t dimension >
    SolidMeshAdjacency< dimension >::SolidMeshAdjacency(
        const SolidMesh< dimension >& mesh, bool verbose )
        : impl_( mesh, verbose )
    {
    }

    template < index_t dimension >
    SolidMeshAdjacency< dimension >::~SolidMeshAdjacency()
    {
    }

    template < index_t dimension >
    bool SolidMeshAdjacency< dimension >::mesh_has_wrong_adjacencies() const
    {
        return impl_->mesh_has_wrong_adjacencies();
    }

    template < index_t dimension >
    index_t
        SolidMeshAdjacency< dimension >::nb_facets_with_wrong_adjacency() const
    {
        return impl_->nb_facets_with_wrong_adjacency();
    }

    template < index_t dimension >
    std::vector< PolyhedronFacet > SolidMeshAdjacency<
        dimension >::polyhedron_facets_with_wrong_adjacency() const
    {
        return impl_->polyhedron_facets_with_wrong_adjacency();
    }

    template class opengeode_inspector_inspector_api SolidMeshAdjacency< 3 >;
} // namespace geode
