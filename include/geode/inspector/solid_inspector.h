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

#include <geode/inspector/common.h>
#include <geode/inspector/criterion/adjacency/solid_adjacency.h>
#include <geode/inspector/criterion/colocation/solid_colocation.h>
#include <geode/inspector/criterion/degeneration/solid_degeneration.h>
#include <geode/inspector/criterion/manifold/solid_edge_manifold.h>
#include <geode/inspector/criterion/manifold/solid_facet_manifold.h>
#include <geode/inspector/criterion/manifold/solid_vertex_manifold.h>
#include <geode/inspector/mixin/add_inspectors.h>

namespace geode
{
    /*!
     * Class for inspecting a SolidMesh
     * @extends SolidMeshColocation
     * @extends SolidMeshDegeneration
     */
    template < index_t dimension >
    class opengeode_inspector_inspector_api SolidMeshInspector
        : public AddInspectors< SolidMesh< dimension >,
              SolidMeshAdjacency< dimension >,
              SolidMeshColocation< dimension >,
              SolidMeshDegeneration< dimension >,
              SolidMeshVertexManifold< dimension >,
              SolidMeshEdgeManifold< dimension >,
              SolidMeshFacetManifold< dimension > >
    {
        OPENGEODE_DISABLE_COPY( SolidMeshInspector );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        SolidMeshInspector( const SolidMesh< dimension >& mesh );

        SolidMeshInspector( const SolidMesh< dimension >& mesh, bool verbose );
    };
    ALIAS_3D( SolidMeshInspector );
} // namespace geode