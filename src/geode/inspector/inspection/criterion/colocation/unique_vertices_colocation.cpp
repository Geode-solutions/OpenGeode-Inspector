/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/inspector/inspection/criterion/colocation/unique_vertices_colocation.hpp>

#include <absl/base/call_once.h>

#include <async++.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/builder/point_set_builder.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/inspection/criterion/colocation/pointset_colocation.hpp>

namespace
{
    // Point of a component mesh vertex, std::nullopt if its component is
    // inactive
    template < geode::index_t dimension, typename Component >
    std::optional< geode::Point< dimension > > active_component_point(
        const Component& component, geode::index_t vertex )
    {
        if( !component.is_active() )
        {
            return std::nullopt;
        }
        return component.mesh().point( vertex );
    }

    template < typename Model >
    std::optional< geode::Point< Model::dim > > active_cmv_point_base(
        const Model& model, const geode::ComponentMeshVertex& cmv )
    {
        const auto& type = cmv.component_id.type;
        const auto& id = cmv.component_id.id;
        if( type == geode::Line< Model::dim >::component_type_static() )
        {
            return active_component_point< Model::dim >(
                model.line( id ), cmv.vertex );
        }
        if( type == geode::Surface< Model::dim >::component_type_static() )
        {
            return active_component_point< Model::dim >(
                model.surface( id ), cmv.vertex );
        }
        return active_component_point< Model::dim >(
            model.corner( id ), cmv.vertex );
    }

    std::optional< geode::Point2D > active_cmv_point(
        const geode::Section& model, const geode::ComponentMeshVertex& cmv )
    {
        return active_cmv_point_base( model, cmv );
    }

    std::optional< geode::Point3D > active_cmv_point(
        const geode::BRep& model, const geode::ComponentMeshVertex& cmv )
    {
        if( cmv.component_id.type == geode::Block3D::component_type_static() )
        {
            return active_component_point< 3 >(
                model.block( cmv.component_id.id ), cmv.vertex );
        }
        return active_cmv_point_base( model, cmv );
    }

    template < typename Model >
    std::optional< geode::Point< Model::dim > > model_unique_vertex_point(
        const Model& model,
        absl::Span< const geode::ComponentMeshVertex > cmvs )
    {
        for( const auto& cmv : cmvs )
        {
            if( auto point = active_cmv_point( model, cmv ) )
            {
                return point;
            }
        }
        return std::nullopt;
    }

    template < typename Model >
    bool model_cmvs_are_colocated_on_point( const Model& model,
        absl::Span< const geode::ComponentMeshVertex > cmvs,
        const geode::Point< Model::dim >& point )
    {
        for( const auto& cmv : cmvs )
        {
            const auto cmv_point = active_cmv_point( model, cmv );
            if( cmv_point && !point.inexact_equal( cmv_point.value() ) )
            {
                return false;
            }
        }
        return true;
    }
} // namespace

namespace geode
{
    index_t UniqueVerticesInspectionResult::nb_issues() const
    {
        return colocated_unique_vertices_groups.nb_issues()
               + unique_vertices_linked_to_different_points.nb_issues();
    }

    std::string UniqueVerticesInspectionResult::string() const
    {
        return absl::StrCat( colocated_unique_vertices_groups.string(),
            unique_vertices_linked_to_different_points.string() );
    }

    std::string UniqueVerticesInspectionResult::inspection_type() const
    {
        return "Unique vertices colocation inspection";
    }

    template < typename Model >
    class UniqueVerticesColocation< Model >::Impl
    {
    public:
        Impl( const Model& model ) : model_( model ) {}

        bool model_has_unique_vertices_linked_to_different_points() const
        {
            return !unique_vertices_linked_to_different_points().empty();
        }

        bool model_has_colocated_unique_vertices() const
        {
            const PointSetColocation< Model::dim > pointset_inspector{
                active_uv_pointset()
            };
            return pointset_inspector.mesh_has_colocated_points();
        }

        void add_unique_vertices_linked_to_different_points(
            InspectionIssues< index_t >& vertices_issues ) const
        {
            for( const auto unique_vertex_id :
                unique_vertices_linked_to_different_points() )
            {
                vertices_issues.add_issue( unique_vertex_id,
                    absl::StrCat( "unique vertex ", unique_vertex_id,
                        " is linked to several mesh vertices on different "
                        "positions" ) );
            }
        }

        void add_colocated_unique_vertices_groups(
            InspectionIssues< std::vector< index_t > >& vertices_issues ) const
        {
            const auto& pointset = active_uv_pointset();
            const PointSetColocation< Model::dim > pointset_inspector{
                pointset
            };
            const auto colocated_pts_groups =
                pointset_inspector.colocated_points_groups();
            for( const auto& point_group : colocated_pts_groups.issues() )
            {
                std::vector< index_t > fixed_point_group;
                fixed_point_group.reserve( point_group.size() );
                std::string point_group_string;
                for( const auto active_uv_index : point_group )
                {
                    const auto model_uv_index =
                        active_uv_to_uv_[active_uv_index];
                    fixed_point_group.push_back( model_uv_index );
                    absl::StrAppend( &point_group_string, " ", model_uv_index );
                }
                vertices_issues.add_issue( fixed_point_group,
                    absl::StrCat( "unique vertices", point_group_string,
                        " are colocated at the position [",
                        pointset.point( point_group[0] ).string(), "]" ) );
            }
        }

    private:
        // Computed once, on first use
        const PointSet< Model::dim >& active_uv_pointset() const
        {
            absl::call_once( once_, [this] {
                initialize();
            } );
            return *active_uv_pointset_;
        }

        void initialize() const
        {
            const auto nb_unique_vertices = model_.nb_unique_vertices();
            std::vector< std::optional< Point< Model::dim > > > points(
                nb_unique_vertices );
            async::parallel_for(
                async::irange( index_t{ 0 }, nb_unique_vertices ),
                [this, &points]( index_t unique_vertex_id ) {
                    points[unique_vertex_id] = model_unique_vertex_point(
                        model_,
                        model_.component_mesh_vertices( unique_vertex_id ) );
                } );
            active_uv_pointset_ = PointSet< Model::dim >::create();
            auto builder =
                PointSetBuilder< Model::dim >::create( *active_uv_pointset_ );
            uv_to_active_uv_.resize( nb_unique_vertices, NO_ID );
            for( const auto unique_vertex_id : Range{ nb_unique_vertices } )
            {
                if( const auto& point = points[unique_vertex_id] )
                {
                    uv_to_active_uv_[unique_vertex_id] =
                        builder->create_point( point.value() );
                    active_uv_to_uv_.push_back( unique_vertex_id );
                }
            }
        }

        std::vector< index_t >
            unique_vertices_linked_to_different_points() const
        {
            const auto& pointset = active_uv_pointset();
            const auto nb_unique_vertices = model_.nb_unique_vertices();
            std::vector< char > linked_to_different_points(
                nb_unique_vertices, false );
            async::parallel_for(
                async::irange( index_t{ 0 }, nb_unique_vertices ),
                [this, &pointset, &linked_to_different_points](
                    index_t unique_vertex_id ) {
                    const auto active_uv_id =
                        uv_to_active_uv_[unique_vertex_id];
                    if( active_uv_id == NO_ID )
                    {
                        return;
                    }
                    linked_to_different_points[unique_vertex_id] =
                        !model_cmvs_are_colocated_on_point( model_,
                            model_.component_mesh_vertices( unique_vertex_id ),
                            pointset.point( active_uv_id ) );
                } );
            std::vector< index_t > result;
            for( const auto unique_vertex_id : Range{ nb_unique_vertices } )
            {
                if( linked_to_different_points[unique_vertex_id] )
                {
                    result.push_back( unique_vertex_id );
                }
            }
            return result;
        }

    private:
        const Model& model_;
        mutable absl::once_flag once_;
        mutable std::unique_ptr< PointSet< Model::dim > > active_uv_pointset_;
        mutable std::vector< index_t > uv_to_active_uv_;
        mutable std::vector< index_t > active_uv_to_uv_;
    };

    template < typename Model >
    UniqueVerticesColocation< Model >::UniqueVerticesColocation(
        const Model& model )
        : impl_( model )
    {
    }

    template < typename Model >
    UniqueVerticesColocation< Model >::~UniqueVerticesColocation() = default;

    template < typename Model >
    bool UniqueVerticesColocation<
        Model >::model_has_unique_vertices_linked_to_different_points() const
    {
        return impl_->model_has_unique_vertices_linked_to_different_points();
    }

    template < typename Model >
    bool
        UniqueVerticesColocation< Model >::model_has_colocated_unique_vertices()
            const
    {
        return impl_->model_has_colocated_unique_vertices();
    }

    template < typename Model >
    UniqueVerticesInspectionResult
        UniqueVerticesColocation< Model >::inspect_unique_vertices() const
    {
        UniqueVerticesInspectionResult result;
        async::parallel_invoke(
            [&result, this] {
                impl_->add_colocated_unique_vertices_groups(
                    result.colocated_unique_vertices_groups );
            },
            [&result, this] {
                impl_->add_unique_vertices_linked_to_different_points(
                    result.unique_vertices_linked_to_different_points );
            } );
        return result;
    }

    template class opengeode_inspector_inspection_api
        UniqueVerticesColocation< Section >;
    template class opengeode_inspector_inspection_api
        UniqueVerticesColocation< BRep >;
} // namespace geode
