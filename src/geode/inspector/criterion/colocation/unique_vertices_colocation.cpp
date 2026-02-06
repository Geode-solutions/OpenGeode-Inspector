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

#include <geode/inspector/criterion/colocation/unique_vertices_colocation.hpp>

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

#include <geode/inspector/criterion/colocation/pointset_colocation.hpp>

namespace
{
    template < typename Model >
    bool model_cmv_is_colocated_on_point_base( const Model& model,
        const geode::ComponentMeshVertex& cmv,
        const geode::Point< Model::dim >& point )
    {
        if( cmv.component_id.type()
            == geode::Line< Model::dim >::component_type_static() )
        {
            return point.inexact_equal( model.line( cmv.component_id.id() )
                    .mesh()
                    .point( cmv.vertex ) );
        }
        else if( cmv.component_id.type()
                 == geode::Surface< Model::dim >::component_type_static() )
        {
            return point.inexact_equal( model.surface( cmv.component_id.id() )
                    .mesh()
                    .point( cmv.vertex ) );
        }
        return point.inexact_equal(
            model.corner( cmv.component_id.id() ).mesh().point( cmv.vertex ) );
    }

    bool model_cmv_is_colocated_on_point( const geode::Section& model,
        const geode::ComponentMeshVertex& cmv,
        const geode::Point2D& point )
    {
        return model_cmv_is_colocated_on_point_base< geode::Section >(
            model, cmv, point );
    }

    bool model_cmv_is_colocated_on_point( const geode::BRep& model,
        const geode::ComponentMeshVertex& cmv,
        const geode::Point3D& point )
    {
        if( cmv.component_id.type() == geode::Block3D::component_type_static() )
        {
            return point.inexact_equal( model.block( cmv.component_id.id() )
                    .mesh()
                    .point( cmv.vertex ) );
        }
        return model_cmv_is_colocated_on_point_base< geode::BRep >(
            model, cmv, point );
    }

    template < typename Model >
    bool model_cmvs_are_colocated_on_point( const Model& model,
        absl::Span< const geode::ComponentMeshVertex > cmvs,
        const geode::Point< Model::dim >& point )
    {
        for( const auto& cmv : cmvs )
        {
            if( !model.component( cmv.component_id.id() ).is_active() )
            {
                continue;
            }
            if( !model_cmv_is_colocated_on_point( model, cmv, point ) )
            {
                return false;
            }
        }
        return true;
    }

    template < typename Model >
    geode::Point< Model::dim > model_cmv_point(
        const Model& model, const geode::ComponentMeshVertex& cmv )
    {
        if( cmv.component_id.type()
            == geode::Line< Model::dim >::component_type_static() )
        {
            return model.line( cmv.component_id.id() )
                .mesh()
                .point( cmv.vertex );
        }
        if( cmv.component_id.type()
            == geode::Surface< Model::dim >::component_type_static() )
        {
            return model.surface( cmv.component_id.id() )
                .mesh()
                .point( cmv.vertex );
        }
        return model.corner( cmv.component_id.id() ).mesh().point( cmv.vertex );
    }

    std::optional< geode::Point2D > model_unique_vertex_point(
        const geode::Section& model,
        absl::Span< const geode::ComponentMeshVertex > cmvs )
    {
        for( const auto& cmv : cmvs )
        {
            if( !model.component( cmv.component_id.id() ).is_active() )
            {
                continue;
            }
            return model_cmv_point< geode::Section >( model, cmv );
        }
        return std::nullopt;
    }

    std::optional< geode::Point3D > model_unique_vertex_point(
        const geode::BRep& model,
        absl::Span< const geode::ComponentMeshVertex > cmvs )
    {
        for( const auto& cmv : cmvs )
        {
            if( !model.component( cmv.component_id.id() ).is_active() )
            {
                continue;
            }
            if( cmv.component_id.type()
                == geode::Block3D::component_type_static() )
            {
                return model.block( cmv.component_id.id() )
                    .mesh()
                    .point( cmv.vertex );
            }
            return model_cmv_point< geode::BRep >( model, cmv );
        }
        return std::nullopt;
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
        Impl( const Model& model )
            : model_( model ),
              active_uv_pointset_{ PointSet< Model::dim >::create() }
        {
            auto builder =
                PointSetBuilder< Model::dim >::create( *active_uv_pointset_ );
            builder->create_vertices( model.nb_unique_vertices() );
            std::vector< bool > not_assigned(
                model.nb_unique_vertices(), true );
            for( const auto unique_vertex_id :
                Range{ model.nb_unique_vertices() } )
            {
                if( auto point = model_unique_vertex_point( model,
                        model.component_mesh_vertices( unique_vertex_id ) ) )
                {
                    builder->set_point( unique_vertex_id, point.value() );
                    not_assigned[unique_vertex_id] = false;
                }
            }
            const auto old2new = builder->delete_vertices( not_assigned );
            for( const auto old_index : Indices{ old2new } )
            {
                uv_to_active_uv_.map( old_index, old2new[old_index] );
            }
        }

        bool model_has_unique_vertices_linked_to_different_points() const
        {
            for( const auto unique_vertex_id :
                Range{ model_.nb_unique_vertices() } )
            {
                const auto active_uv_id =
                    uv_to_active_uv_.in2out( unique_vertex_id ).at( 0 );
                if( active_uv_id == NO_ID )
                {
                    continue;
                }
                if( !model_cmvs_are_colocated_on_point( model_,
                        model_.component_mesh_vertices( unique_vertex_id ),
                        active_uv_pointset_->point( active_uv_id ) ) )
                {
                    return true;
                }
            }
            return false;
        }

        bool model_has_colocated_unique_vertices() const
        {
            const PointSetColocation< Model::dim > pointset_inspector{
                *active_uv_pointset_
            };
            const auto has_colocation =
                pointset_inspector.mesh_has_colocated_points();
            return has_colocation;
        }

        void add_unique_vertices_linked_to_different_points(
            InspectionIssues< index_t >& vertices_issues ) const
        {
            for( const auto unique_vertex_id :
                Range{ model_.nb_unique_vertices() } )
            {
                const auto active_uv_id =
                    uv_to_active_uv_.in2out( unique_vertex_id ).at( 0 );
                if( active_uv_id == NO_ID )
                {
                    continue;
                }
                if( !model_cmvs_are_colocated_on_point( model_,
                        model_.component_mesh_vertices( unique_vertex_id ),
                        active_uv_pointset_->point( active_uv_id ) ) )
                {
                    vertices_issues.add_issue( unique_vertex_id,
                        absl::StrCat( "unique vertex ", unique_vertex_id,
                            " is linked to several mesh vertices on different "
                            "positions" ) );
                }
            }
        }

        void add_colocated_unique_vertices_groups(
            InspectionIssues< std::vector< index_t > >& vertices_issues ) const
        {
            const PointSetColocation< Model::dim > pointset_inspector{
                *active_uv_pointset_
            };
            const auto colocated_pts_groups =
                pointset_inspector.colocated_points_groups();
            for( const auto& point_group : colocated_pts_groups.issues() )
            {
                std::vector< index_t > fixed_point_group;
                std::string point_group_string;
                for( const auto active_uv_index : point_group )
                {
                    const auto model_uv_index =
                        uv_to_active_uv_.out2in( active_uv_index ).at( 0 );
                    fixed_point_group.push_back( model_uv_index );
                    absl::StrAppend( &point_group_string, " ", model_uv_index );
                }
                vertices_issues.add_issue( fixed_point_group,
                    absl::StrCat( "unique vertices ", point_group_string,
                        " are colocated at the position [",
                        active_uv_pointset_->point( point_group[0] ).string(),
                        "]" ) );
            }
        }

    private:
        const Model& model_;
        std::unique_ptr< PointSet< Model::dim > > active_uv_pointset_;
        geode::GenericMapping< index_t > uv_to_active_uv_;
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

    template class opengeode_inspector_inspector_api
        UniqueVerticesColocation< Section >;
    template class opengeode_inspector_inspector_api
        UniqueVerticesColocation< BRep >;
} // namespace geode
