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

#include <geode/inspector/criterion/internal/degeneration_impl.hpp>

#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/solid_edges.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/geometry/distance.hpp>
#include <geode/geometry/point.hpp>

namespace geode
{
    namespace internal
    {
        template < class MeshType >
        DegenerationImpl< MeshType >::DegenerationImpl( const MeshType& mesh )
            : mesh_( mesh ), enabled_edges_( false )
        {
            if( !mesh_.are_edges_enabled() )
            {
                mesh_.enable_edges();
                enabled_edges_ = true;
            }
        }

        template < class MeshType >
        DegenerationImpl< MeshType >::~DegenerationImpl()
        {
            if( enabled_edges_ )
            {
                mesh_.disable_edges();
            }
        }

        template < class MeshType >
        bool DegenerationImpl< MeshType >::is_mesh_degenerated() const
        {
            for( const auto edge_index : Range{ mesh_.edges().nb_edges() } )
            {
                if( edge_is_degenerated( edge_index ) )
                {
                    return true;
                }
            }
            return false;
        }

        template < class MeshType >
        InspectionIssues< index_t >
            DegenerationImpl< MeshType >::degenerated_edges() const
        {
            InspectionIssues< index_t > degenerated_edges_index{
                "Degenerated Edges."
            };
            for( const auto edge_index : Range{ mesh_.edges().nb_edges() } )
            {
                if( edge_is_degenerated( edge_index ) )
                {
                    const auto edge_vertices =
                        mesh_.edges().edge_vertices( edge_index );
                    degenerated_edges_index.add_issue( edge_index,
                        absl::StrCat( "Edge between vertices with index ",
                            edge_vertices[0], " and index ", edge_vertices[1],
                            ", at position [",
                            mesh_.point( edge_vertices[0] ).string(),
                            "], is degenerated." ) );
                }
            }
            return degenerated_edges_index;
        }

        template < class MeshType >
        bool DegenerationImpl< MeshType >::edge_is_degenerated(
            index_t edge_index ) const
        {
            const auto edge_vertices =
                mesh_.edges().edge_vertices( edge_index );
            const auto p1 = mesh_.point( edge_vertices[0] );
            const auto p2 = mesh_.point( edge_vertices[1] );
            return point_point_distance( p1, p2 ) < GLOBAL_EPSILON;
        }

        template < class MeshType >
        const MeshType& DegenerationImpl< MeshType >::mesh() const
        {
            return mesh_;
        }

        template class opengeode_inspector_inspector_api
            DegenerationImpl< SurfaceMesh2D >;
        template class opengeode_inspector_inspector_api
            DegenerationImpl< SurfaceMesh3D >;
        template class opengeode_inspector_inspector_api
            DegenerationImpl< SolidMesh3D >;
    } // namespace internal
} // namespace geode
