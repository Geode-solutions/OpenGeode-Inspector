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

#include <geode/inspector/inspection/section_inspector.hpp>

#include "../inspection_helpers.hpp"

namespace geode
{
    void define_section_inspector( pybind11::module& module )
    {
        auto result = define_inspection_result< SectionInspectionResult >(
            module, "SectionInspectionResult",
            "Result of the inspection of a Section: the inspection of its "
            "meshes and of its topology" );
        INSPECTION_RESULT_MEMBER( result, SectionInspectionResult, meshes,
            "SectionMeshesInspectionResult: the inspection of the meshes" );
        INSPECTION_RESULT_MEMBER( result, SectionInspectionResult, topology,
            "SectionTopologyInspectionResult: the inspection of the "
            "topology" );

        pybind11::class_< SectionInspector, SectionMeshesInspector,
            SectionTopologyInspector >( module, "SectionInspector",
            "Complete inspection of a Section: the inspection of its meshes "
            "(SectionMeshesInspector) and of its topology "
            "(SectionTopologyInspector), whose methods it also has" )
            .def( pybind11::init< const Section& >(),
                pybind11::arg( "section" ), ReleaseGil(),
                "Create the inspector of this Section. The inspector keeps "
                "the Section alive" )
            .def( "inspect_section", &SectionInspector::inspect_section,
                ReleaseGil(),
                "Return the SectionInspectionResult: the inspection of the "
                "meshes and of the topology of the Section" );

        module.def(
            "inspect_section",
            []( const Section& section ) {
                const SectionInspector inspector{ section };
                return inspector.inspect_section();
            },
            pybind11::arg( "section" ), ReleaseGil(),
            "Inspect the Section with a SectionInspector and return the "
            "SectionInspectionResult" );
    }
} // namespace geode
