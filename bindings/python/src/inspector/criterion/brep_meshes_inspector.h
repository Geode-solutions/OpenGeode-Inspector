/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/model/representation/core/brep.h>

#include <geode/inspector/criterion/brep_meshes_inspector.h>

namespace geode
{
    void define_brep_meshes_inspector( pybind11::module& module )
    {
        pybind11::class_< BRepMeshesInspectionResult >(
            module, "BRepMeshesInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "unique_vertices_colocation",
                &BRepMeshesInspectionResult::unique_vertices_colocation )
            .def_readwrite( "meshes_colocation",
                &BRepMeshesInspectionResult::meshes_colocation )
            .def_readwrite(
                "adjacencies", &BRepMeshesInspectionResult::adjacencies )
            .def_readwrite(
                "degenerations", &BRepMeshesInspectionResult::degenerations )
            .def_readwrite(
                "intersections", &BRepMeshesInspectionResult::intersections )
            .def_readwrite(
                "manifolds", &BRepMeshesInspectionResult::manifolds )
            .def( "string", &BRepMeshesInspectionResult::string );

        pybind11::class_< BRepMeshesInspector, BRepUniqueVerticesColocation,
            BRepComponentMeshesAdjacency, BRepComponentMeshesColocation,
            BRepComponentMeshesDegeneration, BRepComponentMeshesManifold,
            BRepMeshesIntersections >( module, "BRepMeshesInspector" )
            .def( pybind11::init< const BRep& >() )
            .def( "inspect_brep_meshes",
                &BRepMeshesInspector::inspect_brep_meshes );
    }
} // namespace geode
