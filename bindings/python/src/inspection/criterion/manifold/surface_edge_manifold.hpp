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
#include <absl/strings/str_cat.h>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/inspector/inspection/criterion/manifold/surface_edge_manifold.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    template < index_t dimension >
    void do_define_surface_edge_manifold( pybind11::module& module )
    {
        using SurfaceMesh = SurfaceMesh< dimension >;
        using SurfaceMeshEdgeManifold = SurfaceMeshEdgeManifold< dimension >;
        const auto name =
            absl::StrCat( "SurfaceMeshEdgeManifold", dimension, "D" );
        pybind11::class_< SurfaceMeshEdgeManifold >( module, name.c_str(),
            "Inspection of the manifold property of the edges of a "
            "SurfaceMesh" )
            .def( pybind11::init< const SurfaceMesh& >(),
                pybind11::arg( "mesh" ),
                "Create the inspector of the manifold property of this mesh. "
                "The inspector keeps the mesh alive" )
            .def( "mesh_edges_are_manifold",
                &SurfaceMeshEdgeManifold::mesh_edges_are_manifold, ReleaseGil(),
                "Return True if every edge is manifold: an edge on the border "
                "has exactly one polygon, any other edge has exactly two" )
            .def( "non_manifold_edges",
                &SurfaceMeshEdgeManifold::non_manifold_edges, ReleaseGil(),
                "Return the InspectionIssuesArrayIndex listing the non "
                "manifold edges (see mesh_edges_are_manifold), each one given "
                "by the indices of its two vertices" );
    }

    void define_surface_edge_manifold( pybind11::module& module )
    {
        do_define_surface_edge_manifold< 2 >( module );
        do_define_surface_edge_manifold< 3 >( module );
    }
} // namespace geode
