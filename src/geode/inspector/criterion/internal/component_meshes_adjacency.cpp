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

#include <geode/inspector/criterion/internal/component_meshes_adjacency.hpp>

#include <geode/basic/logger.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/component_mesh_edges.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/adjacency/surface_adjacency.hpp>

namespace
{
    template < typename Model >
    bool polygon_edge_is_on_a_line( const Model& model,
        const geode::Surface< Model::dim >& surface,
        const geode::PolygonEdge& polygon_edge )
    {
        const auto edge_unique_vertices =
            geode::edge_unique_vertices( model, surface, polygon_edge );
        OPENGEODE_EXCEPTION( edge_unique_vertices[0] != geode::NO_ID
                                 && edge_unique_vertices[1] != geode::NO_ID,
            "[ComponentMeshesAdjacency] Missing unique_vertices" );
        OPENGEODE_EXCEPTION(
            edge_unique_vertices[0] < model.nb_unique_vertices()
                && edge_unique_vertices[1] < model.nb_unique_vertices(),
            "[ComponentMeshesAdjacency] Wrong unique_vertices" );
        return !geode::detail::line_component_mesh_edges(
            model, edge_unique_vertices )
                    .empty();
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
            for( const auto& surface : model_.active_surfaces() )
            {
                const SurfaceMeshAdjacency< Model::dim > inspector{
                    surface.mesh()
                };
                auto issues = inspector.polygon_edges_with_wrong_adjacency();
                issues.set_description( absl::StrCat( "Surface ",
                    surface.name(), " (", surface.id().string(),
                    ") polygon edges adjacency issues" ) );
                const auto& mesh = surface.mesh();
                for( const auto polygon_id : Range{ mesh.nb_polygons() } )
                {
                    for( const auto edge_id :
                        LRange{ mesh.nb_polygon_edges( polygon_id ) } )
                    {
                        const PolygonEdge polygon_edge{ polygon_id, edge_id };
                        try
                        {
                            if( mesh.is_edge_on_border( polygon_edge )
                                && !polygon_edge_is_on_a_line(
                                    model_, surface, polygon_edge ) )
                            {
                                issues.add_issue( polygon_edge,
                                    absl::StrCat( "edge ", edge_id,
                                        " of polygon ", polygon_id,
                                        " has no adjacencies but is not part "
                                        "of a model Line." ) );
                            }
                        }
                        catch( const OpenGeodeException& e )
                        {
                            Logger::warn( e.what() );
                            issues.add_issue( polygon_edge,
                                absl::StrCat( "edge ", edge_id, " of polygon ",
                                    polygon_id,
                                    " has no adjacencies but is not part of a "
                                    "model Line." ) );
                        }
                    }
                }
                components_wrong_adjacencies.add_issues_to_map(
                    surface.id(), std::move( issues ) );
            }
        }

        template < typename Model >
        const Model& ComponentMeshesAdjacency< Model >::model() const
        {
            return model_;
        }

        template class opengeode_inspector_inspector_api
            ComponentMeshesAdjacency< Section >;
        template class opengeode_inspector_inspector_api
            ComponentMeshesAdjacency< BRep >;
    } // namespace internal
} // namespace geode
