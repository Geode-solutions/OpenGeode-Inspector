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

#include <geode/model/mixin/core/vertex_identifier.hpp>

#include <geode/inspector/common.hpp>

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    namespace internal
    {
        struct VertexCMVsByComponent
        {
            std::vector< ComponentMeshVertex > block_cmvs;
            std::vector< ComponentMeshVertex > surface_cmvs;
            std::vector< ComponentMeshVertex > line_cmvs;
            std::vector< ComponentMeshVertex > corner_cmvs;
        };

        VertexCMVsByComponent vertex_cmvs_by_component(
            const BRep& brep, index_t unique_vertex_id );

        index_t nb_expected_block_cmvs( const BRep& brep,
            index_t unique_vertex_id,
            const geode::uuid& block_uuid,
            const VertexCMVsByComponent& unique_vertex_cmvs );

        std::optional< std::string > wrong_nb_expected_block_cmvs(
            const BRep& brep,
            index_t unique_vertex_id,
            const geode::uuid& block_uuid,
            const VertexCMVsByComponent& unique_vertex_cmvs );
    } // namespace internal
} // namespace geode