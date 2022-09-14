/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/inspector/criterion/colocation/component_meshes_colocation.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/colocation/edgedcurve_colocation.h>
#include <geode/inspector/criterion/colocation/solid_colocation.h>
#include <geode/inspector/criterion/colocation/surface_colocation.h>

namespace
{
    template < geode::index_t dimension, typename Model >
    std::vector< std::vector< geode::index_t > >
        filter_colocated_points_with_same_uuid( const Model& model,
            const geode::ComponentID& component_id,
            const std::vector< std::vector< geode::index_t > >&
                colocated_points_groups )
    {
        std::vector< std::vector< geode::index_t > >
            new_colocated_points_groups;
        for( const auto& point_group : colocated_points_groups )
        {
            auto filtered_colocation =
                absl::FixedArray< bool >( point_group.size(), false );
            for( const auto first_id : geode::Range{ point_group.size() - 1 } )
            {
                const auto first_uv = model.unique_vertex(
                    { component_id, point_group[first_id] } );
                for( const auto second_id :
                    geode::Range{ 1, point_group.size() } )
                {
                    if( first_uv
                        != model.unique_vertex(
                            { component_id, point_group[second_id] } ) )
                    {
                        filtered_colocation[first_id] = true;
                        filtered_colocation[second_id] = true;
                    }
                }
            }
            std::vector< geode::index_t > colocated_points;
            for( const auto pt_id : geode::Indices{ point_group } )
            {
                if( filtered_colocation[pt_id] )
                {
                    colocated_points.push_back( point_group[pt_id] );
                }
            }
            if( !colocated_points.empty() )
            {
                new_colocated_points_groups.push_back(
                    std::move( colocated_points ) );
            }
        }
        return new_colocated_points_groups;
    }

    geode::index_t nb_points(
        const std::vector< std::vector< geode::index_t > >& points_groups )
    {
        geode::index_t nb_colocated{ 0 };
        for( const auto& point_group : points_groups )
        {
            nb_colocated += point_group.size();
        }
        return nb_colocated;
    }

    template < geode::index_t dimension, typename Model >
    std::vector< geode::uuid > model_components_with_colocated_points_base(
        const Model& model, bool verbose )
    {
        std::vector< geode::uuid > components_with_colocation;
        for( const auto& line : model.lines() )
        {
            const geode::EdgedCurveColocation< dimension > inspector{
                line.mesh(), verbose
            };
            if( !filter_colocated_points_with_same_uuid< dimension, Model >(
                    model, line.component_id(),
                    inspector.colocated_points_groups() )
                     .empty() )
            {
                components_with_colocation.push_back( line.id() );
                if( verbose )
                {
                    geode::Logger::info( "Line with uuid ", line.id().string(),
                        " has colocated points." );
                }
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            const geode::SurfaceMeshColocation< dimension > inspector{
                surface.mesh(), verbose
            };
            if( !filter_colocated_points_with_same_uuid< dimension, Model >(
                    model, surface.component_id(),
                    inspector.colocated_points_groups() )
                     .empty() )
            {
                components_with_colocation.push_back( surface.id() );
                if( verbose )
                {
                    geode::Logger::info( "Surface with uuid ",
                        surface.id().string(), " has colocated points." );
                }
            }
        }
        return components_with_colocation;
    }

    std::vector< geode::uuid > model_components_with_colocated_points(
        const geode::Section& model, bool verbose )
    {
        return model_components_with_colocated_points_base< 2, geode::Section >(
            model, verbose );
    }

    std::vector< geode::uuid > model_components_with_colocated_points(
        const geode::BRep& model, bool verbose )
    {
        auto components_with_colocation =
            model_components_with_colocated_points_base< 3, geode::BRep >(
                model, verbose );
        for( const auto& block : model.blocks() )
        {
            const geode::SolidMeshColocation3D inspector{ block.mesh(),
                verbose };
            if( !filter_colocated_points_with_same_uuid< 3, geode::BRep >(
                    model, block.component_id(),
                    inspector.colocated_points_groups() )
                     .empty() )
            {
                components_with_colocation.push_back( block.id() );
                if( verbose )
                {
                    geode::Logger::info( "Block with uuid ",
                        block.id().string(), " has colocated points." );
                }
            }
        }
        return components_with_colocation;
    }

    template < geode::index_t dimension, typename Model >
    absl::flat_hash_map< geode::uuid, geode::index_t >
        model_components_nb_colocated_points_base(
            const Model& model, bool verbose )
    {
        absl::flat_hash_map< geode::uuid, geode::index_t >
            components_nb_colocated_points;
        for( const auto& line : model.lines() )
        {
            const geode::EdgedCurveColocation< dimension > inspector{
                line.mesh(), verbose
            };
            const auto nb_colocated = nb_points(
                filter_colocated_points_with_same_uuid< dimension, Model >(
                    model, line.component_id(),
                    inspector.colocated_points_groups() ) );
            if( nb_colocated > 0 )
            {
                components_nb_colocated_points.emplace(
                    line.id(), nb_colocated );
                if( verbose )
                {
                    geode::Logger::info( "Line with uuid ", line.id().string(),
                        " has ", nb_colocated, " colocated points." );
                }
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            const geode::SurfaceMeshColocation< dimension > inspector{
                surface.mesh(), verbose
            };
            const auto nb_colocated = nb_points(
                filter_colocated_points_with_same_uuid< dimension, Model >(
                    model, surface.component_id(),
                    inspector.colocated_points_groups() ) );
            if( nb_colocated > 0 )
            {
                components_nb_colocated_points.emplace(
                    surface.id(), nb_colocated );
                if( verbose )
                {
                    geode::Logger::info( "Surface with uuid ",
                        surface.id().string(), " has ", nb_colocated,
                        " colocated points." );
                }
            }
        }
        return components_nb_colocated_points;
    }

    absl::flat_hash_map< geode::uuid, geode::index_t >
        model_components_nb_colocated_points(
            const geode::Section& model, bool verbose )
    {
        return model_components_nb_colocated_points_base< 2, geode::Section >(
            model, verbose );
    }

    absl::flat_hash_map< geode::uuid, geode::index_t >
        model_components_nb_colocated_points(
            const geode::BRep& model, bool verbose )
    {
        auto components_nb_colocated_points =
            model_components_nb_colocated_points_base< 3, geode::BRep >(
                model, verbose );
        for( const auto& block : model.blocks() )
        {
            const geode::SolidMeshColocation3D inspector{ block.mesh(),
                verbose };
            const auto nb_colocated = nb_points(
                filter_colocated_points_with_same_uuid< 3, geode::BRep >( model,
                    block.component_id(),
                    inspector.colocated_points_groups() ) );
            if( nb_colocated > 0 )
            {
                components_nb_colocated_points.emplace(
                    block.id(), nb_colocated );
                if( verbose )
                {
                    geode::Logger::info( "Block with uuid ",
                        block.id().string(), " has ", nb_colocated,
                        " colocated points." );
                }
            }
        }
        return components_nb_colocated_points;
    }

    template < geode::index_t dimension, typename Model >
    absl::flat_hash_map< geode::uuid,
        std::vector< std::vector< geode::index_t > > >
        model_components_colocated_points_groups_base(
            const Model& model, bool verbose )
    {
        absl::flat_hash_map< geode::uuid,
            std::vector< std::vector< geode::index_t > > >
            components_colocated_points;
        for( const auto& line : model.lines() )
        {
            const geode::EdgedCurveColocation< dimension > inspector{
                line.mesh(), verbose
            };
            auto colocated_pts =
                filter_colocated_points_with_same_uuid< dimension, Model >(
                    model, line.component_id(),
                    inspector.colocated_points_groups() );
            if( !colocated_pts.empty() )
            {
                components_colocated_points.emplace(
                    line.id(), std::move( colocated_pts ) );
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            const geode::SurfaceMeshColocation< dimension > inspector{
                surface.mesh(), verbose
            };
            auto colocated_pts =
                filter_colocated_points_with_same_uuid< dimension, Model >(
                    model, surface.component_id(),
                    inspector.colocated_points_groups() );
            if( !colocated_pts.empty() )
            {
                components_colocated_points.emplace(
                    surface.id(), std::move( colocated_pts ) );
            }
        }
        return components_colocated_points;
    }

    absl::flat_hash_map< geode::uuid,
        std::vector< std::vector< geode::index_t > > >
        model_components_colocated_points_groups(
            const geode::Section& model, bool verbose )
    {
        return model_components_colocated_points_groups_base< 2,
            geode::Section >( model, verbose );
    }

    absl::flat_hash_map< geode::uuid,
        std::vector< std::vector< geode::index_t > > >
        model_components_colocated_points_groups(
            const geode::BRep& model, bool verbose )
    {
        auto components_colocated_points =
            model_components_colocated_points_groups_base< 3, geode::BRep >(
                model, verbose );
        for( const auto& block : model.blocks() )
        {
            const geode::SolidMeshColocation3D inspector{ block.mesh(),
                verbose };
            auto colocated_pts =
                filter_colocated_points_with_same_uuid< 3, geode::BRep >( model,
                    block.component_id(), inspector.colocated_points_groups() );
            if( !colocated_pts.empty() )
            {
                components_colocated_points.emplace(
                    block.id(), std::move( colocated_pts ) );
            }
        }
        return components_colocated_points;
    }
} // namespace

namespace geode
{
    template < geode::index_t dimension, typename Model >
    class ComponentMeshesColocation< dimension, Model >::Impl
    {
    public:
        Impl( const Model& model, bool verbose )
            : model_( model ), verbose_( verbose )
        {
        }

        std::vector< uuid > components_with_colocated_points() const
        {
            return model_components_with_colocated_points( model_, verbose_ );
        }

        absl::flat_hash_map< uuid, index_t >
            components_nb_colocated_points() const
        {
            return model_components_nb_colocated_points( model_, verbose_ );
        }

        absl::flat_hash_map< uuid, std::vector< std::vector< index_t > > >
            components_colocated_points_groups() const
        {
            return model_components_colocated_points_groups( model_, verbose_ );
        }

    private:
        const Model& model_;
        DEBUG_CONST bool verbose_;
    };

    template < geode::index_t dimension, typename Model >
    ComponentMeshesColocation< dimension, Model >::ComponentMeshesColocation(
        const Model& model )
        : impl_( model, false )
    {
    }

    template < geode::index_t dimension, typename Model >
    ComponentMeshesColocation< dimension, Model >::ComponentMeshesColocation(
        const Model& model, bool verbose )
        : impl_( model, verbose )
    {
    }

    template < geode::index_t dimension, typename Model >
    ComponentMeshesColocation< dimension, Model >::~ComponentMeshesColocation()
    {
    }

    template < geode::index_t dimension, typename Model >
    std::vector< uuid > ComponentMeshesColocation< dimension,
        Model >::components_with_colocated_points() const
    {
        return impl_->components_with_colocated_points();
    }

    template < geode::index_t dimension, typename Model >
    absl::flat_hash_map< uuid, index_t > ComponentMeshesColocation< dimension,
        Model >::components_nb_colocated_points() const
    {
        return impl_->components_nb_colocated_points();
    }

    template < geode::index_t dimension, typename Model >
    absl::flat_hash_map< uuid, std::vector< std::vector< index_t > > >
        ComponentMeshesColocation< dimension,
            Model >::components_colocated_points_groups() const
    {
        return impl_->components_colocated_points_groups();
    }

    template class opengeode_inspector_inspector_api
        ComponentMeshesColocation< 2, Section >;
    template class opengeode_inspector_inspector_api
        ComponentMeshesColocation< 3, BRep >;
} // namespace geode
