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

#include <absl/container/flat_hash_map.h>

#include <geode/basic/pimpl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    class Section;
} // namespace geode

namespace geode
{
    struct
        opengeode_inspector_inspector_api SectionMeshesNegativeElementsInspectionResult
    {
        InspectionIssuesMap< index_t > negative_polygons{ "negative polygons" };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };

    /*!
     * Class for inspecting the negativeelements of elements in the Component
     * Meshes of a Section.
     */
    class
        opengeode_inspector_inspector_api SectionComponentMeshesNegativeElements
    {
        OPENGEODE_DISABLE_COPY( SectionComponentMeshesNegativeElements );

    public:
        explicit SectionComponentMeshesNegativeElements(
            const Section& section );

        ~SectionComponentMeshesNegativeElements();

        [[nodiscard]] SectionMeshesNegativeElementsInspectionResult
            inspect_negative_elements() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode