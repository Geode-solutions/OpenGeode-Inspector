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

#include <geode/inspector/criterion/private/component_meshes_adjacency.h>

#include <geode/basic/logger.h>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/adjacency/surface_adjacency.h>

namespace geode
{
    template < index_t dimension, typename Model >
    ComponentMeshesAdjacency< dimension, Model >::ComponentMeshesAdjacency(
        const Model& model, bool verbose )
        : model_( model ), verbose_( verbose )
    {
    }

    template < index_t dimension, typename Model >
    std::vector< uuid > ComponentMeshesAdjacency< dimension,
        Model >::surfaces_with_wrong_adjacencies() const
    {
        std::vector< uuid > comps_with_wrong_adjacencies;
        for( const auto& surface : model_.surfaces() )
        {
            const SurfaceMeshAdjacency< dimension > inspector{ surface.mesh(),
                verbose_ };
            if( inspector.mesh_has_wrong_adjacencies() )
            {
                comps_with_wrong_adjacencies.push_back( surface.id() );
            }
        }
        return comps_with_wrong_adjacencies;
    }

    template < index_t dimension, typename Model >
    absl::flat_hash_map< uuid, index_t > ComponentMeshesAdjacency< dimension,
        Model >::surfaces_nb_edges_with_wrong_adjacencies() const
    {
        absl::flat_hash_map< uuid, index_t > components_nb_wrong_adjacencies;
        for( const auto& surface : model_.surfaces() )
        {
            const SurfaceMeshAdjacency< dimension > inspector{ surface.mesh(),
                verbose_ };
            const auto nb_wrong_adjacencies =
                inspector.nb_edges_with_wrong_adjacency();
            if( nb_wrong_adjacencies != 0 )
            {
                components_nb_wrong_adjacencies.emplace(
                    surface.id(), nb_wrong_adjacencies );
            }
        }
        return components_nb_wrong_adjacencies;
    }

    template < index_t dimension, typename Model >
    absl::flat_hash_map< uuid, std::vector< PolygonEdge > >
        ComponentMeshesAdjacency< dimension,
            Model >::surfaces_edges_with_wrong_adjacencies() const
    {
        absl::flat_hash_map< uuid, std::vector< PolygonEdge > >
            components_wrong_adjacencies;
        for( const auto& surface : model_.surfaces() )
        {
            const SurfaceMeshAdjacency< dimension > inspector{ surface.mesh(),
                verbose_ };
            auto wrong_adjacencies =
                inspector.polygon_edges_with_wrong_adjacency();
            if( !wrong_adjacencies.empty() )
            {
                components_wrong_adjacencies.emplace(
                    surface.id(), std::move( wrong_adjacencies ) );
            }
        }
        return components_wrong_adjacencies;
    }

    template < index_t dimension, typename Model >
    const Model& ComponentMeshesAdjacency< dimension, Model >::model() const
    {
        return model_;
    }

    template < index_t dimension, typename Model >
    bool ComponentMeshesAdjacency< dimension, Model >::verbose() const
    {
        return verbose_;
    }

    template class opengeode_inspector_inspector_api
        ComponentMeshesAdjacency< 2, Section >;
    template class opengeode_inspector_inspector_api
        ComponentMeshesAdjacency< 3, BRep >;
} // namespace geode
