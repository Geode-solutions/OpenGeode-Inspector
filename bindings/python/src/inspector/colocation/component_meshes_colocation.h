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

#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/colocation/component_meshes_colocation.h>

#define PYTHON_COMPONENTS_COLOCATION( type, suffix )                           \
    const auto name##type =                                                    \
        absl::StrCat( #type, "ComponentMeshesColocation" );                    \
    pybind11::class_< suffix##ComponentMeshesColocation >(                     \
        module, name##type.c_str() )                                           \
        .def( pybind11::init< const type& >() )                                \
        .def( pybind11::init< const type&, bool >() )                          \
        .def( "components_with_colocated_points",                              \
            &suffix##ComponentMeshesColocation::                               \
                components_with_colocated_points )                             \
        .def( "components_nb_colocated_points",                                \
            &suffix##ComponentMeshesColocation::                               \
                components_nb_colocated_points )                               \
        .def( "components_colocated_points_groups",                            \
            &suffix##ComponentMeshesColocation::                               \
                components_colocated_points_groups )

namespace geode
{
    void define_models_meshes_colocation( pybind11::module& module )
    {
        PYTHON_COMPONENTS_COLOCATION( Section, Section );
        PYTHON_COMPONENTS_COLOCATION( BRep, BRep );
    }
} // namespace geode
