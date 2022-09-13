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
    std::vector< geode::uuid > model_components_with_colocated_points_base(
        const Model& model, bool verbose )
    {
        std::vector< geode::uuid > components_with_colocation;
        for( const auto& line : model.lines() )
        {
            const geode::EdgedCurveColocation< dimension > inspector{
                line.mesh(), false
            };
            if( inspector.mesh_has_colocated_points() )
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
                surface.mesh(), false
            };
            if( inspector.mesh_has_colocated_points() )
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
            const geode::SolidMeshColocation3D inspector{ block.mesh(), false };
            if( inspector.mesh_has_colocated_points() )
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
                line.mesh(), false
            };
            const auto nb_colocated = inspector.nb_colocated_points();
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
                surface.mesh(), false
            };
            const auto nb_colocated = inspector.nb_colocated_points();
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
            const geode::SolidMeshColocation3D inspector{ block.mesh(), false };
            const auto nb_colocated = inspector.nb_colocated_points();
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
            auto colocated_pts = inspector.colocated_points_groups();
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
            auto colocated_pts = inspector.colocated_points_groups();
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
            auto colocated_pts = inspector.colocated_points_groups();
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
