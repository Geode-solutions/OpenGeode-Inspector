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

#include <geode/inspector/inspection/criterion/internal/degeneration_impl.hpp>

#include <absl/algorithm/container.h>
#include <absl/container/flat_hash_set.h>

#include <geode/basic/uuid.hpp>

#include <geode/mesh/core/solid_edges.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_edges.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/geometry/point.hpp>
#include <geode/geometry/vector.hpp>

namespace
{
    // Calls the action on the vertices of each edge of each element, edges
    // shared by several elements are visited several times. Stops when the
    // action returns true.
    template < geode::index_t dimension, typename Action >
    bool for_each_element_edge(
        const geode::SurfaceMesh< dimension >& mesh, const Action& action )
    {
        for( const auto polygon_id : geode::Range{ mesh.nb_polygons() } )
        {
            for( const auto edge_id :
                geode::LRange{ mesh.nb_polygon_edges( polygon_id ) } )
            {
                if( action( mesh.polygon_edge_vertices(
                        { polygon_id, edge_id } ) ) )
                {
                    return true;
                }
            }
        }
        return false;
    }

    template < typename Action >
    bool for_each_element_edge(
        const geode::SolidMesh3D& mesh, const Action& action )
    {
        for( const auto polyhedron_id : geode::Range{ mesh.nb_polyhedra() } )
        {
            for( const auto& edge_vertices :
                mesh.polyhedron_edges_vertices( polyhedron_id ) )
            {
                if( action( edge_vertices ) )
                {
                    return true;
                }
            }
        }
        return false;
    }
} // namespace

namespace geode
{
    namespace internal
    {
        template < class MeshType >
        DegenerationImpl< MeshType >::DegenerationImpl( const MeshType& mesh )
            : mesh_( mesh ), enabled_edges_( false )
        {
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
            return for_each_element_edge(
                mesh_, [this]( const std::array< index_t, 2 >& edge_vertices ) {
                    return edge_is_smaller_than_threshold(
                        edge_vertices, GLOBAL_EPSILON );
                } );
        }

        template < class MeshType >
        InspectionIssues< index_t > DegenerationImpl< MeshType >::small_edges(
            double threshold ) const
        {
            InspectionIssues< index_t > degenerated_edges_index{
                "degenerated edges"
            };
            // Edges are only built when some small edge is found, to convert
            // its vertices into an edge index
            absl::flat_hash_set< std::array< index_t, 2 > > small_edges;
            for_each_element_edge(
                mesh_, [this, threshold, &small_edges](
                           std::array< index_t, 2 > edge_vertices ) {
                    if( edge_is_smaller_than_threshold(
                            edge_vertices, threshold ) )
                    {
                        if( edge_vertices[0] > edge_vertices[1] )
                        {
                            std::swap( edge_vertices[0], edge_vertices[1] );
                        }
                        small_edges.insert( edge_vertices );
                    }
                    return false;
                } );
            if( small_edges.empty() )
            {
                return degenerated_edges_index;
            }
            enable_edges_on_mesh();
            std::vector< index_t > small_edge_indices;
            small_edge_indices.reserve( small_edges.size() );
            for( const auto& edge_vertices : small_edges )
            {
                if( const auto edge_index =
                        mesh_.edges().edge_from_vertices( edge_vertices ) )
                {
                    small_edge_indices.push_back( edge_index.value() );
                }
            }
            absl::c_sort( small_edge_indices );
            for( const auto edge_index : small_edge_indices )
            {
                const auto& edge_vertices =
                    mesh_.edges().edge_vertices( edge_index );
                degenerated_edges_index.add_issue( edge_index,
                    absl::StrCat( "degenerated edge between vertices ",
                        edge_vertices[0], " and ", edge_vertices[1],
                        ", at position [",
                        mesh_.point( edge_vertices[0] ).string(), "]" ) );
            }
            return degenerated_edges_index;
        }

        template < class MeshType >
        InspectionIssues< index_t >
            DegenerationImpl< MeshType >::degenerated_edges() const
        {
            return small_edges( GLOBAL_EPSILON );
        }

        template < class MeshType >
        bool DegenerationImpl< MeshType >::edge_is_smaller_than_threshold(
            const std::array< index_t, 2 >& edge_vertices,
            double threshold ) const
        {
            const Vector< MeshType::dim > edge{ mesh_.point( edge_vertices[0] ),
                mesh_.point( edge_vertices[1] ) };
            return edge.length2() < threshold * threshold;
        }

        template < class MeshType >
        void DegenerationImpl< MeshType >::enable_edges_on_mesh() const
        {
            if( !mesh_.are_edges_enabled() )
            {
                mesh_.enable_edges();
                enabled_edges_ = true;
            }
        }

        template < class MeshType >
        const MeshType& DegenerationImpl< MeshType >::mesh() const
        {
            return mesh_;
        }

        template class opengeode_inspector_inspection_api
            DegenerationImpl< SurfaceMesh2D >;
        template class opengeode_inspector_inspection_api
            DegenerationImpl< SurfaceMesh3D >;
        template class opengeode_inspector_inspection_api
            DegenerationImpl< SolidMesh3D >;
    } // namespace internal
} // namespace geode
