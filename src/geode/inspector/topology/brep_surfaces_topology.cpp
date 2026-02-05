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

#include <geode/inspector/topology/brep_surfaces_topology.hpp>

#include <optional>

#include <absl/algorithm/container.h>

#include <geode/basic/algorithm.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/helpers/component_mesh_polygons.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/topology/internal/topology_helpers.hpp>

namespace geode
{
    index_t BRepSurfacesTopologyInspectionResult::nb_issues() const
    {
        return surfaces_not_meshed.nb_issues()
               + surfaces_not_linked_to_a_unique_vertex.nb_issues()
               + unique_vertices_linked_to_a_surface_with_invalid_embbedings
                     .nb_issues()
               + unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                     .nb_issues()
               + unique_vertices_linked_to_several_and_invalid_surfaces
                     .nb_issues();
    }

    std::string BRepSurfacesTopologyInspectionResult::string() const
    {
        std::string message;
        if( surfaces_not_meshed.nb_issues() != 0 )
        {
            absl::StrAppend( &message, surfaces_not_meshed.string() );
        }
        if( surfaces_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, surfaces_not_linked_to_a_unique_vertex.string() );
        }
        if( unique_vertices_linked_to_a_surface_with_invalid_embbedings
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_surface_with_invalid_embbedings
                    .string() );
        }
        if( unique_vertices_linked_to_several_and_invalid_surfaces.nb_issues()
            != 0 )
        {
            absl::StrAppend(
                &message, unique_vertices_linked_to_several_and_invalid_surfaces
                              .string() );
        }
        if( unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                    .string() );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "no issues with Surfaces topology \n";
    }

    std::string BRepSurfacesTopologyInspectionResult::inspection_type() const
    {
        return "Surfaces topology inspection";
    }

    BRepSurfacesTopology::BRepSurfacesTopology( const BRep& brep )
        : brep_( brep )
    {
    }

    bool BRepSurfacesTopology::brep_surfaces_topology_is_valid(
        index_t unique_vertex_index ) const
    {
        bool surface_found{ false };
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Surface3D::component_type_static()
                && brep_.surface( cmv.component_id.id() ).is_active() )
            {
                surface_found = true;
                break;
            }
        }
        if( !surface_found )
        {
            return true;
        }
        if( vertex_is_part_of_invalid_embedded_surface( unique_vertex_index )
            || vertex_is_part_of_invalid_multiple_surfaces(
                unique_vertex_index )
            || vertex_is_part_of_line_and_not_on_surface_border(
                unique_vertex_index ) )
        {
            return false;
        }
        return true;
    }

    bool BRepSurfacesTopology::surface_is_meshed(
        const Surface3D& surface ) const
    {
        const auto& surface_mesh = surface.mesh();
        return surface_mesh.nb_vertices() != 0
               && surface_mesh.nb_polygons() != 0;
    }

    bool BRepSurfacesTopology::
        surface_vertices_are_associated_to_unique_vertices(
            const Surface3D& surface ) const
    {
        return internal::
            model_component_vertices_are_associated_to_unique_vertices(
                brep_, surface.component_id(), surface.mesh() );
    }

    std::optional< std::string >
        BRepSurfacesTopology::vertex_is_part_of_invalid_embedded_surface(
            const index_t unique_vertex_index ) const
    {
        for( const auto surface_id : internal::components_uuids( brep_,
                 unique_vertex_index, Surface3D::component_type_static() ) )
        {
            const auto& surface = brep_.surface( surface_id );
            if( !surface.is_active() )
            {
                continue;
            }
            for( const auto& embedding : brep_.embeddings( surface_id ) )
            {
                const auto& block = brep_.block( embedding.id() );
                if( !block.is_active() )
                {
                    continue;
                }
                if( brep_.Relationships::is_boundary(
                        surface_id, embedding.id() ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of Surface ", surface.name(), " (",
                        surface_id.string(),
                        "), which is both internal and boundary of ", "Block ",
                        block.name(), " (", embedding.id().string(), ")" );
                }
                if( internal::brep_blocks_are_meshed( brep_ )
                    && !absl::c_any_of(
                        brep_.component_mesh_vertices( unique_vertex_index ),
                        [&embedding]( const ComponentMeshVertex& cmv ) {
                            return cmv.component_id.id() == embedding.id();
                        } ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of Surface ", surface.name(), " (",
                        surface_id.string(), "), which is embedded in Block ",
                        block.name(), " (", embedding.id().string(),
                        "), but the unique vertex is not linked to any "
                        "of the Block vertices." );
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepSurfacesTopology::vertex_is_part_of_invalid_multiple_surfaces(
            index_t unique_vertex_index ) const
    {
        const auto surface_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Surface3D::component_type_static() );
        if( absl::c_count_if( surface_uuids,
                [*this]( const uuid& surface_id ) {
                    return brep_.surface( surface_id ).is_active();
                } )
            < 2 )
        {
            return std::nullopt;
        }
        const auto line_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Line3D::component_type_static() );
        if( line_uuids.empty() )
        {
            bool has_corner_internal_to_all_surfaces{ false };
            for( const auto& cmv :
                brep_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( cmv.component_id.type()
                    != Corner3D::component_type_static() )
                {
                    continue;
                }
                const auto& corner = brep_.corner( cmv.component_id.id() );
                for( const auto& surface_id : surface_uuids )
                {
                    const auto& surface = brep_.surface( surface_id );
                    if( surface.is_active()
                        && !brep_.is_internal( corner, surface ) )
                    {
                        return absl::StrCat( "unique vertex ",
                            unique_vertex_index, " at position [",
                            corner.mesh().point( cmv.vertex ).string(),
                            "] is part of multiple active Surfaces, and not "
                            "part of any Line, but is part of Corner ",
                            corner.name(), " (", corner.id().string(),
                            "), which is not internal to active Surface ",
                            surface.name(), " (", surface_id.string(), ")." );
                    }
                }
                has_corner_internal_to_all_surfaces = true;
            }
            if( !has_corner_internal_to_all_surfaces )
            {
                return absl::StrCat( "unique vertex ", unique_vertex_index,
                    " is part of multiple active Surfaces, and not part of "
                    "any Line, but not part of any Corner internal to all "
                    "Surfaces." );
            }
        }
        if( line_uuids.size() == 1 )
        {
            index_t nb_cmv_lines{ 0 };
            index_t nb_of_line_relationships_with_surfaces{ 0 };
            for( const auto& cmv :
                brep_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( cmv.component_id.type() != Line3D::component_type_static() )
                {
                    continue;
                }
                nb_cmv_lines += 1;
                for( const auto& surface_id : surface_uuids )
                {
                    if( brep_.Relationships::is_boundary(
                            cmv.component_id.id(), surface_id )
                        || brep_.Relationships::is_internal(
                            cmv.component_id.id(), surface_id ) )
                    {
                        nb_of_line_relationships_with_surfaces += 1;
                    }
                }
            }
            if( nb_cmv_lines >= 2 )
            {
                return std::nullopt;
            }
            for( const auto& cmv :
                brep_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( cmv.component_id.type()
                    != Corner3D::component_type_static() )
                {
                    continue;
                }
                if( brep_.Relationships::nb_embeddings( cmv.component_id.id() )
                        + nb_of_line_relationships_with_surfaces
                    < surface_uuids.size() )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of multiple active Surfaces and only one "
                        "Line, is a Corner, but is not internal to all the "
                        "Surfaces without relationships to the Line." );
                }
                if( brep_.Relationships::nb_embeddings( cmv.component_id.id() )
                        + nb_of_line_relationships_with_surfaces
                    > surface_uuids.size() )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of multiple active Surfaces and only one "
                        "Line, is a Corner, but is internal to Surfaces with "
                        "relationships to the Line." );
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepSurfacesTopology::vertex_is_part_of_line_and_not_on_surface_border(
            index_t unique_vertex_index ) const
    {
        const auto line_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Line3D::component_type_static() );
        if( line_uuids.empty() )
        {
            return std::nullopt;
        }
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() != Surface3D::component_type_static() )
            {
                continue;
            }
            const auto& surface = brep_.surface( cmv.component_id.id() );
            if( surface.mesh().is_vertex_on_border( cmv.vertex ) )
            {
                continue;
            }
            for( const auto& line_id : line_uuids )
            {
                const auto& line = brep_.line( line_id );
                if( line.is_active()
                    && ( brep_.is_boundary( line, surface )
                         || brep_.is_internal( line, surface ) ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of a Line and of Surface ", surface.name(),
                        " (", cmv.component_id.id().string(),
                        ") but the associated vertex in the "
                        "Surface mesh is not on the mesh border." );
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepSurfacesTopology::surface_facet_has_wrong_component_facets_around(
            const Surface3D& surface, const index_t facet_index ) const
    {
        const auto cmp = component_mesh_polygons( brep_, surface, facet_index );
        for( const auto& [block_id, block_facets] : cmp.block_polygons )
        {
            const auto& block = brep_.block( block_id );
            if( !block.is_active() )
            {
                continue;
            }
            if( brep_.is_boundary( surface, block ) )
            {
                if( block_facets.size() != 1 )
                {
                    return absl::StrCat( "Surface ", surface.name(), " (",
                        surface.id().string(), ") is boundary of block ",
                        block.name(), " (", block_id.string(), "), but has ",
                        block_facets.size(),
                        " facets of this Block around it, it should be 1." );
                }
                continue;
            }
            if( brep_.is_internal( surface, block ) )
            {
                if( block_facets.size() != 2 )
                {
                    return absl::StrCat( "Surface ", surface.name(), " (",
                        surface.id().string(), ") is internal to block ",
                        block.name(), " (", block_id.string(), "), but has ",
                        block_facets.size(),
                        " facets of this Block around it, it should be 2." );
                }
                continue;
            }
            return absl::StrCat( "Surface ", surface.name(), " (",
                surface.id().string(), ") has facet with id ", facet_index,
                " in common with Block", block.name(), " (", block_id.string(),
                "), but is neither boundary of nor internal to it." );
        }
        return std::nullopt;
    }

    BRepSurfacesTopologyInspectionResult
        BRepSurfacesTopology::inspect_surfaces_topology() const
    {
        BRepSurfacesTopologyInspectionResult result;
        const auto meshed_blocks = internal::brep_blocks_are_meshed( brep_ );
        for( const auto& surface : brep_.active_surfaces() )
        {
            if( !surface_is_meshed( brep_.surface( surface.id() ) ) )
            {
                result.surfaces_not_meshed.add_issue( surface.id(),
                    absl::StrCat( "Surface ", surface.name(), " (",
                        surface.id().string(), ") is not meshed" ) );
            }

            auto surface_result = internal::
                model_component_vertices_not_associated_to_unique_vertices(
                    brep_, surface.component_id(), surface.mesh() );
            if( surface_result.nb_issues() != 0 )
            {
                surface_result.set_description( absl::StrCat( "Surface ",
                    surface.name(), " (", surface.id().string(), ")" ) );
                result.surfaces_not_linked_to_a_unique_vertex.add_issues_to_map(
                    surface.id(), std::move( surface_result ) );
                /// Next test may result in SegFaults if component vertices are
                /// not associated to unique vertices
                continue;
            }
            if( !meshed_blocks )
            {
                continue;
            }
            InspectionIssues< index_t > surface_facets_with_wrong_cme{
                absl::StrCat( "Surface ", surface.name(), " (",
                    surface.id().string(), ")" )
            };
            for( const auto facet_id : Range{ surface.mesh().nb_polygons() } )
            {
                if( const auto problem_message =
                        surface_facet_has_wrong_component_facets_around(
                            surface, facet_id ) )
                {
                    surface_facets_with_wrong_cme.add_issue(
                        facet_id, problem_message.value() );
                }
            }
            if( surface_facets_with_wrong_cme.nb_issues() != 0 )
            {
                result.surface_polygons_with_wrong_component_facets_around
                    .add_issues_to_map( surface.id(),
                        std::move( surface_facets_with_wrong_cme ) );
            }
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( const auto invalid_internal_topology =
                    vertex_is_part_of_invalid_embedded_surface(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_a_surface_with_invalid_embbedings
                    .add_issue(
                        unique_vertex_id, invalid_internal_topology.value() );
            }
            if( const auto invalid_multiple_surfaces =
                    vertex_is_part_of_invalid_multiple_surfaces(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_several_and_invalid_surfaces
                    .add_issue(
                        unique_vertex_id, invalid_multiple_surfaces.value() );
            }
            if( const auto line_and_not_on_surface_border =
                    vertex_is_part_of_line_and_not_on_surface_border(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                    .add_issue( unique_vertex_id,
                        line_and_not_on_surface_border.value() );
            }
        }
        return result;
    }
} // namespace geode
