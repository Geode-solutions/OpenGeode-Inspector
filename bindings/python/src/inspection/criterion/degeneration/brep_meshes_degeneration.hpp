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

#include <geode/inspector/inspection/criterion/degeneration/brep_meshes_degeneration.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    void define_brep_meshes_degeneration( pybind11::module& module )
    {
        auto result =
            define_inspection_result< BRepMeshesDegenerationInspectionResult >(
                module, "BRepMeshesDegenerationInspectionResult",
                "Result of the inspection of the degenerated elements in the "
                "meshes of the Lines, Surfaces and Blocks of a BRep" );
        INSPECTION_RESULT_MEMBER( result,
            BRepMeshesDegenerationInspectionResult, degenerated_edges,
            "InspectionIssuesMapIndex: the indices of the degenerated edges, "
            "by Line, Surface and Block (edge indices of the mesh)" );
        INSPECTION_RESULT_MEMBER( result,
            BRepMeshesDegenerationInspectionResult, degenerated_polygons,
            "InspectionIssuesMapIndex: the indices of the degenerated "
            "polygons, by Surface" );
        INSPECTION_RESULT_MEMBER( result,
            BRepMeshesDegenerationInspectionResult, degenerated_polyhedra,
            "InspectionIssuesMapIndex: the indices of the degenerated "
            "polyhedra, by Block" );

        pybind11::class_< BRepComponentMeshesDegeneration >( module,
            "BRepComponentMeshesDegeneration",
            "Inspection of the degenerated edges, polygons and polyhedra in "
            "the meshes of the active Lines, Surfaces and Blocks of a BRep. "
            "The edges of the Surface and Block meshes are enabled when an "
            "inspection finds small edges in them, and disabled again when "
            "the inspector is destroyed (if they were not enabled): do not "
            "modify the BRep while an inspector exists, and do not inspect "
            "or use the same mesh or model from two threads at once" )
            .def( pybind11::init< const BRep& >(), pybind11::arg( "brep" ),
                pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the degenerated elements of the "
                "meshes of this BRep. The inspector keeps the BRep alive" )
            .def( "inspect_elements_degeneration",
                &BRepComponentMeshesDegeneration::inspect_elements_degeneration,
                ReleaseGil(),
                "Return the BRepMeshesDegenerationInspectionResult: the "
                "edges and polygons and polyhedra of the meshes that are "
                "degenerated (shorter than GLOBAL_EPSILON (1e-6), or with a "
                "minimum height of at most GLOBAL_EPSILON (1e-6))" )
            .def( "inspect_edges_degeneration",
                &BRepComponentMeshesDegeneration::inspect_edges_degeneration,
                pybind11::arg( "threshold" ), ReleaseGil(),
                "Return the BRepMeshesDegenerationInspectionResult with the "
                "edges (of the Lines, Surfaces and Blocks) shorter than "
                "threshold in degenerated_edges, and nothing else" )
            .def( "inspect_polygons_degeneration",
                &BRepComponentMeshesDegeneration::inspect_polygons_degeneration,
                pybind11::arg( "threshold" ), ReleaseGil(),
                "Return the BRepMeshesDegenerationInspectionResult with the "
                "polygons of the Surfaces whose minimum height is at most "
                "threshold in degenerated_polygons, and nothing else" )
            .def( "inspect_polyhedra_degeneration",
                &BRepComponentMeshesDegeneration::
                    inspect_polyhedra_degeneration,
                pybind11::arg( "threshold" ), ReleaseGil(),
                "Return the BRepMeshesDegenerationInspectionResult with the "
                "polyhedra of the Blocks whose minimum height is at most "
                "threshold in degenerated_polyhedra, and the edges of the "
                "Blocks shorter than threshold in degenerated_edges" );
    }
} // namespace geode
