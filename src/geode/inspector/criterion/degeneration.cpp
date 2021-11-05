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

#include <geode/inspector/criterion/degeneration.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/surface_edges.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>

namespace geode
{
    namespace inspector
    {
        template < index_t dimension >
        class SurfaceMeshDegeneration< dimension >::Impl
        {
        public:
            Impl() = default;

            bool is_mesh_degenerated(
                const SurfaceMesh< dimension >& mesh ) const
            {
                bool is_degenerated = false;

                mesh.enable_edges();
                for( auto edge_index : Range( mesh.edges().nb_edges() ) )
                {
                    const auto edge_vertices =
                        mesh.edges().edge_vertices( edge_index );
                    const Point< dimension >&p1 =
                                               mesh.point( edge_vertices[0] ),
                                           p2 = mesh.point( edge_vertices[1] );
                    if( geode::point_point_distance( p1, p2 ) < global_epsilon )
                    {
                        is_degenerated = true;
                        break;
                    }
                }

                return is_degenerated;
            }

            index_t nb_degenerated_edges(
                const SurfaceMesh< dimension >& mesh ) const
            {
                index_t nb_degeneration = 0;

                mesh.enable_edges();
                for( auto edge_index : Range( mesh.edges().nb_edges() ) )
                {
                    const auto edge_vertices =
                        mesh.edges().edge_vertices( edge_index );
                    const Point< dimension >&p1 =
                                               mesh.point( edge_vertices[0] ),
                                           p2 = mesh.point( edge_vertices[1] );
                    if( geode::point_point_distance( p1, p2 ) < global_epsilon )
                    {
                        nb_degeneration++;
                    }
                }

                return nb_degeneration;
            }

            std::vector< index_t > degenerated_edges(
                const SurfaceMesh< dimension >& mesh ) const
            {
                std::vector< index_t > degenerated_edge_points;

                mesh.enable_edges();
                for( auto edge_index : Range( mesh.edges().nb_edges() ) )
                {
                    const auto edge_vertices =
                        mesh.edges().edge_vertices( edge_index );
                    const Point< dimension >&p1 =
                                               mesh.point( edge_vertices[0] ),
                                           p2 = mesh.point( edge_vertices[1] );
                    if( geode::point_point_distance( p1, p2 ) < global_epsilon )
                    {
                        degenerated_edge_points.push_back( edge_index );
                    }
                }

                return degenerated_edge_points;
            }
        };

        template < index_t dimension >
        SurfaceMeshDegeneration< dimension >::SurfaceMeshDegeneration()
        {
        }

        template < index_t dimension >
        SurfaceMeshDegeneration< dimension >::~SurfaceMeshDegeneration()
        {
        }

        template < index_t dimension >
        bool SurfaceMeshDegeneration< dimension >::is_mesh_degenerated(
            const SurfaceMesh< dimension >& mesh ) const
        {
            return impl_->is_mesh_degenerated( mesh );
        }

        template < index_t dimension >
        index_t SurfaceMeshDegeneration< dimension >::nb_degenerated_edges(
            const SurfaceMesh< dimension >& mesh ) const
        {
            return impl_->nb_degenerated_edges( mesh );
        }

        template < index_t dimension >
        const std::vector< index_t >
            SurfaceMeshDegeneration< dimension >::degenerated_edges(
                const SurfaceMesh< dimension >& mesh ) const
        {
            return impl_->degenerated_edges( mesh );
        }

        template class opengeode_inspector_inspector_api
            SurfaceMeshDegeneration< 2 >;
        template class opengeode_inspector_inspector_api
            SurfaceMeshDegeneration< 3 >;
    } // namespace inspector
} // namespace geode
