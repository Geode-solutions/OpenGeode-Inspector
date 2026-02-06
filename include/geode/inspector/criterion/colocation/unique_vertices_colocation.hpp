/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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
    class Section;
    class BRep;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api UniqueVerticesInspectionResult
    {
        InspectionIssues< std::vector< index_t > >
            colocated_unique_vertices_groups{ "colocated unique vertices" };
        InspectionIssues< index_t > unique_vertices_linked_to_different_points{
            "unique vertex refering multiple positions"
        };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };

    /*!
     * Class for inspecting the colocation of unique vertices in a Model (BRep
     * or Section)
     */
    template < typename Model >
    class UniqueVerticesColocation
    {
        OPENGEODE_DISABLE_COPY( UniqueVerticesColocation );

    public:
        explicit UniqueVerticesColocation( const Model& model );

        ~UniqueVerticesColocation();

        [[nodiscard]] bool
            model_has_unique_vertices_linked_to_different_points() const;

        [[nodiscard]] bool model_has_colocated_unique_vertices() const;

        [[nodiscard]] UniqueVerticesInspectionResult
            inspect_unique_vertices() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    using SectionUniqueVerticesColocation = UniqueVerticesColocation< Section >;
    using BRepUniqueVerticesColocation = UniqueVerticesColocation< BRep >;
} // namespace geode