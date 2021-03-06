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

#include <geode/inspector/criterion/degeneration/component_meshes_degeneration.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/degeneration/edgedcurve_degeneration.h>
#include <geode/inspector/criterion/degeneration/solid_degeneration.h>
#include <geode/inspector/criterion/degeneration/surface_degeneration.h>

namespace
{
    template < geode::index_t dimension, typename Model >
    std::vector< geode::uuid > model_components_with_degenerated_edges_base(
        const Model& model, bool verbose )
    {
        std::vector< geode::uuid > components_with_degeneration;
        for( const auto& line : model.lines() )
        {
            const geode::EdgedCurveDegeneration< dimension > inspector{
                line.mesh(), verbose
            };
            if( inspector.is_mesh_degenerated() )
            {
                components_with_degeneration.push_back( line.id() );
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            const geode::SurfaceMeshDegeneration< dimension > inspector{
                surface.mesh(), verbose
            };
            if( inspector.is_mesh_degenerated() )
            {
                components_with_degeneration.push_back( surface.id() );
            }
        }
        return components_with_degeneration;
    }

    std::vector< geode::uuid > model_components_with_degenerated_edges(
        const geode::Section& model, bool verbose )
    {
        return model_components_with_degenerated_edges_base< 2,
            geode::Section >( model, verbose );
    }

    std::vector< geode::uuid > model_components_with_degenerated_edges(
        const geode::BRep& model, bool verbose )
    {
        auto components_with_degeneration =
            model_components_with_degenerated_edges_base< 3, geode::BRep >(
                model, verbose );
        for( const auto& block : model.blocks() )
        {
            const geode::SolidMeshDegeneration3D inspector{ block.mesh(),
                verbose };
            if( inspector.is_mesh_degenerated() )
            {
                components_with_degeneration.push_back( block.id() );
            }
        }
        return components_with_degeneration;
    }

    template < geode::index_t dimension, typename Model >
    absl::flat_hash_map< geode::uuid, geode::index_t >
        model_components_nb_degenerated_edges_base(
            const Model& model, bool verbose )
    {
        absl::flat_hash_map< geode::uuid, geode::index_t >
            components_nb_degenerated_edges;
        for( const auto& line : model.lines() )
        {
            const geode::EdgedCurveDegeneration< dimension > inspector{
                line.mesh(), verbose
            };
            const auto nb_colocated = inspector.nb_degenerated_edges();
            if( nb_colocated > 0 )
            {
                components_nb_degenerated_edges.emplace(
                    line.id(), nb_colocated );
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            const geode::SurfaceMeshDegeneration< dimension > inspector{
                surface.mesh(), verbose
            };
            const auto nb_colocated = inspector.nb_degenerated_edges();
            if( nb_colocated > 0 )
            {
                components_nb_degenerated_edges.emplace(
                    surface.id(), nb_colocated );
            }
        }
        return components_nb_degenerated_edges;
    }

    absl::flat_hash_map< geode::uuid, geode::index_t >
        model_components_nb_degenerated_edges(
            const geode::Section& model, bool verbose )
    {
        return model_components_nb_degenerated_edges_base< 2, geode::Section >(
            model, verbose );
    }

    absl::flat_hash_map< geode::uuid, geode::index_t >
        model_components_nb_degenerated_edges(
            const geode::BRep& model, bool verbose )
    {
        auto components_nb_degenerated_edges =
            model_components_nb_degenerated_edges_base< 3, geode::BRep >(
                model, verbose );
        for( const auto& block : model.blocks() )
        {
            const geode::SolidMeshDegeneration3D inspector{ block.mesh(),
                verbose };
            const auto nb_colocated = inspector.nb_degenerated_edges();
            if( nb_colocated > 0 )
            {
                components_nb_degenerated_edges.emplace(
                    block.id(), nb_colocated );
            }
        }
        return components_nb_degenerated_edges;
    }

    template < geode::index_t dimension, typename Model >
    absl::flat_hash_map< geode::uuid, std::vector< geode::index_t > >
        model_components_degenerated_edges_base(
            const Model& model, bool verbose )
    {
        absl::flat_hash_map< geode::uuid, std::vector< geode::index_t > >
            components_degenerated_edges;
        for( const auto& line : model.lines() )
        {
            const geode::EdgedCurveDegeneration< dimension > inspector{
                line.mesh(), verbose
            };
            auto degenerated_edges = inspector.degenerated_edges();
            if( !degenerated_edges.empty() )
            {
                components_degenerated_edges.emplace(
                    line.id(), std::move( degenerated_edges ) );
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            const geode::SurfaceMeshDegeneration< dimension > inspector{
                surface.mesh(), verbose
            };
            auto degenerated_edges = inspector.degenerated_edges();
            if( !degenerated_edges.empty() )
            {
                components_degenerated_edges.emplace(
                    surface.id(), std::move( degenerated_edges ) );
            }
        }
        return components_degenerated_edges;
    }

    absl::flat_hash_map< geode::uuid, std::vector< geode::index_t > >
        model_components_degenerated_edges(
            const geode::Section& model, bool verbose )
    {
        return model_components_degenerated_edges_base< 2, geode::Section >(
            model, verbose );
    }

    absl::flat_hash_map< geode::uuid, std::vector< geode::index_t > >
        model_components_degenerated_edges(
            const geode::BRep& model, bool verbose )
    {
        auto components_degenerated_edges =
            model_components_degenerated_edges_base< 3, geode::BRep >(
                model, verbose );
        for( const auto& block : model.blocks() )
        {
            const geode::SolidMeshDegeneration3D inspector{ block.mesh(),
                verbose };
            auto degenerated_edges = inspector.degenerated_edges();
            if( !degenerated_edges.empty() )
            {
                components_degenerated_edges.emplace(
                    block.id(), std::move( degenerated_edges ) );
            }
        }
        return components_degenerated_edges;
    }
} // namespace

namespace geode
{
    template < geode::index_t dimension, typename Model >
    class ComponentMeshesDegeneration< dimension, Model >::Impl
    {
    public:
        Impl( const Model& model, bool verbose )
            : model_( model ), verbose_( verbose )
        {
        }

        std::vector< uuid > components_with_degenerated_edges() const
        {
            return model_components_with_degenerated_edges( model_, verbose_ );
        }

        absl::flat_hash_map< uuid, index_t >
            components_nb_degenerated_edges() const
        {
            return model_components_nb_degenerated_edges( model_, verbose_ );
        }

        absl::flat_hash_map< uuid, std::vector< index_t > >
            components_degenerated_edges() const
        {
            return model_components_degenerated_edges( model_, verbose_ );
        }

    private:
        const Model& model_;
        DEBUG_CONST bool verbose_;
    };

    template < geode::index_t dimension, typename Model >
    ComponentMeshesDegeneration< dimension,
        Model >::ComponentMeshesDegeneration( const Model& model )
        : impl_( model, false )
    {
    }

    template < geode::index_t dimension, typename Model >
    ComponentMeshesDegeneration< dimension,
        Model >::ComponentMeshesDegeneration( const Model& model, bool verbose )
        : impl_( model, verbose )
    {
    }

    template < geode::index_t dimension, typename Model >
    ComponentMeshesDegeneration< dimension,
        Model >::~ComponentMeshesDegeneration()
    {
    }

    template < geode::index_t dimension, typename Model >
    std::vector< uuid > ComponentMeshesDegeneration< dimension,
        Model >::components_with_degenerated_edges() const
    {
        return impl_->components_with_degenerated_edges();
    }

    template < geode::index_t dimension, typename Model >
    absl::flat_hash_map< uuid, index_t > ComponentMeshesDegeneration< dimension,
        Model >::components_nb_degenerated_edges() const
    {
        return impl_->components_nb_degenerated_edges();
    }

    template < geode::index_t dimension, typename Model >
    absl::flat_hash_map< uuid, std::vector< index_t > >
        ComponentMeshesDegeneration< dimension,
            Model >::components_degenerated_edges() const
    {
        return impl_->components_degenerated_edges();
    }

    template class opengeode_inspector_inspector_api
        ComponentMeshesDegeneration< 2, Section >;
    template class opengeode_inspector_inspector_api
        ComponentMeshesDegeneration< 3, BRep >;
} // namespace geode
