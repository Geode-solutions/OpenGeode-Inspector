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

#include <absl/container/inlined_vector.h>

#include <geode/basic/pimpl.h>

#include <geode/mesh/core/solid_mesh.h>

#include <geode/inspector/common.h>

namespace geode
{
    /*!
     * Class for inspecting the manifold property of a SolidMesh
     */
    template < index_t dimension >
    class opengeode_inspector_inspector_api SolidMeshFacetManifold
    {
        OPENGEODE_DISABLE_COPY( SolidMeshFacetManifold );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        SolidMeshFacetManifold( const SolidMesh< dimension >& mesh );

        SolidMeshFacetManifold(
            const SolidMesh< dimension >& mesh, bool verbose );

        ~SolidMeshFacetManifold();

        bool mesh_facets_are_manifold() const;

        index_t nb_non_manifold_facets() const;

        std::vector< PolyhedronFacetVertices > non_manifold_facets() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( SolidMeshFacetManifold );
} // namespace geode