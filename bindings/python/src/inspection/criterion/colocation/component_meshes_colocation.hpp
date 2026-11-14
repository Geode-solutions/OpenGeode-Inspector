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

#include <geode/inspector/inspection/criterion/colocation/component_meshes_colocation.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    template < typename Model >
    void do_define_component_meshes_colocation(
        pybind11::module& module, const std::string& model_name )
    {
        using ComponentMeshesColocation = ComponentMeshesColocation< Model >;
        const auto name =
            absl::StrCat( model_name, "ComponentMeshesColocation" );
        pybind11::class_< ComponentMeshesColocation >( module, name.c_str(),
            absl::StrCat( "Inspection of the colocated vertices in the "
                          "meshes of the active Lines, Surfaces",
                std::is_same_v< Model, BRep > ? " and Blocks" : "", " of a ",
                model_name )
                .c_str() )
            .def( pybind11::init< const Model& >(), pybind11::arg( "model" ),
                pybind11::keep_alive< 1, 2 >(),
                absl::StrCat( "Create the inspector of the colocated "
                              "vertices of the meshes of this ",
                    model_name, ". The inspector keeps the ", model_name,
                    " alive" )
                    .c_str() )
            .def( "inspect_meshes_point_colocations",
                &ComponentMeshesColocation::inspect_meshes_point_colocations,
                ReleaseGil(),
                "Return the MeshesColocationInspectionResult: for each "
                "component, the groups of colocated vertices of its mesh "
                "(as the colocation inspectors of the meshes find them), where "
                "the vertices linked to the same unique vertex count as "
                "one: a vertex linked to the same unique vertex as another "
                "vertex of its group is not reported, and a group left with "
                "one vertex is dropped" );
    }

    void define_models_meshes_colocation( pybind11::module& module )
    {
        auto result =
            define_inspection_result< MeshesColocationInspectionResult >(
                module, "MeshesColocationInspectionResult",
                "Result of the inspection of the colocated vertices in the "
                "meshes of the components of a model" );
        INSPECTION_RESULT_MEMBER( result, MeshesColocationInspectionResult,
            colocated_points_groups,
            "InspectionIssuesMapVectorIndex: the groups of colocated "
            "vertices (lists of vertex indices), by component" );

        do_define_component_meshes_colocation< Section >( module, "Section" );
        do_define_component_meshes_colocation< BRep >( module, "BRep" );
    }
} // namespace geode
