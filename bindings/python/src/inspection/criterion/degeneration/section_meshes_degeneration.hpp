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

#include <geode/inspector/inspection/criterion/degeneration/section_meshes_degeneration.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    void define_section_meshes_degeneration( pybind11::module& module )
    {
        auto result = define_inspection_result<
            SectionMeshesDegenerationInspectionResult >( module,
            "SectionMeshesDegenerationInspectionResult",
            "Result of the inspection of the degenerated elements in the "
            "meshes of the Lines and Surfaces of a Section" );
        INSPECTION_RESULT_MEMBER( result,
            SectionMeshesDegenerationInspectionResult, degenerated_edges,
            "InspectionIssuesMapIndex: the indices of the degenerated edges, "
            "by Line and Surface (edge indices of the mesh)" );
        INSPECTION_RESULT_MEMBER( result,
            SectionMeshesDegenerationInspectionResult, degenerated_polygons,
            "InspectionIssuesMapIndex: the indices of the degenerated "
            "polygons, by Surface" );

        pybind11::class_< SectionComponentMeshesDegeneration >( module,
            "SectionComponentMeshesDegeneration",
            "Inspection of the degenerated edges and polygons in the meshes "
            "of the active Lines and Surfaces of a Section. The edges of the "
            "Surface meshes are enabled when an inspection finds small edges "
            "in them, and disabled again when the inspector is destroyed (if "
            "they were not enabled): do not modify the Section while an "
            "inspector exists, and do not inspect or use the same mesh or "
            "model from two threads at once" )
            .def( pybind11::init< const Section& >(),
                pybind11::arg( "section" ),
                "Create the inspector of the degenerated elements of the "
                "meshes of this Section. The inspector keeps the Section "
                "alive" )
            .def( "inspect_elements_degeneration",
                &SectionComponentMeshesDegeneration::
                    inspect_elements_degeneration,
                ReleaseGil(),
                "Return the SectionMeshesDegenerationInspectionResult: the "
                "edges and polygons of the meshes that are degenerated "
                "(shorter than GLOBAL_EPSILON (1e-6), or with a minimum "
                "height of at most GLOBAL_EPSILON (1e-6))" );
    }
} // namespace geode
