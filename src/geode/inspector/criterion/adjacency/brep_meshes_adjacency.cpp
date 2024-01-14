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
        Impl( const BRep& brep ) : ComponentMeshesAdjacency< 3, BRep >( brep )
        {
        }

        absl::flat_hash_map< uuid, InspectionIssues< PolyhedronFacet > >
            blocks_facets_with_wrong_adjacencies() const
        {
            absl::flat_hash_map< uuid, InspectionIssues< PolyhedronFacet > >
                components_wrong_adjacencies;
            for( const auto& block : model().blocks() )
            {
                const geode::SolidMeshAdjacency3D inspector{ block.mesh() };
                auto wrong_adjacencies =
                    inspector.polyhedron_facets_with_wrong_adjacency();
                if( wrong_adjacencies.number() != 0 )
                {
                    components_wrong_adjacencies.emplace(
                        block.id(), std::move( wrong_adjacencies ) );
                }
            }
            return components_wrong_adjacencies;
        }
    };

    std::string BRepMeshesAdjacencyInspectionResult::string() const
    {
        std::string message{ "" };
        for( const auto& surface_issue : surfaces_edges_with_wrong_adjacencies )
        {
            absl::StrAppend( &message, surface_issue.second.string(), "\n" );
        }
        for( const auto& block_issue : blocks_facets_with_wrong_adjacencies )
        {
            absl::StrAppend( &message, block_issue.second.string(), "\n" );
        }
        return message;
    }

    BRepComponentMeshesAdjacency::BRepComponentMeshesAdjacency(
        const BRep& model )
        : impl_{ model }
    {
    }

    BRepComponentMeshesAdjacency::~BRepComponentMeshesAdjacency() {}

    BRepMeshesAdjacencyInspectionResult
        BRepComponentMeshesAdjacency::inspect_brep_meshes_adjacencies() const
    {
        BRepMeshesAdjacencyInspectionResult result;
        result.surfaces_edges_with_wrong_adjacencies =
            impl_->surfaces_edges_with_wrong_adjacencies();
        result.blocks_facets_with_wrong_adjacencies =
            impl_->blocks_facets_with_wrong_adjacencies();
        return result;
    }
} // namespace geode
