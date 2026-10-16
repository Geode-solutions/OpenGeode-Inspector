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

#include <geode/inspector/inspection/criterion/internal/component_meshes_adjacency.hpp>

#include <async++.h>

#include <geode/basic/logger.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/inspection/criterion/adjacency/surface_adjacency.hpp>

namespace
{
    // True if both unique vertices are linked to the vertices of an edge of a
    // same Line
    template < typename Model >
    bool unique_vertices_are_on_a_line_edge( const Model& model,
        geode::index_t unique_vertex0,
        geode::index_t unique_vertex1 )
    {
        if( unique_vertex0 == unique_vertex1 )
        {
            return false;
        }
        const auto& cmvs1 = model.component_mesh_vertices( unique_vertex1 );
        for( const auto& cmv0 :
            model.component_mesh_vertices( unique_vertex0 ) )
        {
            if( cmv0.component_id.type
                != geode::Line< Model::dim >::component_type_static() )
            {
                continue;
            }
            for( const auto& cmv1 : cmvs1 )
            {
                if( cmv1.component_id.id != cmv0.component_id.id )
                {
                    continue;
                }
                const auto& mesh = model.line( cmv0.component_id.id ).mesh();
                for( const auto& edge_vertex :
                    mesh.edges_around_vertex( cmv0.vertex ) )
                {
                    if( mesh.edge_vertex( edge_vertex.opposite() )
                        == cmv1.vertex )
                    {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    template < typename Model >
    geode::InspectionIssues< geode::PolygonEdge > surface_wrong_adjacencies(
        const Model& model, const geode::Surface< Model::dim >& surface )
    {
        const auto& mesh = surface.mesh();
        const geode::SurfaceMeshAdjacency< Model::dim > inspector{ mesh };
        auto issues = inspector.polygon_edges_with_wrong_adjacency();
        issues.set_description( absl::StrCat( "Surface ",
            surface.name().value_or( surface.id().string() ), " (",
            surface.id().string(), ") polygon edges adjacency issues" ) );
        std::vector< geode::index_t > unique_vertices( mesh.nb_vertices() );
        for( const auto v : geode::Indices{ unique_vertices } )
        {
            unique_vertices[v] =
                model.unique_vertex( { surface.component_id(), v } );
        }
        for( const auto polygon_id : geode::Range{ mesh.nb_polygons() } )
        {
            for( const auto edge_id :
                geode::LRange{ mesh.nb_polygon_edges( polygon_id ) } )
            {
                const geode::PolygonEdge polygon_edge{ polygon_id, edge_id };
                if( !mesh.is_edge_on_border( polygon_edge ) )
                {
                    continue;
                }
                const auto edge_vertices =
                    mesh.polygon_edge_vertices( polygon_edge );
                const auto unique_vertex0 = unique_vertices[edge_vertices[0]];
                const auto unique_vertex1 = unique_vertices[edge_vertices[1]];
                // Vertices without (valid) unique vertex cannot be on a Line
                if( unique_vertex0 >= model.nb_unique_vertices()
                    || unique_vertex1 >= model.nb_unique_vertices()
                    || !unique_vertices_are_on_a_line_edge(
                        model, unique_vertex0, unique_vertex1 ) )
                {
                    issues.add_issue( polygon_edge,
                        absl::StrCat( "edge ", edge_id, " of polygon ",
                            polygon_id,
                            " has no adjacencies but is not part of a model "
                            "Line." ) );
                }
            }
        }
        return issues;
    }
} // namespace

namespace geode
{
    namespace internal
    {
        template < typename Model >
        ComponentMeshesAdjacency< Model >::ComponentMeshesAdjacency(
            const Model& model )
            : model_( model )
        {
        }

        template < typename Model >
        void ComponentMeshesAdjacency< Model >::
            add_surfaces_edges_with_wrong_adjacencies(
                InspectionIssuesMap< PolygonEdge >&
                    components_wrong_adjacencies ) const
        {
            std::vector< async::task<
                std::pair< uuid, InspectionIssues< PolygonEdge > > > >
                tasks;
            for( const auto& surface : model_.active_surfaces() )
            {
                tasks.emplace_back( async::spawn( [this, &surface] {
                    return std::pair{ surface.id(),
                        surface_wrong_adjacencies( model_, surface ) };
                } ) );
            }
            for( auto& task : async::when_all( tasks ).get() )
            {
                auto [surface_id, issues] = task.get();
                components_wrong_adjacencies.add_issues_to_map(
                    surface_id, std::move( issues ) );
            }
        }

        template < typename Model >
        const Model& ComponentMeshesAdjacency< Model >::model() const
        {
            return model_;
        }

        template class opengeode_inspector_inspection_api
            ComponentMeshesAdjacency< Section >;
        template class opengeode_inspector_inspection_api
            ComponentMeshesAdjacency< BRep >;
    } // namespace internal
} // namespace geode
