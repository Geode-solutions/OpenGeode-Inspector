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

#include <geode/inspector/inspection/criterion/colocation/component_meshes_colocation.hpp>

#include <absl/container/flat_hash_set.h>

#include <async++.h>

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

#include <geode/inspector/inspection/criterion/colocation/edgedcurve_colocation.hpp>
#include <geode/inspector/inspection/criterion/colocation/solid_colocation.hpp>
#include <geode/inspector/inspection/criterion/colocation/surface_colocation.hpp>

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
            // Keep one point per unique vertex; points without unique vertex
            // are always kept
            absl::flat_hash_set< geode::index_t > unique_vertices;
            const auto first_uv =
                model.unique_vertex( { component_id, point_group[0] } );
            if( first_uv != geode::NO_ID )
            {
                unique_vertices.insert( first_uv );
            }
            std::vector< geode::index_t > colocated_points;
            for( const auto pt_id : geode::Range{ 1, point_group.size() } )
            {
                const auto uv =
                    model.unique_vertex( { component_id, point_group[pt_id] } );
                if( uv == geode::NO_ID || unique_vertices.insert( uv ).second )
                {
                    colocated_points.push_back( point_group[pt_id] );
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

    template < typename Inspector, typename Model, typename Component >
    std::optional< geode::InspectionIssues< std::vector< geode::index_t > > >
        component_colocated_points_groups(
            const Model& model, const Component& component )
    {
        const auto& mesh = component.mesh();
        const Inspector inspector{ mesh };
        const auto colocated_pts =
            filter_colocated_points_with_same_unique_vertex< Model >( model,
                component.component_id(),
                inspector.colocated_points_groups().issues() );
        if( colocated_pts.empty() )
        {
            return std::nullopt;
        }
        const auto& id = component.id();
        const auto label = absl::StrCat( component.component_type().get(), " ",
            component.name() ? component.name().value() : id.string(), " (",
            id.string(), ")" );
        std::optional<
            geode::InspectionIssues< std::vector< geode::index_t > > >
            issues{ std::in_place,
                absl::StrCat( label, " colocated vertices" ) };
        for( const auto& colocated_points_group : colocated_pts )
        {
            std::string point_group_string;
            for( const auto point_index : colocated_points_group )
            {
                absl::StrAppend( &point_group_string, " ", point_index );
            }
            issues->add_issue( colocated_points_group,
                absl::StrCat( label, " has vertices ", point_group_string,
                    " colocated at position [",
                    mesh.point( colocated_points_group[0] ).string(), "]" ) );
        }
        return issues;
    }

    template < typename Inspector, typename Model, typename Range >
    void add_components_colocated_points_groups( const Model& model,
        const Range& components,
        geode::InspectionIssuesMap< std::vector< geode::index_t > >&
            components_colocated_points )
    {
        using Issues = std::optional<
            geode::InspectionIssues< std::vector< geode::index_t > > >;
        std::vector< async::task< std::pair< geode::uuid, Issues > > > tasks;
        for( const auto& component : components )
        {
            tasks.emplace_back( async::spawn( [&model, &component] {
                return std::pair{ component.id(),
                    component_colocated_points_groups< Inspector >(
                        model, component ) };
            } ) );
        }
        for( auto& task : async::when_all( tasks ).get() )
        {
            auto [id, issues] = task.get();
            if( issues )
            {
                components_colocated_points.add_issues_to_map(
                    id, std::move( issues.value() ) );
            }
        }
    }

    template < typename Model >
    void add_model_components_colocated_points_groups_base( const Model& model,
        geode::InspectionIssuesMap< std::vector< geode::index_t > >&
            components_colocated_points )
    {
        add_components_colocated_points_groups<
            geode::EdgedCurveColocation< Model::dim > >(
            model, model.active_lines(), components_colocated_points );
        add_components_colocated_points_groups<
            geode::SurfaceMeshColocation< Model::dim > >(
            model, model.active_surfaces(), components_colocated_points );
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
        add_components_colocated_points_groups< geode::SolidMeshColocation3D >(
            model, model.active_blocks(), components_colocated_points );
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

    template class opengeode_inspector_inspection_api
        ComponentMeshesColocation< Section >;
    template class opengeode_inspector_inspection_api
        ComponentMeshesColocation< BRep >;
} // namespace geode
