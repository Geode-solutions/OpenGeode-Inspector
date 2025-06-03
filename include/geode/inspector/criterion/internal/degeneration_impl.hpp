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

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    namespace internal
    {
        /*!
         * Implementation of the inspection of the degeneration of a Mesh
         */
        template < typename Mesh >
        class DegenerationImpl
        {
        public:
            virtual ~DegenerationImpl();

            [[nodiscard]] virtual bool is_mesh_degenerated() const;

            [[nodiscard]] InspectionIssues< index_t > small_edges(
                double threshold ) const;

            [[nodiscard]] InspectionIssues< index_t > degenerated_edges() const;

        private:
            [[nodiscard]] bool is_edge_smaller_than_threshold(
                index_t edge_index, double tolerance ) const;

            [[nodiscard]] bool edge_is_degenerated( index_t edge_index ) const;

        protected:
            explicit DegenerationImpl( const Mesh& mesh );

            [[nodiscard]] const Mesh& mesh() const;

        private:
            const Mesh& mesh_;
            bool enabled_edges_;
        };
    } // namespace internal
} // namespace geode
