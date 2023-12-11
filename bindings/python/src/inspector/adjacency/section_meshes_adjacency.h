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

#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/adjacency/section_meshes_adjacency.h>

namespace geode
{
    void define_section_meshes_adjacency( pybind11::module& module )
    {
        pybind11::class_< SectionMeshesAdjacencyInspectionResult >(
            module, "SectionMeshesAdjacencyInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "meshes_with_wrong_adjacencies",
                &SectionMeshesAdjacencyInspectionResult::
                    meshes_with_wrong_adjacencies )
            .def_readwrite( "surfaces_edges_with_wrong_adjacencies",
                &SectionMeshesAdjacencyInspectionResult::
                    surfaces_edges_with_wrong_adjacencies );

        pybind11::class_< SectionComponentMeshesAdjacency >(
            module, "SectionComponentMeshesAdjacency" )
            .def( pybind11::init< const Section& >() )
            .def( "inspect_section_meshes_adjacencies",
                &SectionComponentMeshesAdjacency::
                    inspect_section_meshes_adjacencies );
    }
} // namespace geode
