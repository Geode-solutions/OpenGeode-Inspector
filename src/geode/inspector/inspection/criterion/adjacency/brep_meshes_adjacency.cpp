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

#include <geode/inspector/inspection/criterion/adjacency/brep_meshes_adjacency.hpp>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_set.h>

#include <async++.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/range.hpp>

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/inspection/criterion/adjacency/solid_adjacency.hpp>
#include <geode/inspector/inspection/criterion/internal/component_meshes_adjacency.hpp>

namespace geode
{
    index_t BRepMeshesAdjacencyInspectionResult::nb_issues() const
    {
        return surfaces_edges_with_wrong_adjacencies.nb_issues()
               + blocks_facets_with_wrong_adjacencies.nb_issues();
    }

    std::string BRepMeshesAdjacencyInspectionResult::string() const
    {
        std::string message;
        if( surfaces_edges_with_wrong_adjacencies.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, surfaces_edges_with_wrong_adjacencies.string() );
        }
        if( blocks_facets_with_wrong_adjacencies.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, blocks_facets_with_wrong_adjacencies.string() );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "no adjacency issues in model meshes \n";
    }

    std::string BRepMeshesAdjacencyInspectionResult::inspection_type() const
    {
        return "adjacencies inspection";
    }

    class BRepComponentMeshesAdjacency::Impl
        : public internal::ComponentMeshesAdjacency< BRep >
    {
    public:
        Impl( const BRep& brep )
            : internal::ComponentMeshesAdjacency< BRep >( brep )
        {
        }

        void add_blocks_facets_with_wrong_adjacencies(
            InspectionIssuesMap< PolyhedronFacet >&
                components_wrong_adjacencies ) const
        {
            if( model().nb_active_blocks() == 0 )
            {
                return;
            }
            const auto surface_polygons = surfaces_polygons_unique_vertices();
            std::vector< async::task<
                std::pair< uuid, InspectionIssues< PolyhedronFacet > > > >
                tasks;
            for( const auto& block : model().active_blocks() )
            {
                tasks.emplace_back( async::spawn( [this, &block,
                                                      &surface_polygons] {
                    return std::pair{ block.id(),
                        block_wrong_adjacencies( block, surface_polygons ) };
                } ) );
            }
            for( auto& task : async::when_all( tasks ).get() )
            {
                auto [block_id, issues] = task.get();
                components_wrong_adjacencies.add_issues_to_map(
                    block_id, std::move( issues ) );
            }
        }

    private:
        using SortedUniqueVertices = PolygonVertices;

        std::vector< index_t > mesh_unique_vertices(
            const ComponentID& component_id, index_t nb_vertices ) const
        {
            std::vector< index_t > unique_vertices( nb_vertices );
            for( const auto v : Indices{ unique_vertices } )
            {
                unique_vertices[v] =
                    model().unique_vertex( { component_id, v } );
            }
            return unique_vertices;
        }

        // Sorted unique vertices of every surface polygon
        absl::flat_hash_set< SortedUniqueVertices >
            surfaces_polygons_unique_vertices() const
        {
            std::vector< async::task< std::vector< SortedUniqueVertices > > >
                tasks;
            for( const auto& surface : model().surfaces() )
            {
                tasks.emplace_back( async::spawn( [this, &surface] {
                    const auto& mesh = surface.mesh();
                    const auto unique_vertices = mesh_unique_vertices(
                        surface.component_id(), mesh.nb_vertices() );
                    std::vector< SortedUniqueVertices > surface_polygons;
                    surface_polygons.reserve( mesh.nb_polygons() );
                    for( const auto polygon_id : Range{ mesh.nb_polygons() } )
                    {
                        auto vertices = mesh.polygon_vertices( polygon_id );
                        for( auto& vertex : vertices )
                        {
                            vertex = unique_vertices[vertex];
                        }
                        if( absl::c_contains( vertices, NO_ID ) )
                        {
                            continue;
                        }
                        absl::c_sort( vertices );
                        surface_polygons.emplace_back( std::move( vertices ) );
                    }
                    return surface_polygons;
                } ) );
            }
            absl::flat_hash_set< SortedUniqueVertices > polygons;
            for( auto& task : async::when_all( tasks ).get() )
            {
                for( auto& polygon : task.get() )
                {
                    polygons.emplace( std::move( polygon ) );
                }
            }
            return polygons;
        }

        InspectionIssues< PolyhedronFacet > block_wrong_adjacencies(
            const Block3D& block,
            const absl::flat_hash_set< SortedUniqueVertices >&
                surface_polygons ) const
        {
            const auto& mesh = block.mesh();
            const geode::SolidMeshAdjacency3D inspector{ mesh };
            auto wrong_adjacencies =
                inspector.polyhedron_facets_with_wrong_adjacency();
            wrong_adjacencies.set_description( absl::StrCat( "Block ",
                block.name().value_or( block.id().string() ), " (",
                block.id().string(),
                ") polyhedron facets adjacencies issues" ) );
            const auto unique_vertices = mesh_unique_vertices(
                block.component_id(), mesh.nb_vertices() );
            for( const auto polyhedron_id : Range{ mesh.nb_polyhedra() } )
            {
                for( const auto facet_id :
                    LRange{ mesh.nb_polyhedron_facets( polyhedron_id ) } )
                {
                    const PolyhedronFacet polyhedron_facet{ polyhedron_id,
                        facet_id };
                    if( !mesh.is_polyhedron_facet_on_border(
                            polyhedron_facet ) )
                    {
                        continue;
                    }
                    SortedUniqueVertices facet_unique_vertices;
                    for( const auto vertex :
                        mesh.polyhedron_facet_vertices( polyhedron_facet ) )
                    {
                        facet_unique_vertices.push_back(
                            unique_vertices[vertex] );
                    }
                    absl::c_sort( facet_unique_vertices );
                    // Facets with unlinked vertices cannot be on a Surface
                    if( absl::c_contains( facet_unique_vertices, NO_ID )
                        || !surface_polygons.contains( facet_unique_vertices ) )
                    {
                        wrong_adjacencies.add_issue( polyhedron_facet,
                            absl::StrCat( "facet ", facet_id, " of polyhedron ",
                                polyhedron_id,
                                " has no adjacencies but is not part of a "
                                "model Surface." ) );
                    }
                }
            }
            return wrong_adjacencies;
        }
    };

    BRepComponentMeshesAdjacency::BRepComponentMeshesAdjacency(
        const BRep& model )
        : impl_{ model }
    {
    }

    BRepComponentMeshesAdjacency::~BRepComponentMeshesAdjacency() = default;

    BRepMeshesAdjacencyInspectionResult
        BRepComponentMeshesAdjacency::inspect_brep_meshes_adjacencies() const
    {
        BRepMeshesAdjacencyInspectionResult result;
        async::parallel_invoke(
            [&result, this] {
                impl_->add_surfaces_edges_with_wrong_adjacencies(
                    result.surfaces_edges_with_wrong_adjacencies );
            },
            [&result, this] {
                impl_->add_blocks_facets_with_wrong_adjacencies(
                    result.blocks_facets_with_wrong_adjacencies );
            } );
        return result;
    }
} // namespace geode
