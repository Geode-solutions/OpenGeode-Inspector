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

#include <geode/inspector/criterion/adjacency/brep_meshes_adjacency.h>

namespace geode
{
    void define_brep_meshes_adjacency( pybind11::module& module )
    {
        pybind11::class_< BRepMeshesAdjacencyInspectionResult >(
            module, "BRepMeshesAdjacencyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "surfaces_edges_with_wrong_adjacencies",
                &BRepMeshesAdjacencyInspectionResult::
                    surfaces_edges_with_wrong_adjacencies )
            .def_readwrite( "blocks_facets_with_wrong_adjacencies",
                &BRepMeshesAdjacencyInspectionResult::
                    blocks_facets_with_wrong_adjacencies )
            .def( "string", &BRepMeshesAdjacencyInspectionResult::string )
            .def( "inspection_type",
                &BRepMeshesAdjacencyInspectionResult::inspection_type );

        pybind11::class_< BRepComponentMeshesAdjacency >(
            module, "BRepComponentMeshesAdjacency" )
            .def( pybind11::init< const BRep& >() )
            .def( "inspect_brep_meshes_adjacencies",
                &BRepComponentMeshesAdjacency::
                    inspect_brep_meshes_adjacencies );
    }
} // namespace geode
