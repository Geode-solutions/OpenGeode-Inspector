/*
 * Copyright (c) 2019 - 2021 Geode-solutions
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
    FORWARD_DECLARATION_DIMENSION_CLASS( SurfaceMesh );
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
} // namespace geode

namespace geode
{
    namespace inspector
    {
        /*!
         * Class for inspecting the degeneration of a SurfaceMesh
         */
        template < index_t dimension >
        class opengeode_inspector_inspector_api SurfaceMeshDegeneration
        {
        public:
            SurfaceMeshDegeneration() = default;
            SurfaceMeshDegeneration( SurfaceMeshDegeneration&& other );
            ~SurfaceMeshDegeneration();

            bool is_mesh_degenerated(
                const SurfaceMesh< dimension >& mesh ) const;

            index_t nb_degenerated_edges(
                const SurfaceMesh< dimension >& mesh ) const;

            const std::vector< index_t > degenerated_edges(
                const SurfaceMesh< dimension >& mesh ) const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
        ALIAS_2D_AND_3D( SurfaceMeshDegeneration );

        // template < index_t dimension >
        // class SolidMeshDegeneration
        // {
        // public:
        //     SolidMeshDegeneration();
        //     SolidMeshDegeneration( SolidMeshDegeneration&& other );
        //     ~SolidMeshDegeneration();

        //     bool is_mesh_degenerated(
        //         const SolidMesh< dimension >& mesh ) const;

        //     index_t nb_degenerated_edges(
        //         const SolidMesh< dimension >& mesh ) const;

        //     const std::vector< index_t > degenerated_edges(
        //         const SolidMesh< dimension >& mesh ) const;

        // private:
        //     IMPLEMENTATION_MEMBER( impl_ );
        // };
        // ALIAS_2D_AND_3D( SolidMeshDegeneration );
    } // namespace inspector
} // namespace geode
