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

#include <geode/inspector/solid_inspector.hpp>

#include <async++.h>

#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    index_t SolidInspectionResult::nb_issues() const
    {
        return polyhedron_facets_with_wrong_adjacency.nb_issues()
               + colocated_points_groups.nb_issues()
               + degenerated_edges.nb_issues()
               + degenerated_polyhedra.nb_issues()
               + non_manifold_vertices.nb_issues()
               + non_manifold_edges.nb_issues()
               + non_manifold_facets.nb_issues()
               + negative_polyhedra.nb_issues();
    }

    std::string SolidInspectionResult::string() const
    {
        return absl::StrCat( polyhedron_facets_with_wrong_adjacency.string(),
            colocated_points_groups.string(), degenerated_edges.string(),
            degenerated_polyhedra.string(), non_manifold_vertices.string(),
            non_manifold_edges.string(), non_manifold_facets.string(),
            negative_polyhedra.string() );
    }

    std::string SolidInspectionResult::inspection_type() const
    {
        return "solid inspection";
    }

    template < index_t dimension >
    SolidMeshInspector< dimension >::SolidMeshInspector(
        const SolidMesh< dimension >& mesh )
        : AddInspectors< SolidMesh< dimension >,
              SolidMeshAdjacency< dimension >,
              SolidMeshColocation< dimension >,
              SolidMeshDegeneration< dimension >,
              SolidMeshVertexManifold< dimension >,
              SolidMeshEdgeManifold< dimension >,
              SolidMeshFacetManifold< dimension >,
              SolidMeshNegativeElements< dimension > >{ mesh }
    {
    }

    template < index_t dimension >
    SolidInspectionResult SolidMeshInspector< dimension >::inspect_solid() const
    {
        SolidInspectionResult result;
        async::parallel_invoke(
            [&result, this] {
                result.polyhedron_facets_with_wrong_adjacency =
                    this->polyhedron_facets_with_wrong_adjacency();
            },
            [&result, this] {
                result.colocated_points_groups =
                    this->colocated_points_groups();
            },
            [&result, this] {
                result.degenerated_edges = this->degenerated_edges();
            },
            [&result, this] {
                result.degenerated_polyhedra = this->degenerated_polyhedra();
            },
            [&result, this] {
                result.non_manifold_vertices = this->non_manifold_vertices();
            },
            [&result, this] {
                result.non_manifold_edges = this->non_manifold_edges();
            },
            [&result, this] {
                result.non_manifold_facets = this->non_manifold_facets();
            },
            [&result, this] {
                result.negative_polyhedra = this->negative_polyhedra();
            } );
        return result;
    }

    template class opengeode_inspector_inspector_api SolidMeshInspector< 3 >;
} // namespace geode
