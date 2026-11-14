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

#include <geode/inspector/inspection/criterion/negative_elements/section_meshes_negative_elements.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    void define_section_meshes_negative_elements( pybind11::module& module )
    {
        auto result = define_inspection_result<
            SectionMeshesNegativeElementsInspectionResult >( module,
            "SectionMeshesNegativeElementsInspectionResult",
            "Result of the inspection of the negative polygons in the "
            "meshes of the Surfaces of a Section" );
        INSPECTION_RESULT_MEMBER( result,
            SectionMeshesNegativeElementsInspectionResult, negative_polygons,
            "InspectionIssuesMapIndex: the indices of the polygons with a "
            "negative area, by Surface" );

        pybind11::class_< SectionComponentMeshesNegativeElements >( module,
            "SectionComponentMeshesNegativeElements",
            "Inspection of the negative polygons in the meshes of the active "
            "Surfaces of a Section" )
            .def( pybind11::init< const Section& >(),
                pybind11::arg( "section" ),
                "Create the inspector of the negative polygons of this "
                "Section. The inspector keeps the Section alive" )
            .def( "inspect_negative_elements",
                &SectionComponentMeshesNegativeElements::
                    inspect_negative_elements,
                ReleaseGil(),
                "Return the SectionMeshesNegativeElementsInspectionResult: "
                "the polygons with a negative area of each Surface, as "
                "SurfaceMeshNegativeElements finds them" );
    }
} // namespace geode
