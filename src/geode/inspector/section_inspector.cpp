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

#include <geode/inspector/section_inspector.hpp>

#include <async++.h>

#include <geode/model/representation/core/section.hpp>

namespace geode
{
    index_t SectionInspectionResult::nb_issues() const
    {
        return meshes.nb_issues() + topology.nb_issues();
    }

    std::string SectionInspectionResult::string() const
    {
        return absl::StrCat( meshes.string(), topology.string() );
    }

    std::string SectionInspectionResult::inspection_type() const
    {
        return "Section inspection";
    }

    SectionInspector::SectionInspector( const Section& section )
        : AddInspectors< Section,
              SectionMeshesInspector,
              SectionTopologyInspector >{ section }
    {
    }

    SectionInspectionResult SectionInspector::inspect_section() const
    {
        SectionInspectionResult result;
        async::parallel_invoke(
            [&result, this] {
                result.meshes = inspect_section_meshes();
            },
            [&result, this] {
                result.topology = inspect_section_topology();
            } );
        return result;
    }
} // namespace geode
