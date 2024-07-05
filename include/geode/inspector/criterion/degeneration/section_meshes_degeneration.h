/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/basic/pimpl.h>
#include <geode/basic/uuid.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>

namespace geode
{
    class Section;
} // namespace geode

namespace geode
{
    struct
        opengeode_inspector_inspector_api SectionMeshesDegenerationInspectionResult
    {
        InspectionIssuesMap< index_t > degenerated_edges{
            "Section component meshes with degenerated edges"
        };
        InspectionIssuesMap< index_t > degenerated_polygons{
            "Section component meshes with degenerated polygons"
        };

        std::string string() const;

        std::string inspection_type() const;
    };

    /*!
     * Class for inspecting the degeneration of elements in the Component Meshes
     * of a Section.
     */
    class opengeode_inspector_inspector_api SectionComponentMeshesDegeneration
    {
        OPENGEODE_DISABLE_COPY( SectionComponentMeshesDegeneration );

    public:
        explicit SectionComponentMeshesDegeneration( const Section& section );

        ~SectionComponentMeshesDegeneration();

        SectionMeshesDegenerationInspectionResult
            inspect_elements_degeneration() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode