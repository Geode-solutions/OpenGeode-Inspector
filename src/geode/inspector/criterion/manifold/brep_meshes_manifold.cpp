/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/inspector/criterion/manifold/brep_meshes_manifold.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

#include <geode/inspector/criterion/manifold/solid_edge_manifold.h>
#include <geode/inspector/criterion/manifold/solid_facet_manifold.h>
#include <geode/inspector/criterion/manifold/solid_vertex_manifold.h>
#include <geode/inspector/criterion/private/component_meshes_manifold.h>

namespace geode
{
    class BRepComponentMeshesManifold::Impl
        : public ComponentMeshesManifold< 3, BRep >
    {
    public:
        Impl( const BRep& brep ) : ComponentMeshesManifold< 3, BRep >( brep ) {}

        absl::flat_hash_map< uuid, InspectionIssues< index_t > >
            component_meshes_non_manifold_vertices() const
        {
            auto components_non_manifold_vertices = ComponentMeshesManifold< 3,
                BRep >::surfaces_meshes_non_manifold_vertices();
            for( const auto& block : model().blocks() )
            {
                const SolidMeshVertexManifold3D inspector{ block.mesh() };
                auto non_manifold_vertices = inspector.non_manifold_vertices();
                if( !non_manifold_vertices.problems.empty() )
                {
                    components_non_manifold_vertices.emplace(
                        block.id(), non_manifold_vertices );
                }
            }
            return components_non_manifold_vertices;
        }

        absl::flat_hash_map< uuid,
            InspectionIssues< std::array< index_t, 2 > > >
            component_meshes_non_manifold_edges() const
        {
            auto components_non_manifold_edges = ComponentMeshesManifold< 3,
                BRep >::surfaces_meshes_non_manifold_edges();
            for( const auto& block : model().blocks() )
            {
                const SolidMeshEdgeManifold3D inspector{ block.mesh() };
                auto non_manifold_edges = inspector.non_manifold_edges();
                if( !non_manifold_edges.problems.empty() )
                {
                    components_non_manifold_edges.emplace(
                        block.id(), non_manifold_edges );
                }
            }
            return components_non_manifold_edges;
        }

        absl::flat_hash_map< uuid, InspectionIssues< PolyhedronFacetVertices > >
            component_meshes_non_manifold_facets() const
        {
            absl::flat_hash_map< uuid,
                InspectionIssues< PolyhedronFacetVertices > >
                components_non_manifold_facets;
            for( const auto& block : model().blocks() )
            {
                const SolidMeshFacetManifold3D inspector{ block.mesh() };
                auto non_manifold_facets = inspector.non_manifold_facets();
                if( !non_manifold_facets.problems.empty() )
                {
                    components_non_manifold_facets.emplace(
                        block.id(), non_manifold_facets );
                }
            }
            return components_non_manifold_facets;
        }

        absl::flat_hash_map< std::array< index_t, 2 >,
            InspectionIssues< uuid > >
            model_non_manifold_edges() const
        {
            using Edge = detail::VertexCycle< std::array< index_t, 2 > >;
            absl::flat_hash_map< Edge, std::vector< uuid > > edges;
            for( const auto& surface : model().surfaces() )
            {
                const auto& mesh = surface.mesh();
                for( const auto p : Range{ mesh.nb_polygons() } )
                {
                    const auto vertices = mesh.polygon_vertices( p );
                    for( const auto e : LIndices{ vertices } )
                    {
                        const auto adj = mesh.polygon_adjacent( { p, e } );
                        if( !adj || adj.value() < p )
                        {
                            continue;
                        }
                        const auto v0 = model().unique_vertex(
                            { surface.component_id(), vertices[e] } );
                        const auto v1 =
                            model().unique_vertex( { surface.component_id(),
                                vertices[e == vertices.size() - 1 ? 0
                                                                  : e + 1] } );
                        const auto info = edges.try_emplace(
                            std::array< index_t, 2 >{ v0, v1 },
                            std::vector< uuid >{ surface.id() } );
                        if( !info.second )
                        {
                            info.first->second.push_back( surface.id() );
                        }
                    }
                }
            }
            absl::flat_hash_map< std::array< index_t, 2 >,
                InspectionIssues< uuid > >
                result;
            for( auto& edge : edges )
            {
                sort_unique( edge.second );
                if( edge.second.size() > 1 )
                {
                    InspectionIssues< uuid > issue{ absl::StrCat(
                        "Model edge between unique vertices ",
                        edge.first.vertices()[0], " and ",
                        edge.first.vertices()[1], " is not manifold" ) };
                    for( const auto surface_uuid : edge.second )
                    {
                        issue.add_problem( surface_uuid,
                            absl::StrCat( "Model edge between unique vertices ",
                                edge.first.vertices()[0], " and ",
                                edge.first.vertices()[1],
                                " does not belong to a line but belongs to the "
                                "surface ",
                                surface_uuid.string() ) );
                    }
                    result.emplace( std::move( edge.first.vertices() ),
                        std::move( issue ) );
                }
            }
            return result;
        }
    };

    std::string BRepMeshesManifoldInspectionResult::string() const
    {
        std::string message{ "" };
        for( const auto& vertices_issue : meshes_non_manifold_vertices )
        {
            absl::StrAppend( &message, vertices_issue.second.string(), "\n" );
        }
        for( const auto& edges_issue : meshes_non_manifold_edges )
        {
            absl::StrAppend( &message, edges_issue.second.string(), "\n" );
        }
        for( const auto& facets_issue : meshes_non_manifold_facets )
        {
            absl::StrAppend( &message, facets_issue.second.string(), "\n" );
        }
        for( const auto& model_edges_issue : model_non_manifold_edges )
        {
            absl::StrAppend(
                &message, model_edges_issue.second.string(), "\n" );
        }
        return message;
    }

    BRepComponentMeshesManifold::BRepComponentMeshesManifold(
        const BRep& model )
        : impl_{ model }
    {
    }

    BRepComponentMeshesManifold::~BRepComponentMeshesManifold() {}

    BRepMeshesManifoldInspectionResult
        BRepComponentMeshesManifold::inspect_brep_manifold() const
    {
        BRepMeshesManifoldInspectionResult result;
        result.meshes_non_manifold_vertices =
            impl_->component_meshes_non_manifold_vertices();
        result.meshes_non_manifold_edges =
            impl_->component_meshes_non_manifold_edges();
        result.meshes_non_manifold_facets =
            impl_->component_meshes_non_manifold_facets();
        result.model_non_manifold_edges = impl_->model_non_manifold_edges();
        return result;
    }

} // namespace geode
