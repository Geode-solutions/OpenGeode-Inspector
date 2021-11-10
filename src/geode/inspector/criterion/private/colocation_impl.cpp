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

#include <geode/inspector/criterion/private/colocation_impl.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/geometry/point.h>

namespace geode
{
    namespace detail
    {
        template < index_t dimension, class MeshType >
        const typename NNSearch< dimension >::ColocatedInfo
            mesh_points_colocated_info(
                const MeshType& mesh, double colocation_distance )
        {
            std::vector< Point< dimension > > mesh_points{ mesh.nb_vertices() };
            for( const auto point_index : Range{ mesh.nb_vertices() } )
            {
                mesh_points[point_index] = mesh.point( point_index );
            }
            const geode::NNSearch< dimension > nnsearch{ mesh_points };
            return nnsearch.colocated_index_mapping( colocation_distance );
        }

        template < index_t dimension, class MeshType >
        ColocationImpl< dimension, MeshType >::ColocationImpl(
            const MeshType& mesh )
            : mesh_colocation_info_{
                  mesh_points_colocated_info< dimension, MeshType >(
                      mesh, global_epsilon )
              }
        {
        }

        template < index_t dimension, class MeshType >
        bool ColocationImpl< dimension, MeshType >::mesh_has_colocated_points()
            const
        {
            return mesh_colocation_info_.nb_colocated_points() > 0;
        }

        template < index_t dimension, class MeshType >
        index_t
            ColocationImpl< dimension, MeshType >::nb_colocated_points() const
        {
            return mesh_colocation_info_.nb_colocated_points();
        }

        template < index_t dimension, class MeshType >
        std::vector< std::vector< index_t > >
            ColocationImpl< dimension, MeshType >::colocated_points_groups()
                const
        {
            std::vector< std::vector< index_t > > colocated_points_indices;
            std::vector< index_t > points_already_checked{
                mesh_colocation_info_.nb_colocated_points() - 1
            };

            for( const auto point_index_1 :
                Range{ mesh_colocation_info_.colocated_mapping.size() - 1 } )
            {
                std::vector< index_t > colocated_points;
                for( const auto point_index_2 : Range{ point_index_1 + 1,
                         mesh_colocation_info_.colocated_mapping.size() } )
                {
                    if( absl::c_find( points_already_checked, point_index_2 )
                        != points_already_checked.end() )
                    {
                        continue;
                    }
                    if( mesh_colocation_info_.colocated_mapping[point_index_1]
                        == mesh_colocation_info_
                               .colocated_mapping[point_index_2] )
                    {
                        colocated_points.push_back( point_index_2 );
                        points_already_checked.push_back( point_index_2 );
                    }
                }
                if( colocated_points.size() > 0 )
                {
                    colocated_points.push_back( point_index_1 );
                    colocated_points_indices.push_back( colocated_points );
                }
            }

            return colocated_points_indices;
        }

        template class opengeode_inspector_inspector_api
            ColocationImpl< 2, PointSet2D >;
        template class opengeode_inspector_inspector_api
            ColocationImpl< 3, PointSet3D >;
        template class opengeode_inspector_inspector_api
            ColocationImpl< 2, EdgedCurve2D >;
        template class opengeode_inspector_inspector_api
            ColocationImpl< 3, EdgedCurve3D >;
        template class opengeode_inspector_inspector_api
            ColocationImpl< 2, SurfaceMesh2D >;
        template class opengeode_inspector_inspector_api
            ColocationImpl< 3, SurfaceMesh3D >;
        template class opengeode_inspector_inspector_api
            ColocationImpl< 3, SolidMesh3D >;
    } // namespace detail
} // namespace geode
