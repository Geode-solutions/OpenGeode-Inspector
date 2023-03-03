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

#include <geode/basic/pimpl.h>

#include <geode/inspector/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
    struct PolyhedronFacet;
} // namespace geode

namespace geode
{
    /*!
     * Class for inspecting the adjacency on the facets of a SolidMesh
     */
    template < index_t dimension >
    class opengeode_inspector_inspector_api SolidMeshAdjacency
    {
        OPENGEODE_DISABLE_COPY( SolidMeshAdjacency );

    public:
        SolidMeshAdjacency( const SolidMesh< dimension >& mesh );

        SolidMeshAdjacency( const SolidMesh< dimension >& mesh, bool verbose );

        ~SolidMeshAdjacency();

        bool mesh_has_wrong_adjacencies() const;

        index_t nb_facets_with_wrong_adjacency() const;

        std::vector< PolyhedronFacet >
            polyhedron_facets_with_wrong_adjacency() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( SolidMeshAdjacency );
} // namespace geode