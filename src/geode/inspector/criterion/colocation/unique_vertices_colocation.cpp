/*
 * Copyright (c) 2019 - 2025 Geode-solutions
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
        const std::vector< geode::ComponentMeshVertex >& cmvs,
        const geode::Point< Model::dim >& point )
    {
        for( const auto& cmv : cmvs )
        {
            if( !model_cmv_is_colocated_on_point( model, cmv, point ) )
            {
                return false;
            }
        }
        return true;
    }

    template < typename Model >
    geode::Point< Model::dim > model_unique_vertex_point_base(
        const Model& model, const geode::ComponentMeshVertex& cmv )
    {
        if( cmv.component_id.type()
            == geode::Line< Model::dim >::component_type_static() )
        {
            const auto& mesh = model.line( cmv.component_id.id() ).mesh();
            return mesh.point( cmv.vertex );
        }
        if( cmv.component_id.type()
            == geode::Surface< Model::dim >::component_type_static() )
        {
            const auto& mesh = model.surface( cmv.component_id.id() ).mesh();
            return mesh.point( cmv.vertex );
        }
        const auto& mesh = model.corner( cmv.component_id.id() ).mesh();
        return mesh.point( cmv.vertex );
    }

    geode::Point2D model_unique_vertex_point(
        const geode::Section& model, const geode::ComponentMeshVertex& cmv )
    {
        return model_unique_vertex_point_base< geode::Section >( model, cmv );
    }

    geode::Point3D model_unique_vertex_point(
        const geode::BRep& model, const geode::ComponentMeshVertex& cmv )
    {
        if( cmv.component_id.type() == geode::Block3D::component_type_static() )
        {
            const auto& mesh = model.block( cmv.component_id.id() ).mesh();
            return mesh.point( cmv.vertex );
        }
        return model_unique_vertex_point_base< geode::BRep >( model, cmv );
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
        std::string message;
        absl::StrAppend(
            &message, colocated_unique_vertices_groups.string(), "\n" );
        absl::StrAppend( &message,
            unique_vertices_linked_to_different_points.string(), "\n" );
        return message;
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
              unique_vertices_{ PointSet< Model::dim >::create() }
        {
            auto builder =
                PointSetBuilder< Model::dim >::create( *unique_vertices_ );
            builder->create_vertices( model.nb_unique_vertices() );
            for( const auto unique_vertex_id :
                Range{ model.nb_unique_vertices() } )
            {
                const auto& cmvs =
                    model.component_mesh_vertices( unique_vertex_id );
                if( cmvs.empty() )
                {
                    continue;
                }
                builder->set_point( unique_vertex_id,
                    model_unique_vertex_point( model, cmvs.at( 0 ) ) );
            }
        }

        bool model_has_unique_vertices_linked_to_different_points() const
        {
            for( const auto unique_vertex_id :
                Range{ model_.nb_unique_vertices() } )
            {
                if( !model_cmvs_are_colocated_on_point( model_,
                        model_.component_mesh_vertices( unique_vertex_id ),
                        unique_vertices_->point( unique_vertex_id ) ) )
                {
                    return true;
                }
            }
            return false;
        }

        bool model_has_colocated_unique_vertices() const
        {
            const PointSetColocation< Model::dim > pointset_inspector{
                *unique_vertices_
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
                if( !model_cmvs_are_colocated_on_point( model_,
                        model_.component_mesh_vertices( unique_vertex_id ),
                        unique_vertices_->point( unique_vertex_id ) ) )
                {
                    vertices_issues.add_issue( unique_vertex_id,
                        absl::StrCat( "Unique vertex with index ",
                            unique_vertex_id,
                            " has component mesh vertices which are not "
                            "on the same position." ) );
                }
            }
        }

        void add_colocated_unique_vertices_groups(
            InspectionIssues< std::vector< index_t > >& vertices_issues ) const
        {
            const PointSetColocation< Model::dim > pointset_inspector{
                *unique_vertices_
            };
            const auto colocated_pts_groups =
                pointset_inspector.colocated_points_groups();
            for( const auto& point_group : colocated_pts_groups.issues() )
            {
                std::vector< index_t > fixed_point_group;
                std::string point_group_string;
                for( const auto point_index : Indices{ point_group } )
                {
                    if( model_
                            .component_mesh_vertices( point_group[point_index] )
                            .empty() )
                    {
                        continue;
                    }
                    fixed_point_group.push_back( point_group[point_index] );
                    absl::StrAppend(
                        &point_group_string, " ", point_group[point_index] );
                }
                if( !fixed_point_group.empty() )
                {
                    vertices_issues.add_issue( fixed_point_group,
                        absl::StrCat( "Unique vertices with indices",
                            point_group_string, " are colocated at position [",
                            unique_vertices_->point( fixed_point_group[0] )
                                .string(),
                            "]." ) );
                }
            }
        }

    private:
        const Model& model_;
        std::unique_ptr< PointSet< Model::dim > > unique_vertices_;
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
