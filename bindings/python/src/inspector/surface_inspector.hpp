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
#include <string>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/inspector/surface_inspector.hpp>

namespace geode
{
    template < index_t dimension >
    void do_define_surface_inspector( pybind11::module& module )
    {
        using SurfaceMesh = SurfaceMesh< dimension >;
        using SurfaceMeshInspector = SurfaceMeshInspector< dimension >;
        const auto name =
            absl::StrCat( "SurfaceMeshInspector", dimension, "D" );
        pybind11::class_< SurfaceMeshInspector,
            SurfaceMeshAdjacency< dimension >,
            SurfaceMeshColocation< dimension >,
            SurfaceMeshDegeneration< dimension >,
            SurfaceMeshEdgeManifold< dimension >,
            SurfaceMeshVertexManifold< dimension >,
            SurfaceMeshIntersections< dimension > >( module, name.c_str() )
            .def( pybind11::init< const SurfaceMesh& >() )
            .def( "inspect_surface", &SurfaceMeshInspector::inspect_surface );

        const auto inspect_function_name =
            absl::StrCat( "inspect_surface", dimension, "D" );
        module.def(
            inspect_function_name.c_str(), []( const SurfaceMesh& surface ) {
                SurfaceMeshInspector inspector{ surface };
                return inspector.inspect_surface();
            } );
    }

    void define_surface_inspector( pybind11::module& module )
    {
        pybind11::class_< SurfaceInspectionResult >(
            module, "SurfaceInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "polygon_edges_with_wrong_adjacency",
                &SurfaceInspectionResult::polygon_edges_with_wrong_adjacency )
            .def_readwrite( "colocated_points_groups",
                &SurfaceInspectionResult::colocated_points_groups )
            .def_readwrite( "degenerated_edges",
                &SurfaceInspectionResult::degenerated_edges )
            .def_readwrite( "degenerated_polygons",
                &SurfaceInspectionResult::degenerated_polygons )
            .def_readwrite( "non_manifold_edges",
                &SurfaceInspectionResult::non_manifold_edges )
            .def_readwrite( "non_manifold_vertices",
                &SurfaceInspectionResult::non_manifold_vertices )
            .def_readwrite( "intersecting_elements",
                &SurfaceInspectionResult::intersecting_elements )
            .def( "string", &SurfaceInspectionResult::string )
            .def(
                "inspection_type", &SurfaceInspectionResult::inspection_type );

        do_define_surface_inspector< 2 >( module );
        do_define_surface_inspector< 3 >( module );
    }
} // namespace geode
