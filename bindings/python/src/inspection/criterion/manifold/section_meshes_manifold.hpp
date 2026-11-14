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
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/inspection/criterion/manifold/section_meshes_manifold.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    void define_section_meshes_manifold( pybind11::module& module )
    {
        auto result =
            define_inspection_result< SectionMeshesManifoldInspectionResult >(
                module, "SectionMeshesManifoldInspectionResult",
                "Result of the inspection of the manifold property of the "
                "meshes of the Surfaces of a Section" );
        INSPECTION_RESULT_MEMBER( result, SectionMeshesManifoldInspectionResult,
            meshes_non_manifold_vertices,
            "InspectionIssuesMapIndex: the non manifold vertices of the "
            "Surface meshes, by Surface" );
        INSPECTION_RESULT_MEMBER( result, SectionMeshesManifoldInspectionResult,
            meshes_non_manifold_edges,
            "InspectionIssuesMapArrayIndex2: the non manifold edges of the "
            "Surface meshes (pairs of vertex indices), by Surface" );

        pybind11::class_< SectionComponentMeshesManifold >( module,
            "SectionComponentMeshesManifold",
            "Inspection of the manifold property of the meshes of the active "
            "Surfaces of a Section" )
            .def( pybind11::init< const Section& >(),
                pybind11::arg( "section" ), pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the manifold property of the "
                "meshes of this Section. The inspector keeps the Section "
                "alive" )
            .def( "inspect_section_manifold",
                &SectionComponentMeshesManifold::inspect_section_manifold,
                ReleaseGil(),
                "Return the SectionMeshesManifoldInspectionResult: the non "
                "manifold vertices and edges of each Surface mesh, as "
                "SurfaceMeshVertexManifold and SurfaceMeshEdgeManifold find "
                "them" );
    }
} // namespace geode
