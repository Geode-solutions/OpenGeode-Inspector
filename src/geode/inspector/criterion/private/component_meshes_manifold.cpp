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

#include <geode/inspector/criterion/private/component_meshes_manifold.h>

#include <geode/basic/logger.h>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/manifold/surface_edge_manifold.h>
#include <geode/inspector/criterion/manifold/surface_vertex_manifold.h>

namespace geode
{
    template < index_t dimension, typename Model >
    ComponentMeshesManifold< dimension, Model >::ComponentMeshesManifold(
        const Model& model, bool verbose )
        : model_( model ), verbose_( verbose )
    {
    }

    template < index_t dimension, typename Model >
    std::vector< uuid > ComponentMeshesManifold< dimension,
        Model >::surfaces_non_manifold_meshes() const
    {
        std::vector< uuid > non_manifold_surfaces;
        for( const auto& surface : model_.surfaces() )
        {
            const SurfaceMeshVertexManifold< dimension > v_inspector{
                surface.mesh(), verbose_
            };
            if( !v_inspector.mesh_vertices_are_manifold() )
            {
                non_manifold_surfaces.push_back( surface.id() );
                continue;
            }
            const SurfaceMeshEdgeManifold< dimension > e_inspector{
                surface.mesh(), verbose_
            };
            if( !e_inspector.mesh_edges_are_manifold() )
            {
                non_manifold_surfaces.push_back( surface.id() );
            }
        }
        return non_manifold_surfaces;
    }

    template < index_t dimension, typename Model >
    absl::flat_hash_map< uuid, index_t > ComponentMeshesManifold< dimension,
        Model >::surfaces_meshes_nb_non_manifold_vertices() const
    {
        absl::flat_hash_map< uuid, index_t > surfaces_nb_non_manifold_vertices;
        for( const auto& surface : model_.surfaces() )
        {
            const SurfaceMeshVertexManifold< dimension > inspector{
                surface.mesh(), verbose_
            };
            const auto nb_non_manifold_vertices =
                inspector.nb_non_manifold_vertices();
            if( nb_non_manifold_vertices != 0 )
            {
                surfaces_nb_non_manifold_vertices.emplace(
                    surface.id(), nb_non_manifold_vertices );
            }
        }
        return surfaces_nb_non_manifold_vertices;
    }

    template < index_t dimension, typename Model >
    absl::flat_hash_map< uuid, index_t > ComponentMeshesManifold< dimension,
        Model >::surfaces_meshes_nb_non_manifold_edges() const
    {
        absl::flat_hash_map< uuid, index_t > surfaces_nb_non_manifold_edges;
        for( const auto& surface : model_.surfaces() )
        {
            const SurfaceMeshEdgeManifold< dimension > inspector{
                surface.mesh(), verbose_
            };
            const auto nb_non_manifold_edges =
                inspector.nb_non_manifold_edges();
            if( nb_non_manifold_edges != 0 )
            {
                surfaces_nb_non_manifold_edges.emplace(
                    surface.id(), nb_non_manifold_edges );
            }
        }
        return surfaces_nb_non_manifold_edges;
    }

    template < index_t dimension, typename Model >
    absl::flat_hash_map< uuid, std::vector< index_t > >
        ComponentMeshesManifold< dimension,
            Model >::surfaces_meshes_non_manifold_vertices() const
    {
        absl::flat_hash_map< uuid, std::vector< index_t > >
            surfaces_non_manifold_vertices;
        for( const auto& surface : model_.surfaces() )
        {
            const SurfaceMeshVertexManifold< dimension > inspector{
                surface.mesh(), verbose_
            };
            auto non_manifold_vertices = inspector.non_manifold_vertices();
            if( !non_manifold_vertices.empty() )
            {
                surfaces_non_manifold_vertices.emplace(
                    surface.id(), std::move( non_manifold_vertices ) );
            }
        }
        return surfaces_non_manifold_vertices;
    }

    template < index_t dimension, typename Model >
    absl::flat_hash_map< uuid, std::vector< std::array< index_t, 2 > > >
        ComponentMeshesManifold< dimension,
            Model >::surfaces_meshes_non_manifold_edges() const
    {
        absl::flat_hash_map< uuid, std::vector< std::array< index_t, 2 > > >
            surfaces_non_manifold_edges;
        for( const auto& surface : model_.surfaces() )
        {
            const SurfaceMeshEdgeManifold< dimension > inspector{
                surface.mesh(), verbose_
            };
            auto non_manifold_edges = inspector.non_manifold_edges();
            if( !non_manifold_edges.empty() )
            {
                surfaces_non_manifold_edges.emplace(
                    surface.id(), std::move( non_manifold_edges ) );
            }
        }
        return surfaces_non_manifold_edges;
    }

    template < index_t dimension, typename Model >
    const Model& ComponentMeshesManifold< dimension, Model >::model() const
    {
        return model_;
    }

    template < index_t dimension, typename Model >
    bool ComponentMeshesManifold< dimension, Model >::verbose() const
    {
        return verbose_;
    }

    template class opengeode_inspector_inspector_api
        ComponentMeshesManifold< 2, Section >;
    template class opengeode_inspector_inspector_api
        ComponentMeshesManifold< 3, BRep >;
} // namespace geode
