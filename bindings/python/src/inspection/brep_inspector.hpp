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

#include <geode/inspector/inspection/brep_inspector.hpp>

#include "../inspection_helpers.hpp"

namespace geode
{
    void define_brep_inspector( pybind11::module& module )
    {
        auto result = define_inspection_result< BRepInspectionResult >( module,
            "BRepInspectionResult",
            "Result of the inspection of a BRep: the inspection of its "
            "meshes and of its topology" );
        INSPECTION_RESULT_MEMBER( result, BRepInspectionResult, meshes,
            "BRepMeshesInspectionResult: the inspection of the meshes" );
        INSPECTION_RESULT_MEMBER( result, BRepInspectionResult, topology,
            "BRepTopologyInspectionResult: the inspection of the topology" );

        pybind11::class_< BRepInspector, BRepMeshesInspector,
            BRepTopologyInspector >( module, "BRepInspector",
            pybind11::dynamic_attr(),
            "Complete inspection of a BRep: the inspection of its meshes "
            "(BRepMeshesInspector) and of its topology "
            "(BRepTopologyInspector), whose methods it also has" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                pybind11::keep_alive< 1, 2 >(), remember_model(), ReleaseGil(),
                "Create the inspector of this BRep. The inspector keeps the "
                "BRep alive" )
            .def( "inspect_brep", &BRepInspector::inspect_brep, ReleaseGil(),
                "Return the BRepInspectionResult: the inspection of the "
                "meshes and of the topology of the BRep" );

        module.def(
            "inspect_brep",
            []( const BRep& brep ) {
                const BRepInspector inspector{ brep };
                return inspector.inspect_brep();
            },
            pybind11::arg( "brep" ), ReleaseGil(),
            "Inspect the BRep with a BRepInspector and return the "
            "BRepInspectionResult" );
    }
} // namespace geode
