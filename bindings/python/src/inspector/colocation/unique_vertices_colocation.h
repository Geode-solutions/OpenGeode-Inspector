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

#include <geode/inspector/criterion/colocation/unique_vertices_colocation.h>

#define PYTHON_SECTION_UV_COLOCATION()                                         \
    pybind11::class_< SectionUniqueVerticesColocation >(                       \
        module, "SectionUniqueVerticesColocation" )                            \
        .def( pybind11::init< const Section& >() )                             \
        .def( pybind11::init< const Section&, bool >() )                       \
        .def( "model_has_unique_vertices_linked_to_different_points",          \
            &SectionUniqueVerticesColocation::                                 \
                model_has_unique_vertices_linked_to_different_points )         \
        .def( "model_has_colocated_unique_vertices",                           \
            &SectionUniqueVerticesColocation::                                 \
                model_has_colocated_unique_vertices )                          \
        .def( "nb_colocated_unique_vertices",                                  \
            &SectionUniqueVerticesColocation::nb_colocated_unique_vertices )   \
        .def( "nb_unique_vertices_linked_to_different_points",                 \
            &SectionUniqueVerticesColocation::                                 \
                nb_unique_vertices_linked_to_different_points )                \
        .def( "colocated_unique_vertices_groups",                              \
            &SectionUniqueVerticesColocation::                                 \
                colocated_unique_vertices_groups )                             \
        .def( "unique_vertices_linked_to_different_points",                    \
            &SectionUniqueVerticesColocation::                                 \
                unique_vertices_linked_to_different_points )

#define PYTHON_BREP_UV_COLOCATION()                                            \
    pybind11::class_< BRepUniqueVerticesColocation >(                          \
        module, "BRepUniqueVerticesColocation" )                               \
        .def( pybind11::init< const BRep& >() )                                \
        .def( pybind11::init< const BRep&, bool >() )                          \
        .def( "model_has_unique_vertices_linked_to_different_points",          \
            &BRepUniqueVerticesColocation::                                    \
                model_has_unique_vertices_linked_to_different_points )         \
        .def( "model_has_colocated_unique_vertices",                           \
            &BRepUniqueVerticesColocation::                                    \
                model_has_colocated_unique_vertices )                          \
        .def( "nb_colocated_unique_vertices",                                  \
            &BRepUniqueVerticesColocation::nb_colocated_unique_vertices )      \
        .def( "nb_unique_vertices_linked_to_different_points",                 \
            &BRepUniqueVerticesColocation::                                    \
                nb_unique_vertices_linked_to_different_points )                \
        .def( "colocated_unique_vertices_groups",                              \
            &BRepUniqueVerticesColocation::colocated_unique_vertices_groups )  \
        .def( "unique_vertices_linked_to_different_points",                    \
            &BRepUniqueVerticesColocation::                                    \
                unique_vertices_linked_to_different_points )

namespace geode
{
    void define_models_uv_colocation( pybind11::module& module )
    {
        PYTHON_SECTION_UV_COLOCATION();
        PYTHON_BREP_UV_COLOCATION();
    }
} // namespace geode
