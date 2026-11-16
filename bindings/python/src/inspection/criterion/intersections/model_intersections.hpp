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

#include <geode/inspector/inspection/criterion/intersections/model_intersections.hpp>

#include <geode/inspector/bindings/python/inspection_helpers.hpp>

namespace geode
{
    template < typename Model >
    void do_define_model_intersections(
        pybind11::module& module, const std::string& model_name )
    {
        using ModelMeshesIntersections = ModelMeshesIntersections< Model >;
        const auto name = absl::StrCat( model_name, "MeshesIntersections" );
        pybind11::class_< ModelMeshesIntersections >( module, name.c_str(),
            absl::StrCat( "Inspection of the intersections between the "
                          "meshes of the Lines and Surfaces of a ",
                model_name,
                ". The AABB trees of the meshes are built when the "
                "inspector is created" )
                .c_str() )
            .def( pybind11::init< const Model& >(), pybind11::arg( "model" ),
                pybind11::keep_alive< 1, 2 >(), ReleaseGil(),
                absl::StrCat(
                    "Create the inspector of the intersections of the meshes "
                    "of this ",
                    model_name, ". The inspector keeps the ", model_name,
                    " alive" )
                    .c_str() )
            .def( "model_has_intersecting_surfaces",
                &ModelMeshesIntersections::model_has_intersecting_surfaces,
                ReleaseGil(),
                "Return True if two polygons of the Surface meshes intersect, "
                "whether they are in two Surfaces or in the same one (a "
                "contact at unique vertices they share is not an "
                "intersection)" )
            .def( "inspect_intersections",
                &ModelMeshesIntersections::inspect_intersections, ReleaseGil(),
                "Return the ElementsIntersectionsInspectionResult listing "
                "the pairs of intersecting elements: polygons of two "
                "Surfaces, edges of a Line with polygons of a Surface, and "
                "edges of two Lines" )
            .def( "inspect_surfaces_self_intersections",
                &ModelMeshesIntersections::inspect_surfaces_self_intersections,
                ReleaseGil(),
                "Return the ElementsIntersectionsInspectionResult listing "
                "the pairs of intersecting polygons inside the mesh of each "
                "Surface" );
    }

    void define_model_intersections( pybind11::module& module )
    {
        auto result =
            define_inspection_result< ElementsIntersectionsInspectionResult >(
                module, "ElementsIntersectionsInspectionResult",
                "Result of the inspection of the intersections between the "
                "meshes of the components of a model" );
        INSPECTION_RESULT_MEMBER( result, ElementsIntersectionsInspectionResult,
            elements_intersections,
            "InspectionIssuesPairComponentMeshElement: the pairs of "
            "intersecting elements, each one given by its component and its "
            "element index" );

        do_define_model_intersections< Section >( module, "Section" );
        do_define_model_intersections< BRep >( module, "BRep" );
    }
} // namespace geode
