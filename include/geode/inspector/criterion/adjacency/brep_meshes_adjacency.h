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

#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    struct BRepMeshesAdjacencyInspectionResult
    {
        absl::flat_hash_map< uuid, InspectionIssues< PolygonEdge > >
            surfaces_edges_with_wrong_adjacencies;
        absl::flat_hash_map< uuid, InspectionIssues< PolyhedronFacet > >
            blocks_facets_with_wrong_adjacencies;

        std::string string() const
        {
            std::string message{ "" };
            for( const auto& surface_issue :
                surfaces_edges_with_wrong_adjacencies )
            {
                absl::StrAppend( &message, surface_issue.second.string() );
            }
            for( const auto& block_issue :
                blocks_facets_with_wrong_adjacencies )
            {
                absl::StrAppend( &message, block_issue.second.string() );
            }
            return message;
        }
    };

    /*!
     * Class for inspecting the adjacency of the surface edges and solid facets
     * in the Component Meshes of a BRep.
     */
    class opengeode_inspector_inspector_api BRepComponentMeshesAdjacency
    {
        OPENGEODE_DISABLE_COPY( BRepComponentMeshesAdjacency );

    public:
        BRepComponentMeshesAdjacency( const BRep& model );

        ~BRepComponentMeshesAdjacency();

        BRepMeshesAdjacencyInspectionResult
            inspect_brep_meshes_adjacencies() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode