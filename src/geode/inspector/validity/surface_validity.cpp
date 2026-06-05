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

#include <geode/inspector/validity/surface_validity.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/inspector/inspection/surface_inspector.hpp>
#include <geode/inspector/validity/object_validity.hpp>

namespace geode
{
    template < index_t dimension >
    ObjectValidity is_surface_valid( const SurfaceMesh< dimension >& surface )
    {
        SurfaceMeshInspector< dimension > surface_inspector{ surface };
        const auto inspection_result = surface_inspector.inspect_surface();
        ObjectValidity invalidities;
        if( inspection_result.polygon_edges_with_wrong_adjacency.nb_issues()
            != 0 )
        {
            invalidities.invalidities.emplace_back(
                inspection_result.polygon_edges_with_wrong_adjacency.string() );
        }
        if( inspection_result.colocated_points_groups.nb_issues() != 0 )
        {
            invalidities.invalidities.emplace_back(
                inspection_result.colocated_points_groups.string() );
        }
        if( inspection_result.degenerated_edges.nb_issues() != 0 )
        {
            invalidities.invalidities.emplace_back(
                inspection_result.degenerated_edges.string() );
        }
        if( inspection_result.degenerated_polygons.nb_issues() != 0 )
        {
            invalidities.invalidities.emplace_back(
                inspection_result.degenerated_polygons.string() );
        }
        if( inspection_result.non_manifold_edges.nb_issues() != 0 )
        {
            invalidities.invalidities.emplace_back(
                inspection_result.non_manifold_edges.string() );
        }
        if( inspection_result.non_manifold_vertices.nb_issues() != 0 )
        {
            invalidities.invalidities.emplace_back(
                inspection_result.non_manifold_vertices.string() );
        }
        if( inspection_result.intersecting_elements.nb_issues() != 0 )
        {
            invalidities.invalidities.emplace_back(
                inspection_result.intersecting_elements.string() );
        }
        if( inspection_result.negative_polygons.nb_issues() != 0 )
        {
            invalidities.invalidities.emplace_back(
                inspection_result.negative_polygons.string() );
        }
        return invalidities;
    }

    template ObjectValidity opengeode_inspector_validity_api is_surface_valid(
        const SurfaceMesh< 2 >& );
    template ObjectValidity opengeode_inspector_validity_api is_surface_valid(
        const SurfaceMesh< 3 >& );
} // namespace geode
