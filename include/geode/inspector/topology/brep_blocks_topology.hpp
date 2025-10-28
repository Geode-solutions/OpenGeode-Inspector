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

#pragma once

#include <optional>
#include <string>
#include <utility>
#include <vector>

#include <geode/basic/uuid.hpp>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( Block );
    ALIAS_3D( Block );
    struct ComponentMeshVertex;
    class BRep;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api BRepBlocksTopologyInspectionResult
    {
        InspectionIssues< uuid > some_blocks_not_meshed{
            "blocks without mesh (UUIDs listed)."
        };
        InspectionIssues< uuid > wrong_block_boundary_surface{
            "surfaces incorrectly bounding blocks (UUIDs listed)."
        };
        InspectionIssuesMap< index_t > blocks_not_linked_to_a_unique_vertex{
            "blocks containing mesh vertices not linked to unique vertices."
        };
        InspectionIssues< index_t >
            unique_vertices_part_of_two_blocks_and_no_boundary_surface{
                "unique vertices shared by two blocks without a boundary "
                "surface or incident block line."
            };
        InspectionIssues< index_t >
            unique_vertices_with_incorrect_block_cmvs_count{
                "unique vertices in a block with incorrect "
                "ComponentMeshVertices count."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks{
                "unique vertices linked to surfaces with neither boundary nor "
                "internal status."
            };
        InspectionIssues< index_t >
            unique_vertices_linked_to_a_single_and_invalid_surface{
                "unique vertices linked to a single surface that is invalid."
            };
        InspectionIssues< uuid > blocks_with_not_closed_boundary_surfaces{
            "blocks with non-closed boundary surfaces."
        };
        InspectionIssues< index_t > model_boundaries_dont_form_a_closed_surface{
            "model boundaries don't form a closed surface (UUIDs listed)."
        };
        InspectionIssues< index_t >
            unique_vertex_linked_to_multiple_invalid_surfaces{
                "unique vertices linked to multiple surfaces in an invalid way."
            };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };

    /*!
     * Class for inspecting the topology of a BRep model blocks through
     * their unique vertices
     */
    class opengeode_inspector_inspector_api BRepBlocksTopology
    {
    public:
        explicit BRepBlocksTopology( const BRep& brep );

        [[nodiscard]] bool brep_blocks_topology_is_valid(
            index_t unique_vertex_index ) const;

        [[nodiscard]] bool block_is_meshed( const Block3D& block ) const;

        [[nodiscard]] bool block_vertices_are_associated_to_unique_vertices(
            const Block3D& block ) const;

        [[nodiscard]] std::optional< std::string >
            unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            unique_vertex_block_cmvs_count_is_incorrect(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_surface_with_wrong_relationships_to_block(
                index_t unique_vertex_index,
                absl::Span< const uuid > not_boundaries_surfaces,
                absl::Span< const uuid > dangling_surface ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_invalid_single_surface(
                index_t unique_vertex_index,
                absl::Span< const uuid > not_boundaries_surfaces ) const;

        [[nodiscard]] std::optional< std::string >
            vertex_is_part_of_invalid_multiple_surfaces(
                index_t unique_vertex_index ) const;

        [[nodiscard]] BRepBlocksTopologyInspectionResult inspect_blocks() const;

    private:
        const BRep& brep_;
    };
} // namespace geode