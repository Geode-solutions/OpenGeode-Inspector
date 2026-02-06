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

#include <geode/inspector/criterion/colocation/component_meshes_colocation.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/colocation/edgedcurve_colocation.hpp>
#include <geode/inspector/criterion/colocation/solid_colocation.hpp>
#include <geode/inspector/criterion/colocation/surface_colocation.hpp>

namespace
{
    template < typename Model >
    std::vector< std::vector< geode::index_t > >
        filter_colocated_points_with_same_unique_vertex( const Model& model,
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

    template < typename Model >
    void add_model_components_colocated_points_groups_base( const Model& model,
        geode::InspectionIssuesMap< std::vector< geode::index_t > >&
            components_colocated_points )
    {
        for( const auto& line : model.active_lines() )
        {
            const geode::EdgedCurveColocation< Model::dim > inspector{
                line.mesh()
            };
            auto colocated_pts =
                filter_colocated_points_with_same_unique_vertex< Model >( model,
                    line.component_id(),
                    inspector.colocated_points_groups().issues() );
            if( !colocated_pts.empty() )
            {
                geode::InspectionIssues< std::vector< geode::index_t > >
                    line_issues{ absl::StrCat( "Line ", line.name(), " (",
                        line.id().string(), ") colocated vertices" ) };
                const auto& line_mesh = line.mesh();
                for( const auto& colocated_points_group : colocated_pts )
                {
                    std::string point_group_string;
                    for( const auto point_index : colocated_points_group )
                    {
                        absl::StrAppend(
                            &point_group_string, " ", point_index );
                    }
                    line_issues.add_issue( colocated_points_group,
                        absl::StrCat( "Line ", line.name(), " (",
                            line.id().string(), ") has vertices ",
                            point_group_string, " colocated at position [",
                            line_mesh.point( colocated_points_group[0] )
                                .string(),
                            "]" ) );
                }
                components_colocated_points.add_issues_to_map(
                    line.id(), std::move( line_issues ) );
            }
        }
        for( const auto& surface : model.active_surfaces() )
        {
            const geode::SurfaceMeshColocation< Model::dim > inspector{
                surface.mesh()
            };
            auto colocated_pts =
                filter_colocated_points_with_same_unique_vertex< Model >( model,
                    surface.component_id(),
                    inspector.colocated_points_groups().issues() );
            if( !colocated_pts.empty() )
            {
                geode::InspectionIssues< std::vector< geode::index_t > >
                    surface_issues{ absl::StrCat( "Surface ", surface.name(),
                        " (", surface.id().string(), ") colocated vertices" ) };
                const auto& surface_mesh = surface.mesh();
                for( const auto& colocated_points_group : colocated_pts )
                {
                    std::string point_group_string;
                    for( const auto point_index : colocated_points_group )
                    {
                        absl::StrAppend(
                            &point_group_string, " ", point_index );
                    }
                    surface_issues.add_issue( colocated_points_group,
                        absl::StrCat( "Surface ", surface.name(), " (",
                            surface.id().string(), ") has vertices ",
                            point_group_string, " colocated at position [",
                            surface_mesh.point( colocated_points_group[0] )
                                .string(),
                            "]" ) );
                }
                components_colocated_points.add_issues_to_map(
                    surface.id(), std::move( surface_issues ) );
            }
        }
    }

    void add_model_components_colocated_points_groups(
        const geode::Section& model,
        geode::InspectionIssuesMap< std::vector< geode::index_t > >&
            components_colocated_points )
    {
        add_model_components_colocated_points_groups_base< geode::Section >(
            model, components_colocated_points );
    }

    void add_model_components_colocated_points_groups( const geode::BRep& model,
        geode::InspectionIssuesMap< std::vector< geode::index_t > >&
            components_colocated_points )
    {
        add_model_components_colocated_points_groups_base< geode::BRep >(
            model, components_colocated_points );
        for( const auto& block : model.active_blocks() )
        {
            const geode::SolidMeshColocation3D inspector{ block.mesh() };
            auto colocated_pts =
                filter_colocated_points_with_same_unique_vertex< geode::BRep >(
                    model, block.component_id(),
                    inspector.colocated_points_groups().issues() );
            if( !colocated_pts.empty() )
            {
                geode::InspectionIssues< std::vector< geode::index_t > >
                    block_issues{ absl::StrCat( "Block ", block.name(), " (",
                        block.id().string(), ") colocated vertices" ) };
                const auto& block_mesh = block.mesh();
                for( const auto& colocated_points_group : colocated_pts )
                {
                    std::string point_group_string;
                    for( const auto point_index : colocated_points_group )
                    {
                        absl::StrAppend(
                            &point_group_string, " ", point_index );
                    }
                    block_issues.add_issue( colocated_points_group,
                        absl::StrCat( "Block ", block.name(), " (",
                            block.id().string(), ") has vertices ",
                            point_group_string, " colocated at position [",
                            block_mesh.point( colocated_points_group[0] )
                                .string(),
                            "]" ) );
                }
                components_colocated_points.add_issues_to_map(
                    block.id(), std::move( block_issues ) );
            }
        }
    }

} // namespace

namespace geode
{
    index_t MeshesColocationInspectionResult::nb_issues() const
    {
        return colocated_points_groups.nb_issues();
    }

    std::string MeshesColocationInspectionResult::string() const
    {
        if( colocated_points_groups.nb_issues() != 0 )
        {
            return colocated_points_groups.string();
        }
        return "no issues of colocation in meshes \n";
    }

    std::string MeshesColocationInspectionResult::inspection_type() const
    {
        return "points colocation inspection";
    }

    template < typename Model >
    class ComponentMeshesColocation< Model >::Impl
    {
    public:
        Impl( const Model& model ) : model_( model ) {}

        void add_components_colocated_points_groups(
            InspectionIssuesMap< std::vector< index_t > >& issues_map ) const
        {
            add_model_components_colocated_points_groups( model_, issues_map );
        }

    private:
        const Model& model_;
    };

    template < typename Model >
    ComponentMeshesColocation< Model >::ComponentMeshesColocation(
        const Model& model )
        : impl_( model )
    {
    }

    template < typename Model >
    ComponentMeshesColocation< Model >::~ComponentMeshesColocation() = default;

    template < typename Model >
    MeshesColocationInspectionResult
        ComponentMeshesColocation< Model >::inspect_meshes_point_colocations()
            const
    {
        MeshesColocationInspectionResult result;
        impl_->add_components_colocated_points_groups(
            result.colocated_points_groups );
        return result;
    }

    template class opengeode_inspector_inspector_api
        ComponentMeshesColocation< Section >;
    template class opengeode_inspector_inspector_api
        ComponentMeshesColocation< BRep >;
} // namespace geode
