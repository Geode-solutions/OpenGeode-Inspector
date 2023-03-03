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

#include <geode/inspector/criterion/manifold/surface_edge_manifold.h>

#include <absl/container/flat_hash_map.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/surface_mesh.h>

namespace
{
    using Edge = geode::detail::VertexCycle< std::array< geode::index_t, 2 > >;

    template < geode::index_t dimension >
    absl::flat_hash_map< Edge, geode::local_index_t > edge_to_polygons_around(
        const geode::SurfaceMesh< dimension >& mesh )
    {
        absl::flat_hash_map< Edge, geode::local_index_t > polygons_around_edges;
        for( const auto polygon_id : geode::Range{ mesh.nb_polygons() } )
        {
            for( const auto polygon_edge_id :
                geode::LRange{ mesh.nb_polygon_edges( polygon_id ) } )
            {
                const Edge polygon_edge_vertex_cycle{
                    mesh.polygon_edge_vertices(
                        { polygon_id, polygon_edge_id } )
                };
                if( !polygons_around_edges
                         .try_emplace( polygon_edge_vertex_cycle, 1 )
                         .second )
                {
                    polygons_around_edges[polygon_edge_vertex_cycle] += 1;
                }
            }
        }
        return polygons_around_edges;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SurfaceMeshEdgeManifold< dimension >::Impl
    {
    public:
        Impl( const SurfaceMesh< dimension >& mesh, bool verbose )
            : mesh_( mesh ), verbose_( verbose )
        {
        }

        bool mesh_edges_are_manifold() const
        {
            for( const auto& edge : edge_to_polygons_around( mesh_ ) )
            {
                if( edge.second > 2 )
                {
                    return false;
                }
            }
            return true;
        }

        index_t nb_non_manifold_edges() const
        {
            index_t nb_non_manifold_edges{ 0 };
            for( const auto& edge : edge_to_polygons_around( mesh_ ) )
            {
                if( edge.second > 2 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Edge between vertices with index ",
                            edge.first.vertices()[0], " and index ",
                            edge.first.vertices()[1], ", is not manifold." );
                    }
                    nb_non_manifold_edges++;
                }
            }
            return nb_non_manifold_edges;
        }

        std::vector< std::array< index_t, 2 > > non_manifold_edges() const
        {
            std::vector< std::array< index_t, 2 > > non_manifold_edges;
            for( const auto& edge : edge_to_polygons_around( mesh_ ) )
            {
                if( edge.second > 2 )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Edge between vertices with index ",
                            edge.first.vertices()[0], " and index ",
                            edge.first.vertices()[1], ", is not manifold." );
                    }
                    non_manifold_edges.push_back( edge.first.vertices() );
                }
            }
            return non_manifold_edges;
        }

    private:
        const SurfaceMesh< dimension >& mesh_;
        DEBUG_CONST bool verbose_;
    };

    template < index_t dimension >
    SurfaceMeshEdgeManifold< dimension >::SurfaceMeshEdgeManifold(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh, false )
    {
    }

    template < index_t dimension >
    SurfaceMeshEdgeManifold< dimension >::SurfaceMeshEdgeManifold(
        const SurfaceMesh< dimension >& mesh, bool verbose )
        : impl_( mesh, verbose )
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
    std::vector< std::array< index_t, 2 > >
        SurfaceMeshEdgeManifold< dimension >::non_manifold_edges() const
    {
        return impl_->non_manifold_edges();
    }

    template class opengeode_inspector_inspector_api
        SurfaceMeshEdgeManifold< 2 >;
    template class opengeode_inspector_inspector_api
        SurfaceMeshEdgeManifold< 3 >;
} // namespace geode
