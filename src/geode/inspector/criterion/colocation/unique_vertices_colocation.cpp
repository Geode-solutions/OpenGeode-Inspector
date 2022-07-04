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

#include <geode/inspector/criterion/colocation/unique_vertices_colocation.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/builder/point_set_builder.h>
#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/colocation/pointset_colocation.h>

namespace
{
    template < geode::index_t dimension, typename Model >
    bool model_mcv_is_colocated_on_point_base( const Model& model,
        const geode::MeshComponentVertex& mcv,
        const geode::Point< dimension >& point )
    {
        if( mcv.component_id.type()
            == geode::Line< dimension >::component_type_static() )
        {
            return point.inexact_equal(
                model.line( mcv.component_id.id() ).mesh().point( mcv.vertex ),
                geode::global_epsilon );
        }
        else if( mcv.component_id.type()
                 == geode::Surface< dimension >::component_type_static() )
        {
            return point.inexact_equal( model.surface( mcv.component_id.id() )
                                            .mesh()
                                            .point( mcv.vertex ),
                geode::global_epsilon );
        }
        return point.inexact_equal(
            model.corner( mcv.component_id.id() ).mesh().point( mcv.vertex ),
            geode::global_epsilon );
    }

    bool model_mcv_is_colocated_on_point( const geode::Section& model,
        const geode::MeshComponentVertex& mcv,
        const geode::Point2D& point )
    {
        return model_mcv_is_colocated_on_point_base( model, mcv, point );
    }

    bool model_mcv_is_colocated_on_point( const geode::BRep& model,
        const geode::MeshComponentVertex& mcv,
        const geode::Point3D& point )
    {
        if( mcv.component_id.type() == geode::Block3D::component_type_static() )
        {
            return point.inexact_equal(
                model.block( mcv.component_id.id() ).mesh().point( mcv.vertex ),
                geode::global_epsilon );
        }
        return model_mcv_is_colocated_on_point_base( model, mcv, point );
    }

    template < geode::index_t dimension, typename Model >
    bool model_mcvs_are_colocated_on_point( const Model& model,
        const std::vector< geode::MeshComponentVertex >& mcvs,
        const geode::Point< dimension >& point )
    {
        for( const auto& mcv : mcvs )
        {
            if( !model_mcv_is_colocated_on_point( model, mcv, point ) )
            {
                return false;
            }
        }
        return true;
    }

    template < geode::index_t dimension, typename Model >
    geode::Point< dimension > model_unique_vertex_point_base(
        const Model& model, const geode::MeshComponentVertex& mcv )
    {
        if( mcv.component_id.type()
            == geode::Line< dimension >::component_type_static() )
        {
            const auto& mesh = model.line( mcv.component_id.id() ).mesh();
            return mesh.point( mcv.vertex );
        }
        if( mcv.component_id.type()
            == geode::Surface< dimension >::component_type_static() )
        {
            const auto& mesh = model.surface( mcv.component_id.id() ).mesh();
            return mesh.point( mcv.vertex );
        }
        const auto& mesh = model.corner( mcv.component_id.id() ).mesh();
        return mesh.point( mcv.vertex );
    }

    geode::Point2D model_unique_vertex_point(
        const geode::Section& model, const geode::MeshComponentVertex& mcv )
    {
        return model_unique_vertex_point_base< 2, geode::Section >(
            model, mcv );
    }

    geode::Point3D model_unique_vertex_point(
        const geode::BRep& model, const geode::MeshComponentVertex& mcv )
    {
        if( mcv.component_id.type() == geode::Block3D::component_type_static() )
        {
            const auto& mesh = model.block( mcv.component_id.id() ).mesh();
            return mesh.point( mcv.vertex );
        }
        return model_unique_vertex_point_base< 3, geode::BRep >( model, mcv );
    }
} // namespace

namespace geode
{
    template < geode::index_t dimension, typename Model >
    class UniqueVerticesColocation< dimension, Model >::Impl
    {
    public:
        Impl( const Model& model, bool verbose )
            : model_( model ),
              verbose_( verbose ),
              unique_vertices_{ PointSet< dimension >::create() }
        {
            auto builder =
                PointSetBuilder< dimension >::create( *unique_vertices_ );
            builder->create_vertices( model.nb_unique_vertices() );
            for( const auto unique_vertex_id :
                Range{ model.nb_unique_vertices() } )
            {
                const auto& mcvs =
                    model.mesh_component_vertices( unique_vertex_id );
                if( mcvs.empty() )
                {
                    continue;
                }
                builder->set_point( unique_vertex_id,
                    model_unique_vertex_point( model, mcvs.at( 0 ) ) );
            }
        }

        bool model_has_unique_vertices_linked_to_different_points() const
        {
            for( const auto unique_vertex_id :
                Range{ model_.nb_unique_vertices() } )
            {
                if( !model_mcvs_are_colocated_on_point( model_,
                        model_.mesh_component_vertices( unique_vertex_id ),
                        unique_vertices_->point( unique_vertex_id ) ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_id,
                            " has component mesh vertices which are not "
                            "on the same position." );
                    }
                    return true;
                }
            }
            return false;
        }

        bool model_has_colocated_unique_vertices() const
        {
            const PointSetColocation< dimension > pointset_inspector{
                *unique_vertices_, verbose_
            };
            return pointset_inspector.mesh_has_colocated_points();
        }

        index_t nb_unique_vertices_linked_to_different_points() const
        {
            index_t nb_vertices{ 0 };
            for( const auto unique_vertex_id :
                Range{ model_.nb_unique_vertices() } )
            {
                if( !model_mcvs_are_colocated_on_point( model_,
                        model_.mesh_component_vertices( unique_vertex_id ),
                        unique_vertices_->point( unique_vertex_id ) ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_id,
                            " has component mesh vertices which are not "
                            "on the same position." );
                    }
                    nb_vertices++;
                }
            }
            return nb_vertices;
        }

        index_t nb_colocated_unique_vertices() const
        {
            const PointSetColocation< dimension > pointset_inspector{
                *unique_vertices_, verbose_
            };
            return pointset_inspector.nb_colocated_points();
        }

        std::vector< index_t >
            unique_vertices_linked_to_different_points() const
        {
            std::vector< index_t > vertices;
            for( const auto unique_vertex_id :
                Range{ model_.nb_unique_vertices() } )
            {
                if( !model_mcvs_are_colocated_on_point( model_,
                        model_.mesh_component_vertices( unique_vertex_id ),
                        unique_vertices_->point( unique_vertex_id ) ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with index ",
                            unique_vertex_id,
                            " has component mesh vertices which are not "
                            "on the same position." );
                    }
                    vertices.push_back( unique_vertex_id );
                }
            }
            return vertices;
        }

        std::vector< std::vector< index_t > >
            colocated_unique_vertices_groups() const
        {
            const PointSetColocation< dimension > pointset_inspector{
                *unique_vertices_, verbose_
            };
            return pointset_inspector.colocated_points_groups();
        }

    private:
        const Model& model_;
        DEBUG_CONST bool verbose_;
        std::unique_ptr< PointSet< dimension > > unique_vertices_;
    };

    template < geode::index_t dimension, typename Model >
    UniqueVerticesColocation< dimension, Model >::UniqueVerticesColocation(
        const Model& model )
        : impl_( model, false )
    {
    }

    template < geode::index_t dimension, typename Model >
    UniqueVerticesColocation< dimension, Model >::UniqueVerticesColocation(
        const Model& model, bool verbose )
        : impl_( model, verbose )
    {
    }

    template < geode::index_t dimension, typename Model >
    UniqueVerticesColocation< dimension, Model >::~UniqueVerticesColocation()
    {
    }

    template < geode::index_t dimension, typename Model >
    bool UniqueVerticesColocation< dimension,
        Model >::model_has_unique_vertices_linked_to_different_points() const
    {
        return impl_->model_has_unique_vertices_linked_to_different_points();
    }

    template < geode::index_t dimension, typename Model >
    bool UniqueVerticesColocation< dimension,
        Model >::model_has_colocated_unique_vertices() const
    {
        return impl_->model_has_colocated_unique_vertices();
    }

    template < geode::index_t dimension, typename Model >
    index_t UniqueVerticesColocation< dimension,
        Model >::nb_colocated_unique_vertices() const
    {
        return impl_->nb_colocated_unique_vertices();
    }

    template < geode::index_t dimension, typename Model >
    index_t UniqueVerticesColocation< dimension,
        Model >::nb_unique_vertices_linked_to_different_points() const
    {
        return impl_->nb_unique_vertices_linked_to_different_points();
    }

    template < geode::index_t dimension, typename Model >
    std::vector< std::vector< index_t > > UniqueVerticesColocation< dimension,
        Model >::colocated_unique_vertices_groups() const
    {
        return impl_->colocated_unique_vertices_groups();
    }

    template < geode::index_t dimension, typename Model >
    std::vector< index_t > UniqueVerticesColocation< dimension,
        Model >::unique_vertices_linked_to_different_points() const
    {
        return impl_->unique_vertices_linked_to_different_points();
    }

    template class opengeode_inspector_inspector_api
        UniqueVerticesColocation< 2, Section >;
    template class opengeode_inspector_inspector_api
        UniqueVerticesColocation< 3, BRep >;
} // namespace geode
