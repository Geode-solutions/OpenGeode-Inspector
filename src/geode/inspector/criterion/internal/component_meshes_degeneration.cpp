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

#include <geode/inspector/criterion/internal/component_meshes_degeneration.hpp>

#include <geode/basic/logger.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace geode
{
    namespace internal
    {
        template < typename Model >
        ComponentMeshesDegeneration< Model >::ComponentMeshesDegeneration(
            const Model& model )
            : model_( model )
        {
        }

        template < typename Model >
        void ComponentMeshesDegeneration< Model >::add_small_edges(
            InspectionIssuesMap< index_t >& components_small_edges,
            double threshold ) const
        {
            for( const auto& line : model_.lines() )
            {
                if( !edged_curve_inspectors_.contains( line.id() ) )
                {
                    edged_curve_inspectors_.try_emplace(
                        line.id(), std::make_unique<
                                       EdgedCurveDegeneration< Model::dim > >(
                                       line.mesh() ) );
                }
                const auto& inspector =
                    *edged_curve_inspectors_.at( line.id() );
                auto issues = inspector.small_edges( threshold );
                issues.set_description( absl::StrCat(
                    "Line ", line.id().string(), " small edges" ) );
                components_small_edges.add_issues_to_map(
                    line.id(), std::move( issues ) );
            }
            for( const auto& surface : model_.surfaces() )
            {
                if( !surface_mesh_inspectors_.contains( surface.id() ) )
                {
                    surface_mesh_inspectors_.try_emplace( surface.id(),
                        std::make_unique<
                            SurfaceMeshDegeneration< Model::dim > >(
                            surface.mesh() ) );
                }
                const auto& inspector =
                    *surface_mesh_inspectors_.at( surface.id() );
                auto issues = inspector.small_edges( threshold );
                issues.set_description( absl::StrCat(
                    "Surface ", surface.id().string(), " small edges" ) );
                components_small_edges.add_issues_to_map(
                    surface.id(), std::move( issues ) );
            }
        }

        template < typename Model >
        void ComponentMeshesDegeneration< Model >::add_degenerated_edges(
            InspectionIssuesMap< index_t >& components_degenerated_edges ) const
        {
            add_small_edges( components_degenerated_edges, GLOBAL_EPSILON );
        }

        template < typename Model >
        void ComponentMeshesDegeneration< Model >::add_small_height_polygons(
            InspectionIssuesMap< index_t >& components_small_polygons,
            double threshold ) const
        {
            for( const auto& surface : model_.surfaces() )
            {
                if( !surface_mesh_inspectors_.contains( surface.id() ) )
                {
                    surface_mesh_inspectors_.try_emplace( surface.id(),
                        std::make_unique<
                            SurfaceMeshDegeneration< Model::dim > >(
                            surface.mesh() ) );
                }
                const auto& inspector =
                    *surface_mesh_inspectors_.at( surface.id() );
                auto issues = inspector.small_height_polygons( threshold );
                issues.set_description( absl::StrCat(
                    "Surface ", surface.id().string(), " small polygons" ) );
                components_small_polygons.add_issues_to_map(
                    surface.id(), std::move( issues ) );
            }
        }

        template < typename Model >
        void ComponentMeshesDegeneration< Model >::add_degenerated_polygons(
            InspectionIssuesMap< index_t >& components_degenerated_polygons )
            const
        {
            add_small_height_polygons(
                components_degenerated_polygons, GLOBAL_EPSILON );
        }

        template < typename Model >
        const Model& ComponentMeshesDegeneration< Model >::model() const
        {
            return model_;
        }

        template class opengeode_inspector_inspector_api
            ComponentMeshesDegeneration< Section >;
        template class opengeode_inspector_inspector_api
            ComponentMeshesDegeneration< BRep >;
    } // namespace internal
} // namespace geode
