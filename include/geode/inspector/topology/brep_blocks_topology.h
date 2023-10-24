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

#pragma once

#include <absl/container/flat_hash_map.h>
#include <absl/types/optional.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>

namespace geode
{
    struct ComponentMeshVertex;
    class BRep;
    struct uuid;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api BRepBlocksTopologyInspectionResult
    {
        ProblemInspectionResult< uuid > blocks_not_meshed{
            "uuids of block without mesh."
        };
        std::vector< std::pair< uuid, ProblemInspectionResult< index_t > > >
            blocks_not_linked_to_a_unique_vertex;
        ProblemInspectionResult< index_t >
            vertices_part_of_two_blocks_and_no_boundary_surface{
                "Indexes of unique vertex part of two blocks and no boundary "
                "surface (or block incident line)"
            };
        ProblemInspectionResult< index_t >
            vertices_with_incorrect_block_cmvs_count{
                "Indexes of unique vertex part of a block but with incorrect "
                "ComponentMeshVertices count"
            };
    };

    /*!
     * Class for inspecting the topology of a BRep model blocks through
     * their unique vertices
     */
    class opengeode_inspector_inspector_api BRepBlocksTopology
    {
    public:
        BRepBlocksTopology( const BRep& brep );

        bool brep_blocks_topology_is_valid( index_t unique_vertex_index ) const;

        absl::optional< std::string >
            vertex_is_part_of_two_blocks_and_no_boundary_surface(
                index_t unique_vertex_index ) const;

        // should it be rename
        // vertex_is_part_of_block_with_invalid_internal_topology?
        absl::optional< std::string > vertex_block_cmvs_count_is_incorrect(
            index_t unique_vertex_index ) const;

        BRepBlocksTopologyInspectionResult inspect_blocks() const;

    private:
        const BRep& brep_;
    };
} // namespace geode