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

#include <geode/mesh/core/surface_mesh.h>

#include <geode/inspector/criterion/manifold/surface_vertex_manifold.h>

namespace
{
    template < geode::index_t dimension >
    void do_define_surface_vertex_manifold( pybind11::module& module )
    {
        using SurfaceMesh = geode::SurfaceMesh< dimension >;
        using SurfaceMeshVertexManifold =
            geode::SurfaceMeshVertexManifold< dimension >;
        const auto name =
            "SurfaceMeshVertexManifold" + std::to_string( dimension ) + "D";
        pybind11::class_< SurfaceMeshVertexManifold >( module, name.c_str() )
            .def( pybind11::init< const SurfaceMesh& >() )
            .def( pybind11::init< const SurfaceMesh&, bool >() )
            .def( "mesh_vertices_are_manifold",
                &SurfaceMeshVertexManifold::mesh_vertices_are_manifold )
            .def( "nb_non_manifold_vertices",
                &SurfaceMeshVertexManifold::nb_non_manifold_vertices )
            .def( "non_manifold_vertices",
                &SurfaceMeshVertexManifold::non_manifold_vertices );
    }
} // namespace

namespace geode
{
    void define_surface_vertex_manifold( pybind11::module& module )
    {
        do_define_surface_vertex_manifold< 2 >( module );
        do_define_surface_vertex_manifold< 3 >( module );
    }
} // namespace geode
