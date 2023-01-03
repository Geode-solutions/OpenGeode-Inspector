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

#include <geode/inspector/criterion/manifold/surface_vertex_manifold.h>

#include <absl/algorithm/container.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/surface_mesh.h>

namespace
{
    bool polygons_around_vertex_are_the_same(
        const geode::PolygonsAroundVertex& pav1,
        const geode::PolygonsAroundVertex& pav2 )
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
} // namespace

namespace geode
{
    template < index_t dimension >
    class SurfaceMeshVertexManifold< dimension >::Impl
    {
    public:
        Impl( const SurfaceMesh< dimension >& mesh, bool verbose )
            : mesh_( mesh ), verbose_( verbose )
        {
        }

        bool mesh_vertices_are_manifold() const
        {
            const auto polygons_around_vertices_list =
                polygons_around_vertices( mesh_ );
            for( const auto vertex_id : geode::Range{ mesh_.nb_vertices() } )
            {
                if( !polygons_around_vertex_are_the_same(
                        polygons_around_vertices_list[vertex_id],
                        mesh_.polygons_around_vertex( vertex_id ) ) )
                {
                    return false;
                }
            }
            return true;
        }

        index_t nb_non_manifold_vertices() const
        {
            const auto polygons_around_vertices_list =
                polygons_around_vertices( mesh_ );
            index_t nb_non_manifold_vertices{ 0 };
            for( const auto vertex_id : geode::Range{ mesh_.nb_vertices() } )
            {
                if( !polygons_around_vertex_are_the_same(
                        polygons_around_vertices_list[vertex_id],
                        mesh_.polygons_around_vertex( vertex_id ) ) )
                {
                    if( verbose_ )
                    {
                        geode::Logger::info( "Vertex with index ", vertex_id,
                            ", at position ", mesh_.point( vertex_id ).string(),
                            ", is not manifold." );
                    }
                    nb_non_manifold_vertices++;
                }
            }
            return nb_non_manifold_vertices;
        }

        std::vector< index_t > non_manifold_vertices() const
        {
            const auto polygons_around_vertices_list =
                polygons_around_vertices( mesh_ );
            std::vector< geode::index_t > non_manifold_vertices;
            for( const auto vertex_id : geode::Range{ mesh_.nb_vertices() } )
            {
                if( !polygons_around_vertex_are_the_same(
                        polygons_around_vertices_list[vertex_id],
                        mesh_.polygons_around_vertex( vertex_id ) ) )
                {
                    if( verbose_ )
                    {
                        geode::Logger::info( "Vertex with index ", vertex_id,
                            ", at position ", mesh_.point( vertex_id ).string(),
                            ", is not manifold." );
                    }
                    non_manifold_vertices.push_back( vertex_id );
                }
            }
            return non_manifold_vertices;
        }

    private:
        const SurfaceMesh< dimension >& mesh_;
        DEBUG_CONST bool verbose_;
    };

    template < index_t dimension >
    SurfaceMeshVertexManifold< dimension >::SurfaceMeshVertexManifold(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh, false )
    {
    }

    template < index_t dimension >
    SurfaceMeshVertexManifold< dimension >::SurfaceMeshVertexManifold(
        const SurfaceMesh< dimension >& mesh, bool verbose )
        : impl_( mesh, verbose )
    {
    }

    template < index_t dimension >
    SurfaceMeshVertexManifold< dimension >::~SurfaceMeshVertexManifold()
    {
    }

    template < index_t dimension >
    bool SurfaceMeshVertexManifold< dimension >::mesh_vertices_are_manifold()
        const
    {
        return impl_->mesh_vertices_are_manifold();
    }

    template < index_t dimension >
    index_t
        SurfaceMeshVertexManifold< dimension >::nb_non_manifold_vertices() const
    {
        return impl_->nb_non_manifold_vertices();
    }

    template < index_t dimension >
    std::vector< index_t >
        SurfaceMeshVertexManifold< dimension >::non_manifold_vertices() const
    {
        return impl_->non_manifold_vertices();
    }

    template class opengeode_inspector_inspector_api
        SurfaceMeshVertexManifold< 2 >;
    template class opengeode_inspector_inspector_api
        SurfaceMeshVertexManifold< 3 >;
} // namespace geode
