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

#include <geode/inspector/criterion/degeneration/component_meshes_degeneration.h>

#define PYTHON_COMPONENTS_DEGENERATION( type, suffix )                         \
    const auto name##type =                                                    \
        absl::StrCat( #type, "ComponentMeshesDegeneration" );                  \
    pybind11::class_< suffix##ComponentMeshesDegeneration >(                   \
        module, name##type.c_str() )                                           \
        .def( pybind11::init< const type& >() )                                \
        .def( pybind11::init< const type&, bool >() )                          \
        .def( "degenerated_component_meshes",                                  \
            &suffix##ComponentMeshesDegeneration::                             \
                degenerated_component_meshes )                                 \
        .def( "components_nb_degenerated_elements",                            \
            &suffix##ComponentMeshesDegeneration::                             \
                components_nb_degenerated_elements )                           \
        .def( "components_degenerated_elements",                               \
            &suffix##ComponentMeshesDegeneration::                             \
                components_degenerated_elements )

namespace geode
{
    void define_models_meshes_degeneration( pybind11::module& module )
    {
        PYTHON_COMPONENTS_DEGENERATION( Section, Section );
        PYTHON_COMPONENTS_DEGENERATION( BRep, BRep );
    }
} // namespace geode
