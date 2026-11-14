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

#include <geode/inspector/inspection/criterion/adjacency/surface_adjacency.hpp>

#include <geode/inspector/bindings/python/inspection_helpers.hpp>

namespace geode
{
    template < index_t dimension >
    void do_define_surface_adjacency( pybind11::module& module )
    {
        using SurfaceMesh = SurfaceMesh< dimension >;
        using SurfaceMeshAdjacency = SurfaceMeshAdjacency< dimension >;
        const auto name =
            absl::StrCat( "SurfaceMeshAdjacency", dimension, "D" );
        pybind11::class_< SurfaceMeshAdjacency >( module, name.c_str(),
            "Inspection of the adjacencies between the polygons of a "
            "SurfaceMesh" )
            .def( pybind11::init< const SurfaceMesh& >(),
                pybind11::arg( "mesh" ), pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the adjacencies of the polygons of "
                "this mesh. The inspector keeps the mesh alive" )
            .def( "mesh_has_wrong_adjacencies",
                &SurfaceMeshAdjacency::mesh_has_wrong_adjacencies, ReleaseGil(),
                "Return True if a polygon edge that is not on the border has "
                "a wrong adjacency: the adjacent polygon does not exist, or "
                "does not have the same edge in the opposite direction with "
                "this polygon as adjacent" )
            .def( "polygon_edges_with_wrong_adjacency",
                &SurfaceMeshAdjacency::polygon_edges_with_wrong_adjacency,
                ReleaseGil(),
                "Return the InspectionIssuesPolygonEdge listing every "
                "polygon edge, not on the border, with a wrong adjacency (see "
                "mesh_has_wrong_adjacencies)" );
    }

    void define_surface_adjacency( pybind11::module& module )
    {
        do_define_surface_adjacency< 2 >( module );
        do_define_surface_adjacency< 3 >( module );
    }
} // namespace geode
