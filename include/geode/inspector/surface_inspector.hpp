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

#include <geode/inspector/common.hpp>
#include <geode/inspector/criterion/adjacency/surface_adjacency.hpp>
#include <geode/inspector/criterion/colocation/surface_colocation.hpp>
#include <geode/inspector/criterion/degeneration/surface_degeneration.hpp>
#include <geode/inspector/criterion/intersections/surface_intersections.hpp>
#include <geode/inspector/criterion/manifold/surface_edge_manifold.hpp>
#include <geode/inspector/criterion/manifold/surface_vertex_manifold.hpp>
#include <geode/inspector/mixin/add_inspectors.hpp>

namespace geode
{
    struct opengeode_inspector_inspector_api SurfaceInspectionResult
    {
        InspectionIssues< PolygonEdge > polygon_edges_with_wrong_adjacency{
            "Adjacencies between polygon edges not tested"
        };
        InspectionIssues< std::vector< index_t > > colocated_points_groups{
            "Colocation of vertices not tested"
        };
        InspectionIssues< index_t > degenerated_edges{
            "Degeneration of edges not tested"
        };
        InspectionIssues< index_t > degenerated_polygons{
            "Degeneration of polygons not tested"
        };
        InspectionIssues< std::array< index_t, 2 > > non_manifold_edges{
            "Manifold of edges not tested"
        };
        InspectionIssues< index_t > non_manifold_vertices{
            "Manifold of vertices not tested"
        };
        InspectionIssues< std::pair< index_t, index_t > > intersecting_elements{
            "Intersection between mesh elements not tested"
        };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };
    /*!
     * Class for inspecting a SurfaceMesh
     * @extends SurfaceMeshAdjacency
     * @extends SurfaceMeshColocation
     * @extends SurfaceMeshDegeneration
     * @extends SurfaceMeshEdgeManifold
     * @extends SurfaceMeshVertexManifold
     */
    template < index_t dimension >
    class SurfaceMeshInspector : public AddInspectors< SurfaceMesh< dimension >,
                                     SurfaceMeshAdjacency< dimension >,
                                     SurfaceMeshColocation< dimension >,
                                     SurfaceMeshDegeneration< dimension >,
                                     SurfaceMeshEdgeManifold< dimension >,
                                     SurfaceMeshVertexManifold< dimension >,
                                     SurfaceMeshIntersections< dimension > >
    {
        OPENGEODE_DISABLE_COPY( SurfaceMeshInspector );

    public:
        SurfaceMeshInspector( const SurfaceMesh< dimension >& mesh );

        virtual ~SurfaceMeshInspector() = default;

        [[nodiscard]] virtual SurfaceInspectionResult inspect_surface() const;
    };
    ALIAS_2D_AND_3D( SurfaceMeshInspector );
} // namespace geode