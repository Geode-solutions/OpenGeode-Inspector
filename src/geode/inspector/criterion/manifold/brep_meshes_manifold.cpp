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
        Impl( const BRep& brep, bool verbose )
            : ComponentMeshesManifold< 3, BRep >( brep, verbose )
        {
        }

        std::vector< uuid > components_non_manifold_meshes() const
        {
            auto non_manifold_components = ComponentMeshesManifold< 3,
                BRep >::surfaces_non_manifold_meshes();
            for( const auto& block : model().blocks() )
            {
                const SolidMeshVertexManifold3D vertex_inspector{ block.mesh(),
                    verbose() };
                if( !vertex_inspector.mesh_vertices_are_manifold() )
                {
                    non_manifold_components.push_back( block.id() );
                    continue;
                }
                const SolidMeshEdgeManifold3D edge_inspector{ block.mesh(),
                    verbose() };
                if( !edge_inspector.mesh_edges_are_manifold() )
                {
                    non_manifold_components.push_back( block.id() );
                    continue;
                }
                const SolidMeshFacetManifold3D facet_inspector{ block.mesh(),
                    verbose() };
                if( !facet_inspector.mesh_facets_are_manifold() )
                {
                    non_manifold_components.push_back( block.id() );
                }
            }
            const auto& model_edges = model_non_manifold_edges();
            if( !model_edges.empty() )
            {
                for( const auto& model_edge : model_edges )
                {
                    for( const auto& component_id : model_edge.second )
                    {
                        non_manifold_components.push_back( component_id );
                    }
                }
            }
            sort_unique( non_manifold_components );
            return non_manifold_components;
        }

        absl::flat_hash_map< uuid, index_t >
            component_meshes_nb_non_manifold_vertices() const
        {
            auto components_nb_non_manifold_vertices =
                ComponentMeshesManifold< 3,
                    BRep >::surfaces_meshes_nb_non_manifold_vertices();
            for( const auto& block : model().blocks() )
            {
                const SolidMeshVertexManifold3D inspector{ block.mesh(),
                    verbose() };
                auto nb_non_manifold_vertices =
                    inspector.nb_non_manifold_vertices();
                if( nb_non_manifold_vertices != 0 )
                {
                    components_nb_non_manifold_vertices.emplace(
                        block.id(), nb_non_manifold_vertices );
                }
            }
            return components_nb_non_manifold_vertices;
        }

        absl::flat_hash_map< uuid, index_t >
            component_meshes_nb_non_manifold_edges() const
        {
            auto components_nb_non_manifold_edges = ComponentMeshesManifold< 3,
                BRep >::surfaces_meshes_nb_non_manifold_edges();
            for( const auto& block : model().blocks() )
            {
                const SolidMeshEdgeManifold3D inspector{ block.mesh(),
                    verbose() };
                auto nb_non_manifold_edges = inspector.nb_non_manifold_edges();
                if( nb_non_manifold_edges != 0 )
                {
                    components_nb_non_manifold_edges.emplace(
                        block.id(), nb_non_manifold_edges );
                }
            }
            return components_nb_non_manifold_edges;
        }

        absl::flat_hash_map< uuid, index_t >
            component_meshes_nb_non_manifold_facets() const
        {
            absl::flat_hash_map< uuid, index_t >
                components_nb_non_manifold_facets;
            for( const auto& block : model().blocks() )
            {
                const SolidMeshFacetManifold3D inspector{ block.mesh(),
                    verbose() };
                auto nb_non_manifold_facets =
                    inspector.nb_non_manifold_facets();
                if( nb_non_manifold_facets != 0 )
                {
                    components_nb_non_manifold_facets.emplace(
                        block.id(), nb_non_manifold_facets );
                }
            }
            return components_nb_non_manifold_facets;
        }

        absl::flat_hash_map< uuid, std::vector< index_t > >
            component_meshes_non_manifold_vertices() const
        {
            auto components_non_manifold_vertices = ComponentMeshesManifold< 3,
                BRep >::surfaces_meshes_non_manifold_vertices();
            for( const auto& block : model().blocks() )
            {
                const SolidMeshVertexManifold3D inspector{ block.mesh(),
                    verbose() };
                auto non_manifold_vertices = inspector.non_manifold_vertices();
                if( !non_manifold_vertices.empty() )
                {
                    components_non_manifold_vertices.emplace(
                        block.id(), non_manifold_vertices );
                }
            }
            return components_non_manifold_vertices;
        }

        absl::flat_hash_map< uuid, std::vector< std::array< index_t, 2 > > >
            component_meshes_non_manifold_edges() const
        {
            auto components_non_manifold_edges = ComponentMeshesManifold< 3,
                BRep >::surfaces_meshes_non_manifold_edges();
            for( const auto& block : model().blocks() )
            {
                const SolidMeshEdgeManifold3D inspector{ block.mesh(),
                    verbose() };
                auto non_manifold_edges = inspector.non_manifold_edges();
                if( !non_manifold_edges.empty() )
                {
                    components_non_manifold_edges.emplace(
                        block.id(), non_manifold_edges );
                }
            }
            return components_non_manifold_edges;
        }

        absl::flat_hash_map< uuid, std::vector< PolyhedronFacetVertices > >
            component_meshes_non_manifold_facets() const
        {
            absl::flat_hash_map< uuid, std::vector< PolyhedronFacetVertices > >
                components_non_manifold_facets;
            for( const auto& block : model().blocks() )
            {
                const SolidMeshFacetManifold3D inspector{ block.mesh(),
                    verbose() };
                auto non_manifold_facets = inspector.non_manifold_facets();
                if( !non_manifold_facets.empty() )
                {
                    components_non_manifold_facets.emplace(
                        block.id(), non_manifold_facets );
                }
            }
            return components_non_manifold_facets;
        }

        absl::flat_hash_map< std::array< index_t, 2 >, std::vector< uuid > >
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
            absl::flat_hash_map< std::array< index_t, 2 >, std::vector< uuid > >
                result;
            for( auto& edge : edges )
            {
                sort_unique( edge.second );
                if( edge.second.size() > 1 )
                {
                    result.emplace( std::move( edge.first.vertices() ),
                        std::move( edge.second ) );
                }
            }
            if( verbose() )
            {
                for( const auto& edge : result )
                {
                    std::string uuid_group_string{ "" };
                    for( const auto surface_uuid : edge.second )
                    {
                        absl::StrAppend(
                            &uuid_group_string, " ", surface_uuid.string() );
                    }
                    geode::Logger::info( "Model edge between unique vertices ",
                        edge.first[0], " and ", edge.first[1],
                        " is not manifold: it does not belong to a line but "
                        "belongs to surfaces",
                        uuid_group_string );
                }
            }
            return result;
        }
    };

    BRepComponentMeshesManifold::BRepComponentMeshesManifold(
        const BRep& model )
        : impl_{ model, false }
    {
    }

    BRepComponentMeshesManifold::BRepComponentMeshesManifold(
        const BRep& model, bool verbose )
        : impl_{ model, verbose }
    {
    }

    BRepComponentMeshesManifold::~BRepComponentMeshesManifold() {}

    std::vector< uuid >
        BRepComponentMeshesManifold::components_non_manifold_meshes() const
    {
        return impl_->components_non_manifold_meshes();
    }

    absl::flat_hash_map< uuid, index_t >
        BRepComponentMeshesManifold::component_meshes_nb_non_manifold_vertices()
            const
    {
        return impl_->component_meshes_nb_non_manifold_vertices();
    }

    absl::flat_hash_map< uuid, index_t >
        BRepComponentMeshesManifold::component_meshes_nb_non_manifold_edges()
            const
    {
        return impl_->component_meshes_nb_non_manifold_edges();
    }

    absl::flat_hash_map< uuid, index_t >
        BRepComponentMeshesManifold::component_meshes_nb_non_manifold_facets()
            const
    {
        return impl_->component_meshes_nb_non_manifold_facets();
    }

    absl::flat_hash_map< uuid, std::vector< index_t > >
        BRepComponentMeshesManifold::component_meshes_non_manifold_vertices()
            const
    {
        return impl_->component_meshes_non_manifold_vertices();
    }

    absl::flat_hash_map< uuid, std::vector< std::array< index_t, 2 > > >
        BRepComponentMeshesManifold::component_meshes_non_manifold_edges() const
    {
        return impl_->component_meshes_non_manifold_edges();
    }

    absl::flat_hash_map< uuid, std::vector< PolyhedronFacetVertices > >
        BRepComponentMeshesManifold::component_meshes_non_manifold_facets()
            const
    {
        return impl_->component_meshes_non_manifold_facets();
    }

    absl::flat_hash_map< std::array< index_t, 2 >, std::vector< uuid > >
        BRepComponentMeshesManifold::model_non_manifold_edges() const
    {
        return impl_->model_non_manifold_edges();
    }
} // namespace geode
