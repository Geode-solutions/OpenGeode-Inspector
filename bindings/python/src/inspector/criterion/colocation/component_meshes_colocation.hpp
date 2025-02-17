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

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/colocation/component_meshes_colocation.hpp>

#define PYTHON_COMPONENTS_COLOCATION( type, suffix )                           \
    const auto name##type =                                                    \
        absl::StrCat( #type, "ComponentMeshesColocation" );                    \
    pybind11::class_< suffix##ComponentMeshesColocation >(                     \
        module, name##type.c_str() )                                           \
        .def( pybind11::init< const type& >() )                                \
        .def( "inspect_meshes_point_colocations",                              \
            &suffix##ComponentMeshesColocation::                               \
                inspect_meshes_point_colocations )

namespace geode
{
    void define_models_meshes_colocation( pybind11::module& module )
    {
        pybind11::class_< MeshesColocationInspectionResult >(
            module, "MeshesColocationInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "colocated_points_groups",
                &MeshesColocationInspectionResult::colocated_points_groups )
            .def( "string", &MeshesColocationInspectionResult::string )
            .def( "inspection_type",
                &MeshesColocationInspectionResult::inspection_type );

        PYTHON_COMPONENTS_COLOCATION( Section, Section );
        PYTHON_COMPONENTS_COLOCATION( BRep, BRep );
    }
} // namespace geode
