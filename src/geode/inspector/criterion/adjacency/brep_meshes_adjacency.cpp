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

#include <geode/inspector/criterion/adjacency/brep_meshes_adjacency.hpp>

#include <async++.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/helpers/component_mesh_polygons.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/criterion/adjacency/solid_adjacency.hpp>
#include <geode/inspector/criterion/internal/component_meshes_adjacency.hpp>

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
            for( const auto& block : model().blocks() )
            {
                const geode::SolidMeshAdjacency3D inspector{ block.mesh() };
                auto wrong_adjacencies =
                    inspector.polyhedron_facets_with_wrong_adjacency();
                wrong_adjacencies.set_description( absl::StrCat( "Block ",
                    block.name(), " (", block.id().string(),
                    ") polyhedron facets adjacencies issues" ) );
                const auto& mesh = block.mesh();
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
                        const auto is_on_surface =
                            polyhedron_facet_is_on_a_surface(
                                block, polyhedron_facet );
                        if( is_on_surface.has_value()
                            && !is_on_surface.value() )
                        {
                            wrong_adjacencies.add_issue( polyhedron_facet,
                                absl::StrCat( "facet ", facet_id,
                                    " of polyhedron ", polyhedron_id,
                                    " has no adjacencies but is not part of a "
                                    "model Surface." ) );
                        }
                    }
                }
                components_wrong_adjacencies.add_issues_to_map(
                    block.id(), std::move( wrong_adjacencies ) );
            }
        }

    private:
        absl::optional< bool > polyhedron_facet_is_on_a_surface(
            const Block3D& block,
            const PolyhedronFacet& polyhedron_facet ) const
        {
            const auto facet_unique_vertices =
                polygon_unique_vertices( model(), block, polyhedron_facet );
            if( absl::c_contains( facet_unique_vertices, geode::NO_ID ) )
            {
                return std::nullopt;
            }
            return !detail::surface_component_mesh_polygons(
                model(), facet_unique_vertices )
                        .empty();
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
