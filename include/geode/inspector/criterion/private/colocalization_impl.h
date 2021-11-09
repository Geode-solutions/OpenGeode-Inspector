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
         * Implementation of the inspection of the colocalization of a Mesh
         */
        template < class MeshType >
        class opengeode_inspector_inspector_api ColocalizationImpl
        {
            OPENGEODE_DISABLE_COPY( ColocalizationImpl );

        public:
            ColocalizationImpl( const MeshType& mesh );
            ~ColocalizationImpl();

            bool mesh_has_colocalized_points() const;

            index_t nb_colocalized_points() const;

            std::vector< std::vector< index_t > >
                colocalized_points_groups() const;

        private:
            bool points_are_colocalized(
                index_t pt_index_1, index_t pt_index_2 ) const;

            const MeshType& mesh_;
        };
    } // namespace detail
} // namespace geode
