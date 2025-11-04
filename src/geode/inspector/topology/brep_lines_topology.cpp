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

#include <geode/inspector/topology/brep_lines_topology.hpp>

#include <optional>

#include <absl/algorithm/container.h>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/model/helpers/component_mesh_edges.hpp>
#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/topology/internal/topology_helpers.hpp>

namespace geode
{
    index_t BRepLinesTopologyInspectionResult::nb_issues() const
    {
        return lines_not_meshed.nb_issues()
               + lines_not_linked_to_a_unique_vertex.nb_issues()
               + unique_vertices_linked_to_a_line_with_invalid_embeddings
                     .nb_issues()
               + unique_vertices_linked_to_a_single_and_invalid_line.nb_issues()
               + unique_vertices_linked_to_line_with_wrong_relationship_to_surface
                     .nb_issues()
               + unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                     .nb_issues();
    }

    std::string BRepLinesTopologyInspectionResult::string() const
    {
        std::string message;
        if( lines_not_meshed.nb_issues() != 0 )
        {
            absl::StrAppend( &message, lines_not_meshed.string() );
        }
        if( lines_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            absl::StrAppend(
                &message, lines_not_linked_to_a_unique_vertex.string() );
        }
        if( unique_vertices_linked_to_a_line_with_invalid_embeddings.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_line_with_invalid_embeddings
                    .string() );
        }
        if( unique_vertices_linked_to_a_single_and_invalid_line.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_single_and_invalid_line.string(),
                "\n" );
        }
        if( unique_vertices_linked_to_line_with_wrong_relationship_to_surface
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_line_with_wrong_relationship_to_surface
                    .string(),
                "\n" );
        }
        if( unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                    .string() );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "no issues with lines topology \n";
    }

    std::string BRepLinesTopologyInspectionResult::inspection_type() const
    {
        return "Lines topology inspection";
    }

    BRepLinesTopology::BRepLinesTopology( const BRep& brep ) : brep_( brep ) {}

    bool BRepLinesTopology::brep_lines_topology_is_valid(
        index_t unique_vertex_index ) const
    {
        bool part_of_line{ false };
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Line3D::component_type_static() )
            {
                part_of_line = true;
                break;
            }
        }
        if( !part_of_line )
        {
            return true;
        }
        if( vertex_is_part_of_invalid_embedded_line( unique_vertex_index )
            || vertex_has_lines_but_is_not_a_corner( unique_vertex_index ) )
        {
            return false;
        }
        return true;
    }

    bool BRepLinesTopology::line_is_meshed( const Line3D& line ) const
    {
        const auto& line_mesh = line.mesh();
        return line_mesh.nb_vertices() != 0 && line_mesh.nb_edges() != 0;
    }

    bool BRepLinesTopology::line_vertices_are_associated_to_unique_vertices(
        const Line3D& line ) const
    {
        return internal::
            model_component_vertices_are_associated_to_unique_vertices(
                brep_, line.component_id(), line.mesh() );
    }

    std::optional< std::string >
        BRepLinesTopology::vertex_is_part_of_invalid_embedded_line(
            index_t unique_vertex_index ) const
    {
        for( const auto& line_cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( line_cmv.component_id.type()
                != Line3D::component_type_static() )
            {
                continue;
            }
            const auto line_id = line_cmv.component_id.id();
            for( const auto& embedding : brep_.embeddings( line_id ) )
            {
                if( brep_.Relationships::is_boundary(
                        line_id, embedding.id() ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of line ", brep_.line( line_id ).name(), " (",
                        line_id.string(),
                        "), which is both boundary and embedded in "
                        "Surface ",
                        brep_.surface( embedding.id() ).name(), " (",
                        embedding.id().string() + ")." );
                }
                if( embedding.type() == Block3D::component_type_static()
                    && !internal::brep_blocks_are_meshed( brep_ ) )
                {
                    continue;
                }
                if( !absl::c_any_of(
                        brep_.component_mesh_vertices( unique_vertex_index ),
                        [&embedding]( const ComponentMeshVertex& cmv ) {
                            return cmv.component_id == embedding;
                        } ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of Line ", brep_.line( line_id ).name(), " (",
                        line_id.string(), "), which is embedded in Surface ",
                        brep_.surface( embedding.id() ).name(), " (",
                        embedding.id().string(),
                        "), but the unique vertex is not linked to the "
                        "Surface mesh vertices." );
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepLinesTopology::vertex_is_part_of_invalid_single_line(
            index_t unique_vertex_index ) const
    {
        const auto line_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Line3D::component_type_static() );
        if( line_uuids.size() != 1 )
        {
            return std::nullopt;
        }
        const auto& line_id = line_uuids[0];
        const auto surface_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Surface3D::component_type_static() );
        const auto block_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Block3D::component_type_static() );
        if( brep_.nb_embedding_surfaces( brep_.line( line_id ) ) < 1
            && brep_.nb_incidences( line_id ) < 1 )
        {
            return std::nullopt;
        }
        if( surface_uuids.size() == 1 )
        {
            if( !brep_.Relationships::is_internal( line_id, surface_uuids[0] )
                && !( brep_.Relationships::nb_embeddings( surface_uuids[0] ) > 0
                      || brep_.Relationships::is_boundary(
                          line_id, surface_uuids[0] ) ) )
            {
                return absl::StrCat( "unique vertex ", unique_vertex_index,
                    " is part of only one Line ", brep_.line( line_id ).name(),
                    " (", line_id.string(), "), and only one Surface ",
                    brep_.surface( surface_uuids[0] ).name(), " (",
                    surface_uuids[0].string(),
                    "), but the Line is neither embedded in the "
                    "Surface, nor boundary of the Surface while the "
                    "Surface is embedded in a Block." );
            }
        }
        else if( surface_uuids.empty() )
        {
            if( !internal::brep_blocks_are_meshed( brep_ ) )
            {
                return std::nullopt;
            }
            if( block_uuids.size() != 1 )
            {
                return absl::StrCat( "unique vertex ", unique_vertex_index,
                    " is part of only one Line ", brep_.line( line_id ).name(),
                    " (", line_id.string(), "), no Surfaces, but is part of ",
                    block_uuids.size(), " Blocks, instead of one." );
            }
            if( !brep_.Relationships::is_internal( line_id, block_uuids[0] ) )
            {
                return absl::StrCat( "unique vertex ", unique_vertex_index,
                    " is part of only one Line ", brep_.line( line_id ).name(),
                    " (", line_id.string(),
                    "), no Surfaces, one Block, but the Line is not "
                    "internal to the Block." );
            }
        }
        else
        {
            for( const auto& surface_id : surface_uuids )
            {
                if( !brep_.Relationships::is_boundary( line_id, surface_id )
                    && !brep_.Relationships::is_internal(
                        line_id, surface_id ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of only one Line ",
                        brep_.line( line_id ).name(), " (", line_id.string(),
                        "), and multiple Surfaces, but the Line is "
                        "neither internal nor boundary of Surface ",
                        brep_.surface( surface_id ).name(), " (",
                        surface_id.string(), "), in which the vertex is." );
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string > BRepLinesTopology::
        vertex_is_part_of_line_with_wrong_relationships_to_surface(
            index_t unique_vertex_index ) const
    {
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() != Line3D::component_type_static() )
            {
                continue;
            }
            if( brep_.line( cmv.component_id.id() ).mesh().nb_edges() == 0 )
            {
                continue;
            }
            const auto cme = geode::component_mesh_edges(
                brep_, brep_.line( cmv.component_id.id() ), 0 );
            for( const auto& incident_surface :
                brep_.incidences( brep_.line( cmv.component_id.id() ) ) )
            {
                if( !cme.surface_edges.contains( incident_surface.id() ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of Line ",
                        brep_.line( cmv.component_id.id() ).name(), " (",
                        cmv.component_id.id().string(),
                        "), which should be boundary of Surface ",
                        brep_.surface( incident_surface.id() ).name(), " (",
                        incident_surface.id().string(), ")" );
                }
                const auto& surface_edges =
                    cme.surface_edges.at( incident_surface.id() );
                if( surface_edges.size() != 1 )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of Line ",
                        brep_.line( cmv.component_id.id() ).name(), " (",
                        cmv.component_id.id().string(),
                        "), which should not be boundary of Surface ",
                        brep_.surface( incident_surface.id() ).name(), " (",
                        incident_surface.id().string(), ")" );
                }
            }
            for( const auto& embedding_surface : brep_.embedding_surfaces(
                     brep_.line( cmv.component_id.id() ) ) )
            {
                if( !cme.surface_edges.contains( embedding_surface.id() ) )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of Line ",
                        brep_.line( cmv.component_id.id() ).name(), " (",
                        cmv.component_id.id().string(),
                        "', which should be embedded in Surface ",
                        brep_.surface( embedding_surface.id() ).name(), " (",
                        embedding_surface.id().string(), "'" );
                }
                const auto& surface_edges =
                    cme.surface_edges.at( embedding_surface.id() );
                if( surface_edges.size() <= 1 )
                {
                    return absl::StrCat( "unique vertex ", unique_vertex_index,
                        " is part of Line",
                        brep_.line( cmv.component_id.id() ).name(), " (",
                        cmv.component_id.id().string(),
                        ", which should not be embedded in Surface ",
                        brep_.surface( embedding_surface.id() ).name(), " (",
                        embedding_surface.id().string(), ")" );
                }
            }
            if( brep_.nb_incidences( cmv.component_id.id() ) < 1
                && brep_.nb_embedding_surfaces(
                       brep_.line( cmv.component_id.id() ) )
                       < 1 )
            {
                for( const auto& surface : brep_.surfaces() )
                {
                    if( cme.surface_edges.contains( surface.id() ) )
                    {
                        return absl::StrCat( "unique vertex ",
                            unique_vertex_index, " is part of Line ",
                            brep_.line( cmv.component_id.id() ).name(), " (",
                            cmv.component_id.id().string(),
                            "), which should be internal or boundary to "
                            "Surface ",
                            brep_.surface( surface.id() ).name(), " (",
                            surface.id().string(), ")" );
                    }
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepLinesTopology::vertex_has_lines_but_is_not_a_corner(
            index_t unique_vertex_index ) const
    {
        index_t nb_lines{ 0 };
        bool is_a_corner{ false };
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Corner3D::component_type_static() )
            {
                is_a_corner = true;
            }
            else if( cmv.component_id.type()
                     == Line3D::component_type_static() )
            {
                nb_lines++;
            }
        }
        if( nb_lines > 1 && !is_a_corner )
        {
            return absl::StrCat( "nique vertex ", unique_vertex_index,
                " is part of multiple Lines but is not a Corner." );
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepLinesTopology::line_edge_has_wrong_component_edges_around(
            const Line3D& line, const index_t edge_index ) const
    {
        const auto cme = component_mesh_edges( brep_, line, edge_index );
        for( const auto& [surface_id, surface_edges] : cme.surface_edges )
        {
            if( brep_.is_boundary( line, brep_.surface( surface_id ) ) )
            {
                if( surface_edges.size() != 1 )
                {
                    return absl::StrCat( "Line ", line.name(), " (",
                        line.id().string(), ") is boundary of Surface ",
                        brep_.surface( surface_id ).name(), " (",
                        surface_id.string(), "), but has ",
                        surface_edges.size(),
                        " edges of this Surface around it, it should be 1." );
                }
                continue;
            }
            if( brep_.is_internal( line, brep_.surface( surface_id ) ) )
            {
                if( surface_edges.size() != 2 )
                {
                    return absl::StrCat( "Line with uuid '", line.id().string(),
                        "' is internal to surface with uuid '",
                        surface_id.string(), "', but has ",
                        surface_edges.size(),
                        " edges of this surface around it, it should be 2." );
                }
                continue;
            }
            return absl::StrCat( "Line with uuid '", line.id().string(),
                "' has edge with id ", edge_index,
                " in common with surface with uuid '", surface_id.string(),
                "', but is neither boundary of nor internal to it." );
        }
        for( const auto& [block_id, block_edges] : cme.block_edges )
        {
            if( brep_.is_internal( line, brep_.block( block_id ) ) )
            {
                if( block_edges.size() != 1 )
                {
                    return absl::StrCat( "Line ", line.name(), " (",
                        line.id().string(), ") is internal to Block ",
                        brep_.block( block_id ).name(), " (", block_id.string(),
                        "), but has ", block_edges.size(),
                        " edges of this Surface around it, it should be 1." );
                }
                continue;
            }
        }
        return std::nullopt;
    }

    BRepLinesTopologyInspectionResult
        BRepLinesTopology::inspect_lines_topology() const
    {
        BRepLinesTopologyInspectionResult result;
        for( const auto& line : brep_.lines() )
        {
            if( !line_is_meshed( brep_.line( line.id() ) ) )
            {
                result.lines_not_meshed.add_issue(
                    line.id(), absl::StrCat( "Line ", line.name(), " (",
                                   line.id().string(), ") is not meshed" ) );
            }
            auto line_result = internal::
                model_component_vertices_not_associated_to_unique_vertices(
                    brep_, line.component_id(), line.mesh() );
            if( line_result.nb_issues() != 0 )
            {
                line_result.set_description( absl::StrCat(
                    "Line ", line.name(), " (", line.id().string(), ")" ) );
                result.lines_not_linked_to_a_unique_vertex.add_issues_to_map(
                    line.id(), std::move( line_result ) );
                /// Next test may result in SegFaults if component vertices are
                /// not associated to unique vertices
                continue;
            }
            InspectionIssues< index_t > line_edges_with_wrong_cme{ absl::StrCat(
                "Line ", line.name(), " (", line.id().string(), ")" ) };
            for( const auto edge_id : Range{ line.mesh().nb_edges() } )
            {
                if( const auto problem_message =
                        line_edge_has_wrong_component_edges_around(
                            line, edge_id ) )
                {
                    line_edges_with_wrong_cme.add_issue(
                        edge_id, problem_message.value() );
                }
            }
            if( line_edges_with_wrong_cme.nb_issues() != 0 )
            {
                result.line_edges_with_wrong_component_edges_around
                    .add_issues_to_map(
                        line.id(), std::move( line_edges_with_wrong_cme ) );
            }
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( const auto invalid_internal_topology =
                    vertex_is_part_of_invalid_embedded_line(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_a_line_with_invalid_embeddings
                    .add_issue(
                        unique_vertex_id, invalid_internal_topology.value() );
            }
            if( const auto problem_message =
                    vertex_is_part_of_line_with_wrong_relationships_to_surface(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_line_with_wrong_relationship_to_surface
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto problem_message =
                    vertex_is_part_of_invalid_single_line( unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_a_single_and_invalid_line
                    .add_issue( unique_vertex_id, problem_message.value() );
            }
            if( const auto lines_but_is_not_corner =
                    vertex_has_lines_but_is_not_a_corner( unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                    .add_issue(
                        unique_vertex_id, lines_but_is_not_corner.value() );
            }
        }
        return result;
    }
} // namespace geode
