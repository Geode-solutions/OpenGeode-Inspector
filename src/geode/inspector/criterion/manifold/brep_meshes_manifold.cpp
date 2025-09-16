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

#include <geode/inspector/criterion/manifold/brep_meshes_manifold.hpp>

#include <geode/basic/algorithm.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/detail/vertex_cycle.hpp>
#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/component_mesh_polygons.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/criterion/internal/component_meshes_manifold.hpp>
#include <geode/inspector/criterion/manifold/solid_edge_manifold.hpp>
#include <geode/inspector/criterion/manifold/solid_facet_manifold.hpp>
#include <geode/inspector/criterion/manifold/solid_vertex_manifold.hpp>

namespace geode
{
    index_t BRepMeshesManifoldInspectionResult::nb_issues() const
    {
        return meshes_non_manifold_vertices.nb_issues()
               + meshes_non_manifold_edges.nb_issues()
               + meshes_non_manifold_facets.nb_issues()
               + brep_non_manifold_edges.nb_issues()
               + brep_non_manifold_facets.nb_issues();
    }

    std::string BRepMeshesManifoldInspectionResult::string() const
    {
        std::string message;
        if( meshes_non_manifold_vertices.nb_issues() != 0 )
        {
            absl::StrAppend( &message, meshes_non_manifold_vertices.string() );
        }
        if( meshes_non_manifold_edges.nb_issues() != 0 )
        {
            absl::StrAppend( &message, meshes_non_manifold_edges.string() );
        }
        if( meshes_non_manifold_facets.nb_issues() != 0 )
        {
            absl::StrAppend( &message, meshes_non_manifold_facets.string() );
        }
        if( brep_non_manifold_edges.nb_issues() != 0 )
        {
            absl::StrAppend( &message, brep_non_manifold_edges.string() );
        }
        if( brep_non_manifold_facets.nb_issues() != 0 )
        {
            absl::StrAppend( &message, brep_non_manifold_facets.string() );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "No manifold issues in model component meshes \n";
    }

    std::string BRepMeshesManifoldInspectionResult::inspection_type() const
    {
        return "Manifold inspection";
    }

    class BRepComponentMeshesManifold::Impl
        : public internal::ComponentMeshesManifold< BRep >
    {
    public:
        Impl( const BRep& brep )
            : internal::ComponentMeshesManifold< BRep >( brep )
        {
        }

        void add_component_meshes_non_manifold_vertices(
            InspectionIssuesMap< index_t >& components_non_manifold_vertices )
            const
        {
            ComponentMeshesManifold< BRep >::
                add_surfaces_meshes_non_manifold_vertices(
                    components_non_manifold_vertices );
            for( const auto& block : model().blocks() )
            {
                const SolidMeshVertexManifold3D inspector{ block.mesh() };
                auto non_manifold_vertices = inspector.non_manifold_vertices();
                non_manifold_vertices.set_description( absl::StrCat(
                    "Block ", block.id().string(), " non manifold vertices" ) );
                components_non_manifold_vertices.add_issues_to_map(
                    block.id(), std::move( non_manifold_vertices ) );
            }
        }

        void add_component_meshes_non_manifold_edges(
            InspectionIssuesMap< std::array< index_t, 2 > >&
                components_non_manifold_edges ) const
        {
            ComponentMeshesManifold< BRep >::
                add_surfaces_meshes_non_manifold_edges(
                    components_non_manifold_edges );
            for( const auto& block : model().blocks() )
            {
                const SolidMeshEdgeManifold3D inspector{ block.mesh() };
                auto non_manifold_edges = inspector.non_manifold_edges();
                non_manifold_edges.set_description( absl::StrCat(
                    "Block ", block.id().string(), " non manifold edges" ) );
                components_non_manifold_edges.add_issues_to_map(
                    block.id(), non_manifold_edges );
            }
        }

        void add_component_meshes_non_manifold_facets(
            InspectionIssuesMap< PolyhedronFacetVertices >&
                components_non_manifold_facets ) const
        {
            for( const auto& block : model().blocks() )
            {
                const SolidMeshFacetManifold3D inspector{ block.mesh() };
                auto non_manifold_facets = inspector.non_manifold_facets();
                non_manifold_facets.set_description( absl::StrCat(
                    "Block ", block.id().string(), " non manifold facets" ) );
                components_non_manifold_facets.add_issues_to_map(
                    block.id(), non_manifold_facets );
            }
        }

        void add_model_non_manifold_edges(
            InspectionIssues< BRepNonManifoldEdge >& issues ) const
        {
            using Edge = detail::VertexCycle< std::array< index_t, 2 > >;
            absl::flat_hash_map< Edge, std::vector< uuid > > edges;
            for( const auto& surface : model().surfaces() )
            {
                const auto& mesh = surface.mesh();
                for( const auto polygon_id : Range{ mesh.nb_polygons() } )
                {
                    const auto vertices = mesh.polygon_vertices( polygon_id );
                    for( const auto edge_id : LIndices{ vertices } )
                    {
                        const auto adj =
                            mesh.polygon_adjacent( { polygon_id, edge_id } );
                        if( !adj || adj.value() < polygon_id )
                        {
                            continue;
                        }
                        const auto v0 = model().unique_vertex(
                            { surface.component_id(), vertices[edge_id] } );
                        const auto v1 =
                            model().unique_vertex( { surface.component_id(),
                                vertices[edge_id == vertices.size() - 1
                                             ? 0
                                             : edge_id + 1] } );
                        const auto info = edges.try_emplace(
                            Edge{ std::array< index_t, 2 >{ v0, v1 } },
                            std::vector< uuid >{ surface.id() } );
                        if( !info.second )
                        {
                            info.first->second.push_back( surface.id() );
                        }
                    }
                }
            }
            for( auto& edge : edges )
            {
                sort_unique( edge.second );
                if( edge.second.size() <= 1 )
                {
                    continue;
                }
                std::string message = absl::StrCat(
                    "Model edge between unique vertices ",
                    edge.first.vertices()[0], " and ", edge.first.vertices()[1],
                    " is not manifold: it does not belong to a line "
                    "but is on surfaces " );
                for( const auto surface_uuid : edge.second )
                {
                    absl::StrAppend( &message, surface_uuid.string(), ", " );
                }
                issues.add_issue(
                    BRepNonManifoldEdge{ edge.first.vertices(), edge.second },
                    message );
            }
            for( const auto& line : model().lines() )
            {
                const auto& mesh = line.mesh();
                if( mesh.nb_edges() != 1
                    || model().nb_embedding_surfaces( line ) == 0 )
                {
                    continue;
                }
                std::array< index_t, 2 > edge_unique_vertices;
                for( const auto edge_vertex : LRange{ 2 } )
                {
                    edge_unique_vertices[edge_vertex] =
                        model().unique_vertex( { line.component_id(),
                            mesh.edge_vertex( { 0, edge_vertex } ) } );
                }
                if( several_cmvs_on_one_vertex( edge_unique_vertices ) )
                {
                    continue;
                }
                std::string message = absl::StrCat(
                    "Model edge between unique vertices ",
                    edge_unique_vertices[0], " and ", edge_unique_vertices[1],
                    " is not manifold: it belongs to internal line ",
                    line.id().string(), " with only one edge" );
                issues.add_issue(
                    BRepNonManifoldEdge{ edge_unique_vertices, { line.id() } },
                    message );
            }
        }

        void add_model_non_manifold_facets(
            InspectionIssues< BRepNonManifoldFacet >& issues ) const
        {
            for( const auto& surface : model().surfaces() )
            {
                const auto& mesh = surface.mesh();
                if( mesh.nb_polygons() != 1
                    || model().nb_embedding_blocks( surface ) == 0 )
                {
                    continue;
                }
                auto facet_vertices =
                    polygon_unique_vertices( model(), surface, 0 );
                if( several_cmvs_on_one_vertex( facet_vertices ) )
                {
                    continue;
                }
                std::string message = "Model facet between unique vertices ";
                for( const auto polygon_vertex : facet_vertices )
                {
                    absl::StrAppend( &message, polygon_vertex, " " );
                }
                absl::StrAppend( &message,
                    " is not manifold: it belongs to internal surface ",
                    surface.id().string(), " with only one facet" );
                issues.add_issue(
                    BRepNonManifoldFacet{ facet_vertices, { surface.id() } },
                    message );
            }
        }

    private:
        bool several_cmvs_on_one_vertex(
            absl::Span< const index_t > unique_vertices ) const
        {
            std::vector< uuid > surfaces;
            for( const auto unique_vertex : unique_vertices )
            {
                if( model().component_mesh_vertices( unique_vertex ).size()
                    > 1 )
                {
                    return true;
                }
            }
            return false;
        }
    };

    BRepComponentMeshesManifold::BRepComponentMeshesManifold(
        const BRep& model )
        : impl_{ model }
    {
    }

    BRepComponentMeshesManifold::~BRepComponentMeshesManifold() = default;

    BRepMeshesManifoldInspectionResult
        BRepComponentMeshesManifold::inspect_brep_manifold() const
    {
        BRepMeshesManifoldInspectionResult result;
        impl_->add_component_meshes_non_manifold_vertices(
            result.meshes_non_manifold_vertices );
        impl_->add_component_meshes_non_manifold_edges(
            result.meshes_non_manifold_edges );
        impl_->add_component_meshes_non_manifold_facets(
            result.meshes_non_manifold_facets );
        impl_->add_model_non_manifold_edges( result.brep_non_manifold_edges );
        impl_->add_model_non_manifold_facets( result.brep_non_manifold_facets );
        return result;
    }
} // namespace geode
