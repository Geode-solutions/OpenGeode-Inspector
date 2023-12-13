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

#include <geode/model/representation/core/brep.h>

#include <geode/inspector/criterion/manifold/brep_meshes_manifold.h>

namespace geode
{
    void define_brep_meshes_manifold( pybind11::module& module )
    {
        pybind11::class_< BRepMeshesManifoldInspectionResult >(
            module, "BRepMeshesManifoldInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "meshes_non_manifold_vertices",
                &BRepMeshesManifoldInspectionResult::
                    meshes_non_manifold_vertices )
            .def_readwrite( "meshes_non_manifold_edges",
                &BRepMeshesManifoldInspectionResult::meshes_non_manifold_edges )
            .def_readwrite( "meshes_non_manifold_facets",
                &BRepMeshesManifoldInspectionResult::
                    meshes_non_manifold_facets )
            .def_readwrite( "model_non_manifold_edges",
                &BRepMeshesManifoldInspectionResult::model_non_manifold_edges );

        pybind11::class_< BRepComponentMeshesManifold >(
            module, "BRepComponentMeshesManifold" )
            .def( pybind11::init< const BRep& >() )
            .def( "inspect_brep_manifold",
                &BRepComponentMeshesManifold::inspect_brep_manifold );
    }
} // namespace geode
