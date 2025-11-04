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

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    class BRep;
} // namespace geode

namespace geode
{
    struct BRepNonManifoldEdge
    {
        std::array< index_t, 2 > edge;
        std::vector< uuid > component_ids;
    };

    struct BRepNonManifoldFacet
    {
        PolygonVertices facet;
        std::vector< uuid > component_ids;
    };

    struct opengeode_inspector_inspector_api BRepMeshesManifoldInspectionResult
    {
        InspectionIssuesMap< index_t > meshes_non_manifold_vertices{
            "non manifold mesh vertices"
        };

        InspectionIssuesMap< std::array< index_t, 2 > >
            meshes_non_manifold_edges{ "non manifold mesh edges" };

        InspectionIssuesMap< PolyhedronFacetVertices >
            meshes_non_manifold_facets{ "non manifold mesh facets" };

        InspectionIssues< BRepNonManifoldEdge > brep_non_manifold_edges{
            "non manifold model edges"
        };

        InspectionIssues< BRepNonManifoldFacet > brep_non_manifold_facets{
            "non manifold model facets"
        };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };
    /*!
     * Class for inspecting the manifold property in the Component Meshes of
     * a BRep.
     */
    class opengeode_inspector_inspector_api BRepComponentMeshesManifold
    {
        OPENGEODE_DISABLE_COPY( BRepComponentMeshesManifold );

    public:
        explicit BRepComponentMeshesManifold( const BRep& brep );

        ~BRepComponentMeshesManifold();

        [[nodiscard]] BRepMeshesManifoldInspectionResult
            inspect_brep_manifold() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode