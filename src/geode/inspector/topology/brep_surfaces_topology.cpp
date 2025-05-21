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

#include <geode/mesh/core/surface_mesh.hpp>

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
            bool is_corner_internal_to_all_surfaces{ true };
            for( const auto& cmv :
                brep_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( cmv.component_id.type()
                    == Corner3D::component_type_static() )
                {
                    if( brep_.Relationships::nb_embeddings(
                            cmv.component_id.id() )
                        != surface_uuids.size() )
                    {
                        is_corner_internal_to_all_surfaces = false;
                    }
                }
            }
            if( !is_corner_internal_to_all_surfaces )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " ",
                    " is part of multiple surfaces, but not part of any "
                    "line and there is no internal corner to all surfaces." );
            }
        }
        if( line_uuids.size() == 1 )
        {
            bool corner_found{ false };
            bool
                is_corner_internal_to_surfaces_with_no_relationship_to_the_line{
                    true
                };
            index_t nb_cmv_lines{ 0 };
            index_t nb_of_relationships_with_surfaces{ 0 };
            for( const auto& cmv :
                brep_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( cmv.component_id.type() == Line3D::component_type_static() )
                {
                    nb_cmv_lines += 1;
                    for( const auto& surface_id : surface_uuids )
                    {
                        if( brep_.Relationships::is_boundary(
                                cmv.component_id.id(), surface_id )
                            || brep_.Relationships::is_internal(
                                cmv.component_id.id(), surface_id ) )
                        {
                            nb_of_relationships_with_surfaces += 1;
                        }
                    }
                }
            }
            for( const auto& cmv :
                brep_.component_mesh_vertices( unique_vertex_index ) )
            {
                if( cmv.component_id.type()
                    == Corner3D::component_type_static() )
                {
                    if( brep_.Relationships::nb_embeddings(
                            cmv.component_id.id() )
                            + nb_of_relationships_with_surfaces
                        != surface_uuids.size() )
                    {
                        is_corner_internal_to_surfaces_with_no_relationship_to_the_line =
                            false;
                    }
                    corner_found = true;
                }
            }
            if( corner_found && nb_cmv_lines < 2
                && !is_corner_internal_to_surfaces_with_no_relationship_to_the_line )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index, " ",
                    " is part of multiple surfaces and only one line, "
                    "but is a corner not internal to all surfaces without "
                    "relationship to the line." );
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
        return std::nullopt;
    }

    BRepSurfacesTopologyInspectionResult
        BRepSurfacesTopology::inspect_surfaces_topology() const
    {
        BRepSurfacesTopologyInspectionResult result;
        for( const auto& surface : brep_.surfaces() )
        {
            if( !surface_is_meshed( brep_.surface( surface.id() ) ) )
            {
                result.surfaces_not_meshed.add_issue(
                    surface.id(), absl::StrCat( surface.id().string(),
                                      " is a surface without mesh." ) );
            }

            auto surface_result = internal::
                model_component_vertices_not_associated_to_unique_vertices(
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
