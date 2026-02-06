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

#include <geode/inspector/criterion/degeneration/brep_meshes_degeneration.hpp>

namespace geode
{
    void define_brep_meshes_degeneration( pybind11::module& module )
    {
        pybind11::class_< BRepMeshesDegenerationInspectionResult >(
            module, "BRepMeshesDegenerationInspectionResult" )
            .def( pybind11::init<>() )
            .def_readwrite( "degenerated_edges",
                &BRepMeshesDegenerationInspectionResult::degenerated_edges )
            .def_readwrite( "degenerated_polygons",
                &BRepMeshesDegenerationInspectionResult::degenerated_polygons )
            .def_readwrite( "degenerated_polyhedra",
                &BRepMeshesDegenerationInspectionResult::degenerated_polyhedra )
            .def( "string", &BRepMeshesDegenerationInspectionResult::string )
            .def( "inspection_type",
                &BRepMeshesDegenerationInspectionResult::inspection_type );

        pybind11::class_< BRepComponentMeshesDegeneration >(
            module, "BRepComponentMeshesDegeneration" )
            .def( pybind11::init< const BRep& >() )
            .def( "inspect_elements_degeneration",
                &BRepComponentMeshesDegeneration::
                    inspect_elements_degeneration )
            .def( "inspect_polygons_degeneration",
                &BRepComponentMeshesDegeneration::
                    inspect_polygons_degeneration )
            .def( "inspect_edges_degeneration",
                &BRepComponentMeshesDegeneration::inspect_edges_degeneration )
            .def( "inspect_polyhedra_degeneration",
                &BRepComponentMeshesDegeneration::
                    inspect_polyhedra_degeneration );
    }
} // namespace geode
