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

#include <geode/basic/pimpl.hpp>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );

    namespace detail
    {
        template < typename Container >
        class VertexCycle;
    }
} // namespace geode

namespace geode
{
    /*!
     * Class for inspecting the manifold property of a SolidMesh
     */
    template < index_t dimension >
    class opengeode_inspector_inspector_api SolidMeshEdgeManifold
    {
        OPENGEODE_DISABLE_COPY( SolidMeshEdgeManifold );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        explicit SolidMeshEdgeManifold( const SolidMesh< dimension >& mesh );

        ~SolidMeshEdgeManifold();

        [[nodiscard]] bool mesh_edges_are_manifold() const;

        [[nodiscard]] InspectionIssues< std::array< index_t, 2 > >
            non_manifold_edges() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( SolidMeshEdgeManifold );
} // namespace geode