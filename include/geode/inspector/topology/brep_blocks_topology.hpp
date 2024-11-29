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
    struct ComponentMeshVertex;
    class BRep;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api BRepBlocksTopologyInspectionResult
    {
        InspectionIssues< uuid > blocks_not_meshed{
            "uuids of Blocks without mesh."
        };
        InspectionIssues< uuid > wrong_block_boundary_surface{
            "uuid of boundary surface that is actually not a boundary one."
        };
        InspectionIssuesMap< index_t > blocks_not_linked_to_a_unique_vertex{
            "Blocks with mesh vertices not linked to a unique vertex"
        };
        InspectionIssues< index_t >
            unique_vertices_part_of_two_blocks_and_no_boundary_surface{
                "Indices of unique vertices part of two Blocks and no boundary "
                "Surface (or Block incident Line)"
            };
        InspectionIssues< index_t >
            unique_vertices_with_incorrect_block_cmvs_count{
                "Indices of unique vertices part of a Block but with incorrect "
                "ComponentMeshVertices count"
            };

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

        [[nodiscard]] std::optional< std::string >
            unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(
                index_t unique_vertex_index ) const;

        [[nodiscard]] std::optional< std::string >
            unique_vertex_block_cmvs_count_is_incorrect(
                index_t unique_vertex_index ) const;

        [[nodiscard]] BRepBlocksTopologyInspectionResult inspect_blocks() const;

    private:
        const BRep& brep_;
    };
} // namespace geode