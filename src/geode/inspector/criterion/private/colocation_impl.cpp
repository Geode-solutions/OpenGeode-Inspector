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

#include <geode/inspector/criterion/private/colocation_impl.h>

#include <geode/basic/logger.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/geometry/point.h>

namespace
{
    template < geode::index_t dimension, typename Mesh >
    typename geode::NNSearch< dimension >::ColocatedInfo
        mesh_points_colocated_info(
            const Mesh& mesh, double colocation_distance )
    {
        std::vector< geode::Point< dimension > > mesh_points(
            mesh.nb_vertices() );
        for( const auto point_index : geode::Range{ mesh.nb_vertices() } )
        {
            mesh_points[point_index] = mesh.point( point_index );
        }
        const geode::NNSearch< dimension > nnsearch{ mesh_points };
        return nnsearch.colocated_index_mapping( colocation_distance );
    }

    template < geode::index_t dimension, typename Mesh >
    std::vector< std::vector< geode::index_t > > colocated_points_groups(
        const Mesh& mesh, double colocation_distance, bool verbose )
    {
        const auto mesh_colocation_info =
            mesh_points_colocated_info< dimension, Mesh >(
                mesh, colocation_distance );
        std::vector< std::vector< geode::index_t > > colocated_points_indices(
            mesh_colocation_info.nb_unique_points() );
        for( const auto point_index :
            geode::Indices{ mesh_colocation_info.colocated_mapping } )
        {
            colocated_points_indices[mesh_colocation_info
                                         .colocated_mapping[point_index]]
                .push_back( point_index );
        }
        const auto colocated_points_groups_end = std::remove_if(
            colocated_points_indices.begin(), colocated_points_indices.end(),
            []( const std::vector< geode::index_t >& colocated_points_group ) {
                return colocated_points_group.size() < 2;
            } );
        colocated_points_indices.erase(
            colocated_points_groups_end, colocated_points_indices.end() );
        if( verbose )
        {
            for( const auto& colocated_points_group : colocated_points_indices )
            {
                std::string point_group_string{ "" };
                for( const auto point_index : colocated_points_group )
                {
                    absl::StrAppend( &point_group_string, " ", point_index );
                }
                geode::Logger::info( "Vertices with indices",
                    point_group_string, " are colocated at position [",
                    mesh_colocation_info
                        .unique_points[mesh_colocation_info.colocated_mapping
                                           [colocated_points_group[0]]]
                        .string(),
                    "]." );
            }
        }
        return colocated_points_indices;
    }
} // namespace

namespace geode
{
    namespace detail
    {
        template < index_t dimension, typename Mesh >
        ColocationImpl< dimension, Mesh >::ColocationImpl(
            const Mesh& mesh, bool verbose )
            : mesh_( mesh ), verbose_( verbose )
        {
        }

        template < index_t dimension, typename Mesh >
        bool
            ColocationImpl< dimension, Mesh >::mesh_has_colocated_points() const
        {
            return mesh_points_colocated_info< dimension, Mesh >(
                       mesh_, global_epsilon )
                       .nb_colocated_points()
                   > 0;
        }

        template < index_t dimension, typename Mesh >
        index_t ColocationImpl< dimension, Mesh >::nb_colocated_points() const
        {
            const auto colocated_points_indices =
                ::colocated_points_groups< dimension, Mesh >(
                    mesh_, global_epsilon, verbose_ );
            index_t nb_colocated{ 0 };
            for( const auto& point_group : colocated_points_indices )
            {
                nb_colocated += point_group.size();
            }
            if( nb_colocated > 0 && verbose_ )
            {
                Logger::info( "Mesh has ", nb_colocated, " colocated points" );
            }
            return nb_colocated;
        }

        template < index_t dimension, typename Mesh >
        std::vector< std::vector< index_t > >
            ColocationImpl< dimension, Mesh >::colocated_points_groups() const
        {
            const auto colocated_points_indices =
                ::colocated_points_groups< dimension, Mesh >(
                    mesh_, global_epsilon, verbose_ );
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
