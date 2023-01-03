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

#include <geode/basic/pimpl.h>

#include <geode/inspector/common.h>

namespace geode
{
    class Section;
    struct uuid;
    struct PolygonEdge;
} // namespace geode

namespace geode
{
    /*!
     * Class for inspecting the adjacency of the surface edges in the Component
     * Meshes of a Section.
     */
    class opengeode_inspector_inspector_api SectionComponentMeshesAdjacency
    {
        OPENGEODE_DISABLE_COPY( SectionComponentMeshesAdjacency );

    public:
        SectionComponentMeshesAdjacency( const Section& model );

        SectionComponentMeshesAdjacency( const Section& model, bool verbose );

        ~SectionComponentMeshesAdjacency();

        std::vector< uuid > components_with_wrong_adjacencies() const;

        absl::flat_hash_map< uuid, index_t >
            surfaces_nb_edges_with_wrong_adjacencies() const;

        absl::flat_hash_map< uuid, std::vector< PolygonEdge > >
            surfaces_edges_with_wrong_adjacencies() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode