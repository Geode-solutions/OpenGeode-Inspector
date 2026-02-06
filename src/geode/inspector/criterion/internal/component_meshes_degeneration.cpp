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

#include <geode/inspector/criterion/internal/component_meshes_degeneration.hpp>

#include <async++.h>

#include <geode/basic/logger.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/degeneration/edgedcurve_degeneration.hpp>
#include <geode/inspector/criterion/degeneration/surface_degeneration.hpp>

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
        ComponentMeshesDegeneration< Model >::~ComponentMeshesDegeneration()
        {
            for( const auto& surface_id : enabled_edges_surfaces_ )
            {
                model_.surface( surface_id ).mesh().disable_edges();
            }
        }

        template < typename Model >
        void ComponentMeshesDegeneration< Model >::add_small_edges(
            InspectionIssuesMap< index_t >& components_small_edges,
            double threshold ) const
        {
            std::vector<
                async::task< std::pair< uuid, InspectionIssues< index_t > > > >
                line_tasks;
            line_tasks.reserve( model_.nb_active_lines() );
            for( const auto& line : model_.active_lines() )
            {
                line_tasks.emplace_back( async::spawn( [&threshold, &line] {
                    const EdgedCurveDegeneration< Model::dim > inspector{
                        line.mesh()
                    };
                    auto issues = inspector.small_edges( threshold );
                    issues.set_description( absl::StrCat( "Line ", line.name(),
                        " (", line.id().string(), ") small edges" ) );
                    return std::make_pair( line.id(), std::move( issues ) );
                } ) );
            }
            for( auto& task :
                async::when_all( line_tasks.begin(), line_tasks.end() ).get() )
            {
                auto [line_id, issues] = task.get();
                components_small_edges.add_issues_to_map(
                    line_id, std::move( issues ) );
            }
            std::vector<
                async::task< std::pair< uuid, InspectionIssues< index_t > > > >
                surface_tasks;
            surface_tasks.reserve( model_.nb_active_surfaces() );
            const auto surfaces_to_enable = surfaces_on_which_enable_edges();
            for( const auto& surface : model_.active_surfaces() )
            {
                const auto enable_edges =
                    absl::c_contains( surfaces_to_enable, surface.id() );
                surface_tasks.emplace_back(
                    async::spawn( [&enable_edges, &threshold, &surface] {
                        if( enable_edges )
                        {
                            surface.mesh().enable_edges();
                        }
                        const geode::SurfaceMeshDegeneration< Model::dim >
                            inspector{ surface.mesh() };
                        auto issues = inspector.small_edges( threshold );
                        issues.set_description( absl::StrCat( "Surface ",
                            surface.name(), " (", surface.id().string(),
                            ") small facet edges" ) );
                        return std::make_pair(
                            surface.id(), std::move( issues ) );
                    } ) );
            }
            for( auto& task :
                async::when_all( surface_tasks.begin(), surface_tasks.end() )
                    .get() )
            {
                auto [surface_id, issues] = task.get();
                components_small_edges.add_issues_to_map(
                    surface_id, std::move( issues ) );
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
            std::vector<
                async::task< std::pair< uuid, InspectionIssues< index_t > > > >
                tasks;
            tasks.reserve( model_.nb_active_surfaces() );
            for( const auto& surface : model_.active_surfaces() )
            {
                tasks.emplace_back( async::spawn( [&threshold, &surface] {
                    const geode::SurfaceMeshDegeneration< Model::dim >
                        inspector{ surface.mesh() };
                    auto issues = inspector.small_height_polygons( threshold );
                    issues.set_description( absl::StrCat( "Surface ",
                        surface.name(), " (", surface.id().string(),
                        ") small height polygons" ) );
                    return std::make_pair( surface.id(), std::move( issues ) );
                } ) );
            }
            for( auto& task :
                async::when_all( tasks.begin(), tasks.end() ).get() )
            {
                auto [surface_id, issues] = task.get();
                components_small_polygons.add_issues_to_map(
                    surface_id, std::move( issues ) );
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

        template < typename Model >
        std::vector< uuid > ComponentMeshesDegeneration<
            Model >::surfaces_on_which_enable_edges() const
        {
            std::vector< uuid > result;
            for( const auto& surface : model_.active_surfaces() )
            {
                const auto& mesh = surface.mesh();
                if( !mesh.are_edges_enabled()
                    && !enabled_edges_surfaces_.contains( surface.id() ) )
                {
                    result.emplace_back( surface.id() );
                    enabled_edges_surfaces_.emplace( surface.id() );
                }
            }
            return result;
        }

        template class opengeode_inspector_inspector_api
            ComponentMeshesDegeneration< Section >;
        template class opengeode_inspector_inspector_api
            ComponentMeshesDegeneration< BRep >;
    } // namespace internal
} // namespace geode
