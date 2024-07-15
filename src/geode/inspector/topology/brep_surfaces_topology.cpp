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

#include <geode/inspector/topology/brep_surfaces_topology.h>

#include <optional>

#include <absl/algorithm/container.h>

#include <geode/basic/algorithm.h>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

#include <geode/inspector/topology/internal/topology_helpers.h>

namespace
{
    bool line_is_boundary_of_at_least_two_surfaces_or_one_embedding_surface(
        const geode::BRep& brep,
        const geode::uuid& line_uuid,
        const std::vector< geode::uuid >& surface_uuids )
    {
        geode::local_index_t counter{ 0 };
        for( const auto& surface_id : surface_uuids )
        {
            if( brep.Relationships::is_boundary( line_uuid, surface_id ) )
            {
                if( brep.nb_embeddings( surface_id ) > 0 )
                {
                    return true;
                }
                counter++;
                if( counter > 1 )
                {
                    return true;
                }
            }
        }
        return false;
    }
} // namespace

namespace geode
{

    std::string BRepSurfacesTopologyInspectionResult::string() const
    {
        std::string message;
        if( surfaces_not_meshed.nb_issues() != 0 )
        {
            absl::StrAppend( &message, surfaces_not_meshed.string(), "\n" );
        }
        if( surfaces_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            absl::StrAppend( &message,
                surfaces_not_linked_to_a_unique_vertex.string(), "\n" );
        }
        if( unique_vertices_linked_to_not_internal_nor_boundary_surface
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_not_internal_nor_boundary_surface
                    .string(),
                "\n" );
        }
        if( unique_vertices_linked_to_a_surface_with_invalid_embbedings
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_surface_with_invalid_embbedings
                    .string(),
                "\n" );
        }
        if( unique_vertices_linked_to_a_single_and_invalid_surface.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_single_and_invalid_surface.string(),
                "\n" );
        }
        if( unique_vertices_linked_to_several_and_invalid_surfaces.nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_several_and_invalid_surfaces.string(),
                "\n" );
        }
        if( unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                .nb_issues()
            != 0 )
        {
            absl::StrAppend( &message,
                unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                    .string(),
                "\n" );
        }
        if( !message.empty() )
        {
            return message;
        }
        return "No issues with surfaces topology \n";
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
            if( cmv.component_id.type() == Surface3D::component_type_static() )
            {
                surface_found = true;
                break;
            }
        }
        if( !surface_found )
        {
            return true;
        }
        if( vertex_is_part_of_not_internal_nor_boundary_surface(
                unique_vertex_index )
            || vertex_is_part_of_invalid_embedded_surface( unique_vertex_index )
            || vertex_is_part_of_invalid_single_surface( unique_vertex_index )
            || vertex_is_part_of_invalid_multiple_surfaces(
                unique_vertex_index )
            || vertex_is_part_of_line_and_not_on_surface_border(
                unique_vertex_index ) )
        {
            return false;
        }
        return true;
    }

    std::optional< std::string > BRepSurfacesTopology::
        vertex_is_part_of_not_internal_nor_boundary_surface(
            index_t unique_vertex_index ) const
    {
        for( const auto& cmv :
            brep_.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type() == Surface3D::component_type_static()
                && brep_.nb_embeddings( cmv.component_id.id() ) < 1
                && brep_.nb_incidences( cmv.component_id.id() ) < 1 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " is part of surface with uuid '",
                    cmv.component_id.id().string(),
                    "', which is neither internal to nor a boundary of "
                    "a block." );
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepSurfacesTopology::vertex_is_part_of_invalid_embedded_surface(
            const index_t unique_vertex_index ) const
    {
        for( const auto surface_id : internal::components_uuids( brep_,
                 unique_vertex_index, Surface3D::component_type_static() ) )
        {
            for( const auto& embedding : brep_.embeddings( surface_id ) )
            {
                if( brep_.Relationships::is_boundary(
                        surface_id, embedding.id() ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index, " is part of surface with uuid '",
                        surface_id.string(),
                        "', which is both internal and boundary of ",
                        "block with uuid '", embedding.id().string(), "'." );
                }
                if( internal::brep_blocks_are_meshed( brep_ )
                    && !absl::c_any_of(
                        brep_.component_mesh_vertices( unique_vertex_index ),
                        [&embedding]( const ComponentMeshVertex& cmv ) {
                            return cmv.component_id.id() == embedding.id();
                        } ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index, " is part of surface with uuid '",
                        surface_id.string(),
                        "', which is embedded in block with uuid '",
                        embedding.id().string(),
                        "', but the unique vertex is not linked to any "
                        "of the block vertices." );
                }
            }
        }
        return std::nullopt;
    }

    std::optional< std::string >
        BRepSurfacesTopology::vertex_is_part_of_invalid_single_surface(
            index_t unique_vertex_index ) const
    {
        const auto surface_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Surface3D::component_type_static() );
        if( surface_uuids.size() != 1 )
        {
            return std::nullopt;
        }
        const auto& surface_id = surface_uuids[0];
        const auto block_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Block3D::component_type_static() );
        if( block_uuids.size() > 2 )
        {
            return absl::StrCat( "Unique vertex with index ",
                unique_vertex_index,
                " is part of only one surface, but is part of more "
                "than two blocks." );
        }
        if( brep_.nb_embeddings( surface_id ) > 0 )
        {
            if( internal::brep_blocks_are_meshed( brep_ ) )
            {
                if( block_uuids.size() != 1 )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one surface, which is "
                        "embedded, but not part of only one block." );
                }
                else if( !brep_.Relationships::is_internal(
                             surface_id, block_uuids[0] ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one surface, which is "
                        "embedded, and one block, but the surface is "
                        "not internal to the block." );
                }
            }
        }
        else
        {
            for( const auto& block_id : block_uuids )
            {
                if( !brep_.Relationships::is_boundary( surface_id, block_id ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of only one surface, with uuid'",
                        surface_id.string(),
                        "' which is not embedded, but not boundary "
                        "either of block with uuid '",
                        block_id.string(), "', in which the vertex is." );
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
        if( surface_uuids.size() < 2 )
        {
            return std::nullopt;
        }
        const auto line_uuids = internal::components_uuids(
            brep_, unique_vertex_index, Line3D::component_type_static() );
        if( line_uuids.empty() )
        {
            return absl::StrCat( "Unique vertex with index ",
                unique_vertex_index,
                " is part of multiple surfaces, but not part of any "
                "line." );
        }
        if( line_uuids.size() == 1 )
        {
            bool corner_found{ false };
            index_t nb_cmv_lines{ 0 };
            for( const auto& cmv :
                brep_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( cmv.component_id.type() == Line3D::component_type_static() )
                {
                    nb_cmv_lines += 1;
                }
                else if( cmv.component_id.type()
                         == Corner3D::component_type_static() )
                {
                    corner_found = true;
                }
            }
            if( corner_found && nb_cmv_lines < 2 )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index,
                    " is part of multiple surfaces and only one line, "
                    "but is a corner." );
            }
            for( const auto& surface_id : surface_uuids )
            {
                if( !brep_.Relationships::is_boundary(
                        line_uuids[0], surface_id )
                    && !brep_.Relationships::is_internal(
                        line_uuids[0], surface_id ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of multiple surfaces and only one "
                        "line, with uuid'",
                        line_uuids[0].string(), "', but surface with uuid '",
                        surface_id.string(),
                        "', in which the vertex is, neither incident to nor "
                        "embedding of the line." );
                }
            }
        }
        else
        {
            for( const auto& line_id : line_uuids )
            {
                if( brep_.nb_embeddings( line_id ) < 1
                    && !line_is_boundary_of_at_least_two_surfaces_or_one_embedding_surface(
                        brep_, line_id, surface_uuids ) )
                {
                    return absl::StrCat( "Unique vertex with index ",
                        unique_vertex_index,
                        " is part of multiple surfaces and multiple "
                        "lines, but line with uuid'",
                        line_id.string(),
                        "' is neither internal, nor a boundary of at "
                        "least two surfaces or one embedding "
                        "surface." );
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
            if( !surface.mesh().is_vertex_on_border( cmv.vertex ) )
            {
                for( const auto& line_id : line_uuids )
                {
                    const auto& line = brep_.line( line_id );
                    if( brep_.is_boundary( line, surface )
                        || brep_.is_internal( line, surface ) )
                    {
                        return absl::StrCat( "Unique vertex with index ",
                            unique_vertex_index,
                            " is part of a line and of surface with "
                            "uuid '",
                            cmv.component_id.id().string(),
                            "' but the associated vertex in the "
                            "surface mesh is not on the mesh border." );
                    }
                }
            }
        }
        return std::nullopt;
    }
    BRepSurfacesTopologyInspectionResult
        BRepSurfacesTopology::inspect_surfaces_topology() const
    {
        BRepSurfacesTopologyInspectionResult result;
        for( const auto& surface : brep_.surfaces() )
        {
            if( brep_.surface( surface.id() ).mesh().nb_vertices() == 0 )
            {
                result.surfaces_not_meshed.add_issue(
                    surface.id(), absl::StrCat( surface.id().string(),
                                      " is a surface without mesh." ) );
            }

            auto surface_result = internal::
                brep_component_vertices_not_associated_to_unique_vertices(
                    brep_, surface.component_id(), surface.mesh() );
            if( surface_result.nb_issues() != 0 )
            {
                surface_result.set_description(
                    absl::StrCat( "Surface ", surface.id().string() ) );
                result.surfaces_not_linked_to_a_unique_vertex.add_issues_to_map(
                    surface.id(), std::move( surface_result ) );
            }
        }
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( const auto not_boundary_nor_internal_surface =
                    vertex_is_part_of_not_internal_nor_boundary_surface(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_not_internal_nor_boundary_surface
                    .add_issue( unique_vertex_id,
                        not_boundary_nor_internal_surface.value() );
            }
            if( const auto invalid_internal_topology =
                    vertex_is_part_of_invalid_embedded_surface(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_a_surface_with_invalid_embbedings
                    .add_issue(
                        unique_vertex_id, invalid_internal_topology.value() );
            }
            if( const auto invalid_unique_surface =
                    vertex_is_part_of_invalid_single_surface(
                        unique_vertex_id ) )
            {
                result.unique_vertices_linked_to_a_single_and_invalid_surface
                    .add_issue(
                        unique_vertex_id, invalid_unique_surface.value() );
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
