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

#include <geode/inspector/criterion/private/degeneration_impl.h>

#include <geode/basic/logger.h>

#include <geode/mesh/core/solid_edges.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_edges.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/geometry/distance.h>
#include <geode/geometry/point.h>

namespace geode
{
    namespace detail
    {
        template < class MeshType >
        DegenerationImpl< MeshType >::DegenerationImpl(
            const MeshType& mesh, bool verbose )
            : mesh_( mesh ), verbose_( verbose )
        {
            mesh_.enable_edges();
        }

        template < class MeshType >
        DegenerationImpl< MeshType >::~DegenerationImpl()
        {
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
        index_t DegenerationImpl< MeshType >::nb_degenerated_edges() const
        {
            index_t nb_degeneration{ 0 };
            for( const auto edge_index : Range{ mesh_.edges().nb_edges() } )
            {
                if( edge_is_degenerated( edge_index ) )
                {
                    nb_degeneration++;
                }
            }
            return nb_degeneration;
        }

        template < class MeshType >
        std::vector< index_t >
            DegenerationImpl< MeshType >::degenerated_edges() const
        {
            std::vector< index_t > degenerated_edges_index;
            for( const auto edge_index : Range{ mesh_.edges().nb_edges() } )
            {
                if( edge_is_degenerated( edge_index ) )
                {
                    degenerated_edges_index.push_back( edge_index );
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
            const auto degenerated =
                point_point_distance( p1, p2 ) < global_epsilon;
            if( degenerated && verbose_ )
            {
                Logger::info( "Edge between vertices with index ",
                    edge_vertices[0], " and index ", edge_vertices[1],
                    ", at position [", p1.string(), "], is degenerated." );
            }
            return degenerated;
        }

        template < class MeshType >
        const MeshType& DegenerationImpl< MeshType >::mesh() const
        {
            return mesh_;
        }

        template < class MeshType >
        bool DegenerationImpl< MeshType >::verbose() const
        {
            return verbose_;
        }

        template class opengeode_inspector_inspector_api
            DegenerationImpl< SurfaceMesh2D >;
        template class opengeode_inspector_inspector_api
            DegenerationImpl< SurfaceMesh3D >;
        template class opengeode_inspector_inspector_api
            DegenerationImpl< SolidMesh3D >;
    } // namespace detail
} // namespace geode
