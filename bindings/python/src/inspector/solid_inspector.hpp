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
#include <string>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/inspector/solid_inspector.hpp>

namespace geode
{
    template < geode::index_t dimension >
    void do_define_solid_inspector( pybind11::module& module )
    {
        using SolidMesh = geode::SolidMesh< dimension >;
        using SolidMeshInspector = geode::SolidMeshInspector< dimension >;
        const auto name = absl::StrCat( "SolidMeshInspector", dimension, "D" );
        pybind11::class_< SolidMeshInspector, SolidMeshAdjacency< dimension >,
            SolidMeshColocation< dimension >,
            SolidMeshDegeneration< dimension >,
            SolidMeshVertexManifold< dimension >,
            SolidMeshEdgeManifold< dimension >,
            SolidMeshFacetManifold< dimension > >( module, name.c_str() )
            .def( pybind11::init< const SolidMesh& >() )
            .def( "inspect_solid", &SolidMeshInspector::inspect_solid );

        const auto inspect_function_name =
            absl::StrCat( "inspect_solid", dimension, "D" );
        module.def(
            inspect_function_name.c_str(), []( const SolidMesh& solid ) {
                SolidMeshInspector inspector{ solid };
                return inspector.inspect_solid();
            } );
    }

    void define_solid_inspector( pybind11::module& module )
    {
        pybind11::class_< SolidInspectionResult >(
            module, "SolidInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "polyhedron_facets_with_wrong_adjacency",
                &SolidInspectionResult::polyhedron_facets_with_wrong_adjacency )
            .def_readwrite( "colocated_points_groups",
                &SolidInspectionResult::colocated_points_groups )
            .def_readwrite(
                "degenerated_edges", &SolidInspectionResult::degenerated_edges )
            .def_readwrite( "degenerated_polyhedra",
                &SolidInspectionResult::degenerated_polyhedra )
            .def_readwrite( "non_manifold_vertices",
                &SolidInspectionResult::non_manifold_vertices )
            .def_readwrite( "non_manifold_edges",
                &SolidInspectionResult::non_manifold_edges )
            .def_readwrite( "non_manifold_facets",
                &SolidInspectionResult::non_manifold_facets )
            .def( "string", &SolidInspectionResult::string )
            .def( "inspection_type", &SolidInspectionResult::inspection_type );
        do_define_solid_inspector< 3 >( module );
    }
} // namespace geode
