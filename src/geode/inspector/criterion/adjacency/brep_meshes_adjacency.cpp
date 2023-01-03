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

#include <geode/inspector/criterion/adjacency/brep_meshes_adjacency.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>

#include <geode/inspector/criterion/adjacency/solid_adjacency.h>
#include <geode/inspector/criterion/private/component_meshes_adjacency.h>

namespace geode
{
    class BRepComponentMeshesAdjacency::Impl
        : public ComponentMeshesAdjacency< 3, BRep >
    {
    public:
        Impl( const BRep& brep, bool verbose )
            : ComponentMeshesAdjacency< 3, BRep >( brep, verbose )
        {
        }

        std::vector< uuid > components_with_wrong_adjacencies() const
        {
            auto comps_with_wrong_adjacencies =
                this->surfaces_with_wrong_adjacencies();
            for( const auto& block : model().blocks() )
            {
                const geode::SolidMeshAdjacency3D inspector{ block.mesh(),
                    verbose() };
                if( inspector.mesh_has_wrong_adjacencies() )
                {
                    comps_with_wrong_adjacencies.push_back( block.id() );
                }
            }
            return comps_with_wrong_adjacencies;
        }

        absl::flat_hash_map< uuid, index_t >
            blocks_nb_facets_with_wrong_adjacencies() const
        {
            absl::flat_hash_map< uuid, index_t >
                components_nb_wrong_adjacencies;
            for( const auto& block : model().blocks() )
            {
                const geode::SolidMeshAdjacency3D inspector{ block.mesh(),
                    verbose() };
                const auto nb_wrong_adjacencies =
                    inspector.nb_facets_with_wrong_adjacency();
                if( nb_wrong_adjacencies != 0 )
                {
                    components_nb_wrong_adjacencies.emplace(
                        block.id(), nb_wrong_adjacencies );
                }
            }
            return components_nb_wrong_adjacencies;
        }

        absl::flat_hash_map< uuid, std::vector< PolyhedronFacet > >
            blocks_facets_with_wrong_adjacencies() const
        {
            absl::flat_hash_map< uuid, std::vector< PolyhedronFacet > >
                components_wrong_adjacencies;
            for( const auto& block : model().blocks() )
            {
                const geode::SolidMeshAdjacency3D inspector{ block.mesh(),
                    verbose() };
                auto wrong_adjacencies =
                    inspector.polyhedron_facets_with_wrong_adjacency();
                if( !wrong_adjacencies.empty() )
                {
                    components_wrong_adjacencies.emplace(
                        block.id(), std::move( wrong_adjacencies ) );
                }
            }
            return components_wrong_adjacencies;
        }
    };

    BRepComponentMeshesAdjacency::BRepComponentMeshesAdjacency(
        const BRep& model )
        : impl_{ model, false }
    {
    }

    BRepComponentMeshesAdjacency::BRepComponentMeshesAdjacency(
        const BRep& model, bool verbose )
        : impl_{ model, verbose }
    {
    }

    BRepComponentMeshesAdjacency::~BRepComponentMeshesAdjacency() {}

    std::vector< uuid >
        BRepComponentMeshesAdjacency::components_with_wrong_adjacencies() const
    {
        return impl_->components_with_wrong_adjacencies();
    }

    absl::flat_hash_map< uuid, index_t >
        BRepComponentMeshesAdjacency::surfaces_nb_edges_with_wrong_adjacencies()
            const
    {
        return impl_->surfaces_nb_edges_with_wrong_adjacencies();
    }

    absl::flat_hash_map< uuid, std::vector< PolygonEdge > >
        BRepComponentMeshesAdjacency::surfaces_edges_with_wrong_adjacencies()
            const
    {
        return impl_->surfaces_edges_with_wrong_adjacencies();
    }

    absl::flat_hash_map< uuid, index_t >
        BRepComponentMeshesAdjacency::blocks_nb_facets_with_wrong_adjacencies()
            const
    {
        return impl_->blocks_nb_facets_with_wrong_adjacencies();
    }

    absl::flat_hash_map< uuid, std::vector< PolyhedronFacet > >
        BRepComponentMeshesAdjacency::blocks_facets_with_wrong_adjacencies()
            const
    {
        return impl_->blocks_facets_with_wrong_adjacencies();
    }
} // namespace geode
