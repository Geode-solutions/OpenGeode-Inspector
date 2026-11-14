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
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/inspection/criterion/negative_elements/brep_meshes_negative_elements.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    void define_brep_meshes_negative_elements( pybind11::module& module )
    {
        auto result = define_inspection_result<
            BRepMeshesNegativeElementsInspectionResult >( module,
            "BRepMeshesNegativeElementsInspectionResult",
            "Result of the inspection of the negative polyhedra in the "
            "meshes of the Blocks of a BRep" );
        INSPECTION_RESULT_MEMBER( result,
            BRepMeshesNegativeElementsInspectionResult, negative_polyhedra,
            "InspectionIssuesMapIndex: the indices of the polyhedra with a "
            "negative volume, by Block" );

        pybind11::class_< BRepComponentMeshesNegativeElements >( module,
            "BRepComponentMeshesNegativeElements",
            "Inspection of the negative polyhedra in the meshes of the "
            "active Blocks of a BRep" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                "Create the inspector of the negative polyhedra of this BRep. "
                "The inspector keeps the BRep alive" )
            .def( "inspect_negative_elements",
                &BRepComponentMeshesNegativeElements::inspect_negative_elements,
                ReleaseGil(),
                "Return the BRepMeshesNegativeElementsInspectionResult: the "
                "polyhedra with a negative volume of each Block, as "
                "SolidMeshNegativeElements finds them" );
    }
} // namespace geode
