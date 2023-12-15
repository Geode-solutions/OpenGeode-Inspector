/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/model/representation/core/section.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>

namespace geode
{
    struct SectionMeshesManifoldInspectionResult
    {
        absl::flat_hash_map< uuid, InspectionIssues< index_t > >
            meshes_non_manifold_vertices;
        absl::flat_hash_map< uuid,
            InspectionIssues< std::array< index_t, 2 > > >
            meshes_non_manifold_edges;
        std::string string() const
        {
            std::string message{ "" };
            for( const auto& vertices_issue : meshes_non_manifold_vertices )
            {
                absl::StrAppend(
                    &message, vertices_issue.second.string(), "\n" );
            }
            for( const auto& edges_issue : meshes_non_manifold_edges )
            {
                absl::StrAppend( &message, edges_issue.second.string(), "\n" );
            }
            return message;
        }
    };
    /*!
     * Class for inspecting the manifold property in the Component Meshes of
     * a Section.
     */
    class opengeode_inspector_inspector_api SectionComponentMeshesManifold
    {
        OPENGEODE_DISABLE_COPY( SectionComponentMeshesManifold );

    public:
        SectionComponentMeshesManifold( const Section& section );

        ~SectionComponentMeshesManifold();

        SectionMeshesManifoldInspectionResult inspect_section_manifold() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode