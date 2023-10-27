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
#include <string>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/mesh/core/detail/vertex_cycle.h>

#include <geode/inspector/criterion/manifold/surface_edge_manifold.h>

namespace geode
{
    template < index_t dimension >
    void do_define_surface_edge_manifold( pybind11::module& module )
    {
        using SurfaceMesh = SurfaceMesh< dimension >;
        using SurfaceMeshEdgeManifold = SurfaceMeshEdgeManifold< dimension >;
        const auto name =
            "SurfaceMeshEdgeManifold" + absl::StrCat( dimension ) + "D";
        pybind11::class_< SurfaceMeshEdgeManifold >( module, name.c_str() )
            .def( pybind11::init< const SurfaceMesh& >() )
            .def( pybind11::init< const SurfaceMesh&, bool >() )
            .def( "mesh_edges_are_manifold",
                &SurfaceMeshEdgeManifold::mesh_edges_are_manifold )
            .def( "nb_non_manifold_edges",
                &SurfaceMeshEdgeManifold::nb_non_manifold_edges )
            .def( "non_manifold_edges",
                &SurfaceMeshEdgeManifold::non_manifold_edges );
    }
} // namespace geode

namespace geode
{
    void define_surface_edge_manifold( pybind11::module& module )
    {
        do_define_surface_edge_manifold< 2 >( module );
        do_define_surface_edge_manifold< 3 >( module );
    }
} // namespace geode
