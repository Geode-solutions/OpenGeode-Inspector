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

#include <geode/inspector/surface_inspector.hpp>

#include <async++.h>

#include <geode/mesh/core/surface_mesh.hpp>

namespace geode
{
    index_t SurfaceInspectionResult::nb_issues() const
    {
        return polygon_edges_with_wrong_adjacency.nb_issues()
               + colocated_points_groups.nb_issues()
               + degenerated_edges.nb_issues()
               + degenerated_polygons.nb_issues()
               + non_manifold_edges.nb_issues()
               + non_manifold_vertices.nb_issues()
               + intersecting_elements.nb_issues();
    }

    std::string SurfaceInspectionResult::string() const
    {
        return absl::StrCat( polygon_edges_with_wrong_adjacency.string(),
            colocated_points_groups.string(), degenerated_edges.string(),
            degenerated_polygons.string(), non_manifold_edges.string(),
            non_manifold_vertices.string(), intersecting_elements.string() );
    }

    std::string SurfaceInspectionResult::inspection_type() const
    {
        return "SurfaceInspection";
    }

    template < index_t dimension >
    SurfaceMeshInspector< dimension >::SurfaceMeshInspector(
        const SurfaceMesh< dimension >& mesh )
        : AddInspectors< SurfaceMesh< dimension >,
              SurfaceMeshAdjacency< dimension >,
              SurfaceMeshColocation< dimension >,
              SurfaceMeshDegeneration< dimension >,
              SurfaceMeshEdgeManifold< dimension >,
              SurfaceMeshVertexManifold< dimension >,
              SurfaceMeshIntersections< dimension > >{ mesh }
    {
    }

    template < index_t dimension >
    SurfaceInspectionResult
        SurfaceMeshInspector< dimension >::inspect_surface() const
    {
        SurfaceInspectionResult result;
        async::parallel_invoke(
            [&result, this] {
                result.polygon_edges_with_wrong_adjacency =
                    this->polygon_edges_with_wrong_adjacency();
            },
            [&result, this] {
                result.colocated_points_groups =
                    this->colocated_points_groups();
            },
            [&result, this] {
                result.degenerated_edges = this->degenerated_edges();
            },
            [&result, this] {
                result.degenerated_polygons = this->degenerated_polygons();
            },
            [&result, this] {
                result.non_manifold_edges = this->non_manifold_edges();
            },
            [&result, this] {
                result.non_manifold_vertices = this->non_manifold_vertices();
            },
            [&result, this] {
                result.intersecting_elements = this->intersecting_elements();
            } );
        return result;
    }

    template class opengeode_inspector_inspector_api SurfaceMeshInspector< 2 >;
    template class opengeode_inspector_inspector_api SurfaceMeshInspector< 3 >;
} // namespace geode
