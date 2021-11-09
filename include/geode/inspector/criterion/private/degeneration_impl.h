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

#include <geode/inspector/common.h>

namespace geode
{
    namespace detail
    {
        /*!
         * Implementation of the inspection of the degeneration of a Mesh
         */
        template < class MeshType >
        class opengeode_inspector_inspector_api DegenerationImpl
        {
            OPENGEODE_DISABLE_COPY( DegenerationImpl );

        public:
            DegenerationImpl( const MeshType& mesh );

            bool is_mesh_degenerated() const;

            index_t nb_degenerated_edges() const;

            std::vector< index_t > degenerated_edges() const;

        private:
            bool edge_is_degenerated( index_t edge_index ) const;

        private:
            const MeshType& mesh_;
        };
    } // namespace detail
} // namespace geode
