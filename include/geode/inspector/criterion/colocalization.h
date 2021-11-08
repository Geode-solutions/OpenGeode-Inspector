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
    FORWARD_DECLARATION_DIMENSION_CLASS( VertexSet );
} // namespace geode

namespace geode
{
    namespace inspector
    {
        /*!
         * Class for inspecting the colocalization of points in a mesh
         */
        template < index_t dimension >
        class opengeode_inspector_inspector_api MeshColocalization
        {
        public:
            MeshColocalization();
            ~MeshColocalization();

            bool mesh_has_colocalized_points(
                const VertexSet< dimension >& mesh ) const;

            index_t nb_colocalized_points(
                const VertexSet< dimension >& mesh ) const;

            const std::vector< std::vector< index_t > >
                colocalized_points_groups(
                    const VertexSet< dimension >& mesh ) const;

        private:
            IMPLEMENTATION_MEMBER( impl_ );
        };
        ALIAS_2D_AND_3D( MeshColocalization );
    } // namespace inspector
} // namespace geode