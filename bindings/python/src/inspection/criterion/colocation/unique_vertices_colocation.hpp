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
#include <absl/strings/str_cat.h>

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/inspection/criterion/colocation/unique_vertices_colocation.hpp>

#include <geode/inspector/bindings/python/inspection_helpers.hpp>

namespace geode
{
    template < typename Model >
    void do_define_unique_vertices_colocation(
        pybind11::module& module, const std::string& model_name )
    {
        using UniqueVerticesColocation = UniqueVerticesColocation< Model >;
        const auto name =
            absl::StrCat( model_name, "UniqueVerticesColocation" );
        pybind11::class_< UniqueVerticesColocation >( module, name.c_str(),
            absl::StrCat( "Inspection of the colocation of the unique "
                          "vertices of a ",
                model_name,
                ". The positions of the unique vertices are computed at the "
                "first inspection and kept: create a new inspector after "
                "modifying the model" )
                .c_str() )
            .def( pybind11::init< const Model& >(), pybind11::arg( "model" ),
                pybind11::keep_alive< 1, 2 >(),
                absl::StrCat( "Create the inspector of the unique vertices of "
                              "this ",
                    model_name, ". The inspector keeps the ", model_name,
                    " alive" )
                    .c_str() )
            .def( "model_has_unique_vertices_linked_to_different_points",
                &UniqueVerticesColocation::
                    model_has_unique_vertices_linked_to_different_points,
                ReleaseGil(),
                "Return True if a unique vertex is linked to mesh vertices "
                "that are not colocated" )
            .def( "model_has_colocated_unique_vertices",
                &UniqueVerticesColocation::model_has_colocated_unique_vertices,
                ReleaseGil(),
                "Return True if two unique vertices are colocated (unique "
                "vertices linked to no mesh vertex are ignored)" )
            .def( "inspect_unique_vertices",
                &UniqueVerticesColocation::inspect_unique_vertices,
                ReleaseGil(),
                "Return the UniqueVerticesInspectionResult: the groups of "
                "colocated unique vertices and the unique vertices linked to "
                "mesh vertices at different positions" );
    }

    void define_models_uv_colocation( pybind11::module& module )
    {
        auto result =
            define_inspection_result< UniqueVerticesInspectionResult >( module,
                "UniqueVerticesInspectionResult",
                "Result of the inspection of the colocation of the unique "
                "vertices of a model" );
        INSPECTION_RESULT_MEMBER( result, UniqueVerticesInspectionResult,
            colocated_unique_vertices_groups,
            "InspectionIssuesVectorIndex: the groups of colocated unique "
            "vertices (lists of unique vertex indices)" );
        INSPECTION_RESULT_MEMBER( result, UniqueVerticesInspectionResult,
            unique_vertices_linked_to_different_points,
            "InspectionIssuesIndex: the unique vertices linked to mesh "
            "vertices at different positions" );

        do_define_unique_vertices_colocation< Section >( module, "Section" );
        do_define_unique_vertices_colocation< BRep >( module, "BRep" );
    }
} // namespace geode
