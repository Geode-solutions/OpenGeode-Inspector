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

#include <geode/inspector/criterion/colocalization.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/vertex_set.h>

#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>

namespace geode
{
    namespace inspector
    {
        template < index_t dimension >
        class MeshColocalization< dimension >::Impl
        {
        public:
            Impl() = default;

            bool mesh_has_colocalized_points(
                const VertexSet< dimension >& mesh ) const
            {
                bool has_colocalization = false;

                for( auto point_index : Range( mesh.nb_vertices() ) ) {}
                // mesh.enable_edges();
                // for( auto edge_index : Range( mesh.edges().nb_edges() ) )
                // {
                //     const auto edge_vertices =
                //         mesh.edges().edge_vertices( edge_index );
                //     const Point< dimension >&p1 =
                //                                mesh.point( edge_vertices[0]
                //                                ),
                //                            p2 = mesh.point( edge_vertices[1]
                //                            );
                //     if( geode::point_point_distance( p1, p2 ) <
                //     global_epsilon )
                //     {
                //         is_degenerated = true;
                //         break;
                //     }
                // }

                return has_colocalization;
            }

            index_t nb_colocalized_points(
                const VertexSet< dimension >& mesh ) const
            {
                index_t nb_colocalizations;

                return nb_colocalizations
            }

            std::vector< std::Vector< index_t > > colocalized_points_groups(
                const VertexSet< dimension >& mesh ) const
            {
                std::vector< std::Vector< index_t > > colocalizations;
                return colocalizations;
            }
        };

        template < index_t dimension >
        MeshColocalization< dimension >::MeshColocalization()
        {
        }

        template < index_t dimension >
        MeshColocalization< dimension >::~MeshColocalization()
        {
        }

        template < index_t dimension >
        bool MeshColocalization< dimension >::mesh_has_colocalized_points(
            const VertexSet< dimension >& mesh ) const
        {
            return impl_->mesh_has_colocalized_points( mesh );
        }

        template < index_t dimension >
        index_t MeshColocalization< dimension >::nb_colocalized_points(
            const VertexSet< dimension >& mesh ) const
        {
            return impl_->nb_colocalized_points( mesh );
        }

        template < index_t dimension >
        const std::vector< std::vector< index_t > >
            MeshColocalization< dimension >::colocalized_points_groups(
                const VertexSet< dimension >& mesh ) const
        {
            return impl_->colocalized_points_groups( mesh );
        }

        template class opengeode_inspector_inspector_api
            MeshColocalization< 2 >;
        template class opengeode_inspector_inspector_api
            MeshColocalization< 3 >;
    } // namespace inspector
} // namespace geode
