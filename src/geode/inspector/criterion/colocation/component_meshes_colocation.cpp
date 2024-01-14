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

#include <geode/inspector/criterion/colocation/component_meshes_colocation.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/point.h>

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
            absl::Span< const std::vector< geode::index_t > >
                colocated_points_groups )
    {
        std::vector< std::vector< geode::index_t > >
            new_colocated_points_groups;
        for( const auto& point_group : colocated_points_groups )
        {
            absl::FixedArray< bool > filtered_colocation(
                point_group.size() - 1, true );
            for( const auto first_id : geode::Range{ point_group.size() - 1 } )
            {
                if( !filtered_colocation[first_id] )
                {
                    continue;
                }
                const auto first_uv = model.unique_vertex(
                    { component_id, point_group[first_id] } );
                for( const auto second_id :
                    geode::Range{ first_id + 1, point_group.size() } )
                {
                    if( first_uv
                        == model.unique_vertex(
                            { component_id, point_group[second_id] } ) )
                    {
                        filtered_colocation[second_id - 1] = false;
                    }
                }
            }
            std::vector< geode::index_t > colocated_points;
            for( const auto pt_id : geode::Indices{ filtered_colocation } )
            {
                if( filtered_colocation[pt_id] )
                {
                    colocated_points.push_back( point_group[pt_id + 1] );
                }
            }
            if( !colocated_points.empty() )
            {
                colocated_points.push_back( point_group[0] );
                new_colocated_points_groups.emplace_back(
                    std::move( colocated_points ) );
            }
        }
        return new_colocated_points_groups;
    }

    template < geode::index_t dimension, typename Model >
    absl::flat_hash_map< geode::uuid,
        geode::InspectionIssues< std::vector< geode::index_t > > >
        model_components_colocated_points_groups_base( const Model& model )
    {
        absl::flat_hash_map< geode::uuid,
            geode::InspectionIssues< std::vector< geode::index_t > > >
            components_colocated_points;
        for( const auto& line : model.lines() )
        {
            const geode::EdgedCurveColocation< dimension > inspector{
                line.mesh()
            };
            auto colocated_pts =
                filter_colocated_points_with_same_uuid< dimension, Model >(
                    model, line.component_id(),
                    inspector.colocated_points_groups().problems );
            if( !colocated_pts.empty() )
            {
                geode::InspectionIssues< std::vector< geode::index_t > >
                    line_issues{ "Colocated vertices on the Line with uuid "
                                 + line.id().string() };
                const auto& line_mesh = line.mesh();
                for( const auto& colocated_points_group : colocated_pts )
                {
                    std::string point_group_string{ "" };
                    for( const auto point_index : colocated_points_group )
                    {
                        absl::StrAppend(
                            &point_group_string, " ", point_index );
                    }
                    line_issues.add_problem( colocated_points_group,
                        absl::StrCat( "Line with uuid ", line.id().string(),
                            " has vertices with indices", point_group_string,
                            " which are colocated at position [",
                            line_mesh.point( colocated_points_group[0] )
                                .string(),
                            "]." ) );
                }
                components_colocated_points.emplace( line.id(), line_issues );
            }
        }
        for( const auto& surface : model.surfaces() )
        {
            const geode::SurfaceMeshColocation< dimension > inspector{
                surface.mesh()
            };
            auto colocated_pts =
                filter_colocated_points_with_same_uuid< dimension, Model >(
                    model, surface.component_id(),
                    inspector.colocated_points_groups().problems );
            if( !colocated_pts.empty() )
            {
                geode::InspectionIssues< std::vector< geode::index_t > >
                    surface_issues{
                        "Colocated vertices on the Surface with uuid "
                        + surface.id().string()
                    };
                const auto& surface_mesh = surface.mesh();
                for( const auto& colocated_points_group : colocated_pts )
                {
                    std::string point_group_string{ "" };
                    for( const auto point_index : colocated_points_group )
                    {
                        absl::StrAppend(
                            &point_group_string, " ", point_index );
                    }
                    surface_issues.add_problem( colocated_points_group,
                        absl::StrCat( "Surface with uuid ",
                            surface.id().string(), " has vertices with indices",
                            point_group_string,
                            " which are colocated at position [",
                            surface_mesh.point( colocated_points_group[0] )
                                .string(),
                            "]." ) );
                }
                components_colocated_points.emplace(
                    surface.id(), surface_issues );
            }
        }
        return components_colocated_points;
    }

    absl::flat_hash_map< geode::uuid,
        geode::InspectionIssues< std::vector< geode::index_t > > >
        model_components_colocated_points_groups( const geode::Section& model )
    {
        return model_components_colocated_points_groups_base< 2,
            geode::Section >( model );
    }

    absl::flat_hash_map< geode::uuid,
        geode::InspectionIssues< std::vector< geode::index_t > > >
        model_components_colocated_points_groups( const geode::BRep& model )
    {
        auto components_colocated_points =
            model_components_colocated_points_groups_base< 3, geode::BRep >(
                model );
        for( const auto& block : model.blocks() )
        {
            const geode::SolidMeshColocation3D inspector{ block.mesh() };
            auto colocated_pts =
                filter_colocated_points_with_same_uuid< 3, geode::BRep >( model,
                    block.component_id(),
                    inspector.colocated_points_groups().problems );
            if( !colocated_pts.empty() )
            {
                geode::InspectionIssues< std::vector< geode::index_t > >
                    block_issues{ "Colocated vertices on the Block with uuid "
                                  + block.id().string() };
                const auto& block_mesh = block.mesh();
                for( const auto& colocated_points_group : colocated_pts )
                {
                    std::string point_group_string{ "" };
                    for( const auto point_index : colocated_points_group )
                    {
                        absl::StrAppend(
                            &point_group_string, " ", point_index );
                    }
                    block_issues.add_problem( colocated_points_group,
                        absl::StrCat( "Block with uuid ", block.id().string(),
                            " has vertices with indices", point_group_string,
                            " which are colocated at position [",
                            block_mesh.point( colocated_points_group[0] )
                                .string(),
                            "]." ) );
                }
                components_colocated_points.emplace( block.id(), block_issues );
            }
        }
        return components_colocated_points;
    }

} // namespace

namespace geode
{
    std::string MeshesColocationInspectionResult::string() const
    {
        std::string message{ "" };
        for( const auto& issue : colocated_points_groups )
        {
            absl::StrAppend( &message, issue.second.string(), "\n" );
        }
        return message;
    }
    template < geode::index_t dimension, typename Model >
    class ComponentMeshesColocation< dimension, Model >::Impl
    {
    public:
        Impl( const Model& model ) : model_( model ) {}

        absl::flat_hash_map< uuid, InspectionIssues< std::vector< index_t > > >
            components_colocated_points_groups() const
        {
            return model_components_colocated_points_groups( model_ );
        }

    private:
        const Model& model_;
    };

    template < geode::index_t dimension, typename Model >
    ComponentMeshesColocation< dimension, Model >::ComponentMeshesColocation(
        const Model& model )
        : impl_( model )
    {
    }

    template < geode::index_t dimension, typename Model >
    ComponentMeshesColocation< dimension, Model >::~ComponentMeshesColocation()
    {
    }

    template < geode::index_t dimension, typename Model >
    MeshesColocationInspectionResult ComponentMeshesColocation< dimension,
        Model >::inspect_meshes_point_colocations() const
    {
        MeshesColocationInspectionResult result;
        result.colocated_points_groups =
            impl_->components_colocated_points_groups();
        return result;
    }

    template class opengeode_inspector_inspector_api
        ComponentMeshesColocation< 2, Section >;
    template class opengeode_inspector_inspector_api
        ComponentMeshesColocation< 3, BRep >;
} // namespace geode
