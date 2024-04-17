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

#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/section_meshes_inspector.h>

#include <geode/inspector/section_inspector.h>

namespace geode
{
    void define_section_inspector( pybind11::module& module )
    {
        pybind11::class_< SectionInspectionResult >(
            module, "SectionInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "meshes", &SectionInspectionResult::meshes )
            .def_readwrite( "topology", &SectionInspectionResult::topology )
            .def( "string", &SectionInspectionResult::string )
            .def(
                "inspection_type", &SectionInspectionResult::inspection_type );

        pybind11::class_< SectionInspector, SectionMeshesInspector,
            SectionTopologyInspector >( module, "SectionInspector" )
            .def( pybind11::init< const Section& >() )
            .def( "inspect_section", &SectionInspector::inspect_section );

        module.def( "inspect_section", []( const Section& section ) {
            SectionInspector inspector{ section };
            return inspector.inspect_section();
        } );
    }
} // namespace geode
