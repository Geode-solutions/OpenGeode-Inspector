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

#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/manifold/section_meshes_manifold.hpp>

namespace geode
{
    void define_section_meshes_manifold( pybind11::module& module )
    {
        pybind11::class_< SectionMeshesManifoldInspectionResult >(
            module, "SectionMeshesManifoldInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "meshes_non_manifold_vertices",
                &SectionMeshesManifoldInspectionResult::
                    meshes_non_manifold_vertices )
            .def_readwrite( "meshes_non_manifold_edges",
                &SectionMeshesManifoldInspectionResult::
                    meshes_non_manifold_edges )
            .def( "string", &SectionMeshesManifoldInspectionResult::string )
            .def( "inspection_type",
                &SectionMeshesManifoldInspectionResult::inspection_type );

        pybind11::class_< SectionComponentMeshesManifold >(
            module, "SectionComponentMeshesManifold" )
            .def( pybind11::init< const Section& >() )
            .def( "inspect_section_manifold",
                &SectionComponentMeshesManifold::inspect_section_manifold );
    }
} // namespace geode
