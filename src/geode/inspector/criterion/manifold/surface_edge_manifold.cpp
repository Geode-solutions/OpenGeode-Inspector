/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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

#include <geode/inspector/criterion/manifold/surface_edge_manifold.h>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/surface_mesh.h>

namespace
{
    template < geode::index_t dimension >
    std::vector< geode::detail::VertexCycle< std::array< geode::index_t, 2 > > >
        mesh_non_manifold_edges( const geode::SurfaceMesh< dimension >& mesh )
    {
        std::vector<
            geode::detail::VertexCycle< std::array< geode::index_t, 2 > > >
            non_manifold_edges;
        absl::flat_hash_map<
            geode::detail::VertexCycle< std::array< geode::index_t, 2 > >,
            geode::local_index_t >
            polygons_around_edges;
        for( const auto polygon_id : geode::Range{ mesh.nb_polygons() } )
        {
            for( const auto polygon_edge_id :
                geode::LRange{ mesh.nb_polygon_edges( polygon_id ) } )
            {
                const geode::detail::VertexCycle<
                    std::array< geode::index_t, 2 > >
                    polygon_edge_vertex_cycle{ mesh.polygon_edge_vertices(
                        { { polygon_id, polygon_edge_id } } ) };
                if( !polygons_around_edges
                         .try_emplace( polygon_edge_vertex_cycle, 1 )
                         .second )
                {
                    polygons_around_edges[polygon_edge_vertex_cycle] += 1;
                }
            }
        }
        for( const auto& edge : polygons_around_edges )
        {
            if( edge.second > 2 )
            {
                non_manifold_edges.push_back( edge.first );
            }
        }
        return non_manifold_edges;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SurfaceMeshEdgeManifold< dimension >::Impl
    {
    public:
        Impl( const SurfaceMesh< dimension >& mesh )
            : non_manifold_edges_{ mesh_non_manifold_edges< dimension >(
                mesh ) }
        {
        }

        bool mesh_edges_are_manifold() const
        {
            return non_manifold_edges_.size() == 0;
        }

        index_t nb_non_manifold_edges() const
        {
            return non_manifold_edges_.size();
        }

        const std::vector<
            geode::detail::VertexCycle< std::array< geode::index_t, 2 > > >&
            non_manifold_edges() const
        {
            return non_manifold_edges_;
        }

    private:
        const std::vector<
            geode::detail::VertexCycle< std::array< geode::index_t, 2 > > >
            non_manifold_edges_;
    };

    template < index_t dimension >
    SurfaceMeshEdgeManifold< dimension >::SurfaceMeshEdgeManifold(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SurfaceMeshEdgeManifold< dimension >::~SurfaceMeshEdgeManifold()
    {
    }

    template < index_t dimension >
    bool SurfaceMeshEdgeManifold< dimension >::mesh_edges_are_manifold() const
    {
        return impl_->mesh_edges_are_manifold();
    }

    template < index_t dimension >
    index_t SurfaceMeshEdgeManifold< dimension >::nb_non_manifold_edges() const
    {
        return impl_->nb_non_manifold_edges();
    }

    template < index_t dimension >
    const std::vector<
        geode::detail::VertexCycle< std::array< geode::index_t, 2 > > >&
        SurfaceMeshEdgeManifold< dimension >::non_manifold_edges() const
    {
        return impl_->non_manifold_edges();
    }

    template class opengeode_inspector_inspector_api
        SurfaceMeshEdgeManifold< 2 >;
    template class opengeode_inspector_inspector_api
        SurfaceMeshEdgeManifold< 3 >;
} // namespace geode
