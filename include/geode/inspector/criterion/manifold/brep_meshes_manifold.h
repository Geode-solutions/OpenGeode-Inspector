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

#include <geode/mesh/core/solid_mesh.h>

#include <geode/inspector/common.h>

namespace geode
{
    struct uuid;
    class BRep;
} // namespace geode

namespace geode
{
    /*!
     * Class for inspecting the manifold property in the Component Meshes of
     * a BRep.
     */
    class opengeode_inspector_inspector_api BRepComponentMeshesManifold
    {
        OPENGEODE_DISABLE_COPY( BRepComponentMeshesManifold );

    public:
        BRepComponentMeshesManifold( const BRep& brep );

        BRepComponentMeshesManifold( const BRep& brep, bool verbose );

        ~BRepComponentMeshesManifold();

        std::vector< uuid > components_non_manifold_meshes() const;

        absl::flat_hash_map< uuid, index_t >
            component_meshes_nb_non_manifold_vertices() const;

        absl::flat_hash_map< uuid, index_t >
            component_meshes_nb_non_manifold_edges() const;

        absl::flat_hash_map< uuid, index_t >
            component_meshes_nb_non_manifold_facets() const;

        absl::flat_hash_map< uuid, std::vector< index_t > >
            component_meshes_non_manifold_vertices() const;

        absl::flat_hash_map< uuid, std::vector< std::array< index_t, 2 > > >
            component_meshes_non_manifold_edges() const;

        absl::flat_hash_map< uuid, std::vector< PolyhedronFacetVertices > >
            component_meshes_non_manifold_facets() const;

        absl::flat_hash_map< std::array< index_t, 2 >, std::vector< uuid > >
            model_non_manifold_edges() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
} // namespace geode