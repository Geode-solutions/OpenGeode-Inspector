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

#include <geode/inspector/inspection/criterion/internal/isolation_impl.hpp>

#include <vector>

#include <absl/algorithm/container.h>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

namespace
{
    template < geode::index_t dimension >
    std::vector< bool > vertices_in_elements(
        const geode::EdgedCurve< dimension >& mesh )
    {
        std::vector< bool > in_element( mesh.nb_vertices(), false );
        for( const auto edge_id : geode::Range{ mesh.nb_edges() } )
        {
            for( const auto vertex_id : mesh.edge_vertices( edge_id ) )
            {
                in_element[vertex_id] = true;
            }
        }
        return in_element;
    }

    template < geode::index_t dimension >
    std::vector< bool > vertices_in_elements(
        const geode::SurfaceMesh< dimension >& mesh )
    {
        std::vector< bool > in_element( mesh.nb_vertices(), false );
        for( const auto polygon_id : geode::Range{ mesh.nb_polygons() } )
        {
            for( const auto vertex_id : mesh.polygon_vertices( polygon_id ) )
            {
                in_element[vertex_id] = true;
            }
        }
        return in_element;
    }

    template < geode::index_t dimension >
    std::vector< bool > vertices_in_elements(
        const geode::SolidMesh< dimension >& mesh )
    {
        std::vector< bool > in_element( mesh.nb_vertices(), false );
        for( const auto polyhedron_id : geode::Range{ mesh.nb_polyhedra() } )
        {
            for( const auto vertex_id :
                mesh.polyhedron_vertices( polyhedron_id ) )
            {
                in_element[vertex_id] = true;
            }
        }
        return in_element;
    }
} // namespace

namespace geode
{
    namespace internal
    {
        template < typename Mesh >
        bool mesh_has_isolated_vertices( const Mesh& mesh )
        {
            return absl::c_linear_search( vertices_in_elements( mesh ), false );
        }

        template < typename Mesh >
        InspectionIssues< index_t > isolated_vertices( const Mesh& mesh )
        {
            const auto in_element = vertices_in_elements( mesh );
            InspectionIssues< index_t > issues{ "isolated vertices" };
            for( const auto vertex_id : Range{ mesh.nb_vertices() } )
            {
                if( !in_element[vertex_id] )
                {
                    issues.add_issue( vertex_id,
                        absl::StrCat( "vertex ", vertex_id, ", at position [",
                            mesh.point( vertex_id ).string(), "]" ) );
                }
            }
            return issues;
        }

        template bool mesh_has_isolated_vertices( const EdgedCurve< 2 >& );
        template bool mesh_has_isolated_vertices( const EdgedCurve< 3 >& );
        template bool mesh_has_isolated_vertices( const SurfaceMesh< 2 >& );
        template bool mesh_has_isolated_vertices( const SurfaceMesh< 3 >& );
        template bool mesh_has_isolated_vertices( const SolidMesh< 3 >& );
        template InspectionIssues< index_t > isolated_vertices(
            const EdgedCurve< 2 >& );
        template InspectionIssues< index_t > isolated_vertices(
            const EdgedCurve< 3 >& );
        template InspectionIssues< index_t > isolated_vertices(
            const SurfaceMesh< 2 >& );
        template InspectionIssues< index_t > isolated_vertices(
            const SurfaceMesh< 3 >& );
        template InspectionIssues< index_t > isolated_vertices(
            const SolidMesh< 3 >& );
    } // namespace internal
} // namespace geode
