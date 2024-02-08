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

#include <geode/inspector/topology/section_surfaces_topology.h>

#include <geode/basic/algorithm.h>
#include <geode/basic/logger.h>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/topology/private/topology_helpers.h>

namespace geode
{
    std::string SectionSurfacesTopologyInspectionResult::string() const
    {
        auto message = absl::StrCat( surfaces_not_meshed.string(), "\n" );
        for( const auto& surface_uv_issue :
            surfaces_not_linked_to_a_unique_vertex )
        {
            absl::StrAppend( &message, surface_uv_issue.second.string(), "\n" );
        }
        absl::StrAppend( &message,
            unique_vertices_linked_to_a_surface_with_invalid_embbedings
                .string(),
            "\n" );
        absl::StrAppend( &message,
            unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                .string(),
            "\n" );
        return message;
    }
    SectionSurfacesTopology::SectionSurfacesTopology( const Section& section )
        : section_( section )
    {
    }

    bool SectionSurfacesTopology::section_vertex_surfaces_topology_is_valid(
        index_t unique_vertex_index ) const
    {
        if( section_
                .component_mesh_vertices(
                    unique_vertex_index, Surface2D::component_type_static() )
                .empty() )
        {
            return true;
        }
        if( vertex_is_part_of_invalid_embedded_surface( unique_vertex_index )
            || vertex_is_part_of_line_and_not_on_surface_border(
                unique_vertex_index ) )
        {
            return false;
        }
        return true;
    }

    absl::optional< std::string >
        SectionSurfacesTopology::vertex_is_part_of_invalid_embedded_surface(
            index_t unique_vertex_index ) const
    {
        const auto surface_uuids =
            detail::components_uuids( section_.component_mesh_vertices(
                unique_vertex_index, Surface2D::component_type_static() ) );
        if( surface_uuids.size() == 2 )
        {
            for( const auto& line : section_.component_mesh_vertices(
                     unique_vertex_index, Line2D::component_type_static() ) )
            {
                if( section_.Relationships::is_boundary(
                        line.component_id.id(), surface_uuids[0] )
                    && section_.Relationships::is_boundary(
                        line.component_id.id(), surface_uuids[1] ) )
                {
                    return absl::nullopt;
                }
            }
            return absl::StrCat( "Unique vertex with index ",
                absl::StrCat( unique_vertex_index ),
                " is part of two surfaces, but is associated to no "
                "line boundary of the two surfaces." );
        }
        return absl::nullopt;
    }

    absl::optional< std::string > SectionSurfacesTopology::
        vertex_is_part_of_line_and_not_on_surface_border(
            index_t unique_vertex_index ) const
    {
        if( !detail::section_surfaces_are_meshed( section_ ) )
        {
            return absl::nullopt;
        }
        if( section_
                .component_mesh_vertices(
                    unique_vertex_index, Line2D::component_type_static() )
                .empty() )
        {
            return absl::nullopt;
        }
        for( const auto& surface_vertex : section_.component_mesh_vertices(
                 unique_vertex_index, Surface2D::component_type_static() ) )
        {
            if( !section_.surface( surface_vertex.component_id.id() )
                     .mesh()
                     .is_vertex_on_border( surface_vertex.vertex ) )
            {
                return absl::StrCat( "Unique vertex with index ",
                    unique_vertex_index,
                    " is part of a line and of surface with uuid '",
                    surface_vertex.component_id.id().string(),
                    "' but the associated vertex in the surface "
                    "mesh is not on the mesh border." );
            }
        }
        return absl::nullopt;
    }

    SectionSurfacesTopologyInspectionResult
        SectionSurfacesTopology::inspect_surfaces() const
    {
        SectionSurfacesTopologyInspectionResult result;
        for( const auto& surface : section_.surfaces() )
        {
            if( section_.surface( surface.id() ).mesh().nb_vertices() == 0 )
            {
                result.surfaces_not_meshed.add_problem(
                    surface.id(), absl::StrCat( surface.id().string(),
                                      " is a surface without mesh." ) );
            }

            auto surface_result = detail::
                section_component_vertices_are_associated_to_unique_vertices(
                    section_, surface.component_id(), surface.mesh() );
            surface_result.description =
                absl::StrCat( "Surface ", surface.id().string(),
                    " has mesh vertices not linked to a unique vertex." );
            result.surfaces_not_linked_to_a_unique_vertex.emplace_back(
                surface.id(), surface_result );
        }
        for( const auto unique_vertex_id :
            Range{ section_.nb_unique_vertices() } )
        {
            if( const auto invalid_internal_topology =
                    vertex_is_part_of_invalid_embedded_surface(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_a_surface_with_invalid_embbedings
                    .add_problem(
                        unique_vertex_id, invalid_internal_topology.value() );
            }
            if( const auto line_and_not_on_surface_border =
                    vertex_is_part_of_line_and_not_on_surface_border(
                        unique_vertex_id ) )
            {
                result
                    .unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                    .add_problem( unique_vertex_id,
                        line_and_not_on_surface_border.value() );
            }
        }
        return result;
    }

} // namespace geode
