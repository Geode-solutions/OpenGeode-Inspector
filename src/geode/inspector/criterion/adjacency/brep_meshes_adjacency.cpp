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

#include <geode/inspector/criterion/adjacency/brep_meshes_adjacency.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>

#include <geode/inspector/criterion/adjacency/solid_adjacency.h>
#include <geode/inspector/criterion/internal/component_meshes_adjacency.h>

namespace geode
{
    std::string BRepMeshesAdjacencyInspectionResult::string() const
    {
        std::string message;
        if( surfaces_edges_with_wrong_adjacencies.nb_issues() != 0 )
        {
            absl::StrAppend( &message,
                surfaces_edges_with_wrong_adjacencies.string(), "\n" );
        }
        if( blocks_facets_with_wrong_adjacencies.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, blocks_facets_with_wrong_adjacencies.string(), "\n" );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "No adjacency issues in model component meshes \n";
    }

    std::string BRepMeshesAdjacencyInspectionResult::inspection_type() const
    {
        return "Adjacencies inspection";
    }

    class BRepComponentMeshesAdjacency::Impl
        : public ComponentMeshesAdjacency< BRep >
    {
    public:
        Impl( const BRep& brep ) : ComponentMeshesAdjacency< BRep >( brep ) {}

        void add_blocks_facets_with_wrong_adjacencies(
            InspectionIssuesMap< PolyhedronFacet >&
                components_wrong_adjacencies ) const
        {
            for( const auto& block : model().blocks() )
            {
                const geode::SolidMeshAdjacency3D inspector{ block.mesh() };
                auto wrong_adjacencies =
                    inspector.polyhedron_facets_with_wrong_adjacency();
                wrong_adjacencies.set_description(
                    absl::StrCat( "Block with uuid ", block.id().string(),
                        " wrong polyhedron facets adjacencies" ) );
                components_wrong_adjacencies.add_issues_to_map(
                    block.id(), std::move( wrong_adjacencies ) );
            }
        }
    };

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
        impl_->add_surfaces_edges_with_wrong_adjacencies(
            result.surfaces_edges_with_wrong_adjacencies );
        impl_->add_blocks_facets_with_wrong_adjacencies(
            result.blocks_facets_with_wrong_adjacencies );
        return result;
    }
} // namespace geode
