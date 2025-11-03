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

#include <geode/inspector/criterion/internal/component_meshes_manifold.hpp>

#include <geode/basic/logger.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/manifold/surface_edge_manifold.hpp>
#include <geode/inspector/criterion/manifold/surface_vertex_manifold.hpp>

namespace geode
{
    namespace internal
    {
        template < typename Model >
        ComponentMeshesManifold< Model >::ComponentMeshesManifold(
            const Model& model )
            : model_( model )
        {
        }

        template < typename Model >
        void ComponentMeshesManifold< Model >::
            add_surfaces_meshes_non_manifold_vertices(
                InspectionIssuesMap< index_t >& surfaces_non_manifold_vertices )
                const
        {
            for( const auto& surface : model_.surfaces() )
            {
                const SurfaceMeshVertexManifold< Model::dim > inspector{
                    surface.mesh()
                };
                auto issues = inspector.non_manifold_vertices();
                issues.set_description(
                    absl::StrCat( "Surface ", surface.name(), " (",
                        surface.id().string(), ") non manifold vertices" ) );
                surfaces_non_manifold_vertices.add_issues_to_map(
                    surface.id(), std::move( issues ) );
            }
        }

        template < typename Model >

        void ComponentMeshesManifold< Model >::
            add_surfaces_meshes_non_manifold_edges(
                InspectionIssuesMap< std::array< index_t, 2 > >&
                    surfaces_non_manifold_edges ) const
        {
            for( const auto& surface : model_.surfaces() )
            {
                const SurfaceMeshEdgeManifold< Model::dim > inspector{
                    surface.mesh()
                };
                auto issues = inspector.non_manifold_edges();
                issues.set_description(
                    absl::StrCat( "Surface ", surface.name(), " (",
                        surface.id().string(), ") non manifold edges" ) );
                surfaces_non_manifold_edges.add_issues_to_map(
                    surface.id(), std::move( issues ) );
            }
        }

        template < typename Model >
        const Model& ComponentMeshesManifold< Model >::model() const
        {
            return model_;
        }

        template class opengeode_inspector_inspector_api
            ComponentMeshesManifold< Section >;
        template class opengeode_inspector_inspector_api
            ComponentMeshesManifold< BRep >;
    } // namespace internal
} // namespace geode
