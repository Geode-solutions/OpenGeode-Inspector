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

#include <geode/mesh/core/detail/vertex_cycle.h>
#include <geode/mesh/core/solid_mesh.h>

#include <geode/inspector/criterion/manifold/solid_facet_manifold.h>

namespace geode
{
    template < index_t dimension >
    void do_define_solid_facet_manifold( pybind11::module& module )
    {
        using SolidMesh = SolidMesh< dimension >;
        using SolidMeshFacetManifold = SolidMeshFacetManifold< dimension >;
        const auto name =
            "SolidMeshFacetManifold" + std::to_string( dimension ) + "D";
        pybind11::class_< SolidMeshFacetManifold >( module, name.c_str() )
            .def( pybind11::init< const SolidMesh& >() )
            .def( pybind11::init< const SolidMesh&, bool >() )
            .def( "mesh_facets_are_manifold",
                &SolidMeshFacetManifold::mesh_facets_are_manifold )
            .def( "nb_non_manifold_facets",
                &SolidMeshFacetManifold::nb_non_manifold_facets )
            .def( "non_manifold_facets",
                &SolidMeshFacetManifold::non_manifold_facets );
    }
    void define_solid_facet_manifold( pybind11::module& module )
    {
        do_define_solid_facet_manifold< 3 >( module );
    }
} // namespace geode
