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

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/inspector/inspection/criterion/degeneration/solid_degeneration.hpp>

#include "../../../inspection_helpers.hpp"

namespace geode
{
    template < index_t dimension >
    void do_define_solid_degeneration( pybind11::module& module )
    {
        using SolidMesh = SolidMesh< dimension >;
        using SolidMeshDegeneration = SolidMeshDegeneration< dimension >;
        const auto name =
            absl::StrCat( "SolidMeshDegeneration", dimension, "D" );
        pybind11::class_< SolidMeshDegeneration >( module, name.c_str(),
            "Inspection of the degenerated edges and polyhedra of a "
            "SolidMesh. The edges of the mesh are enabled by the inspections "
            "returning edge indices, and disabled again when the inspector "
            "is destroyed (if they were not enabled): do not modify the mesh "
            "while an inspector exists, and do not inspect or use the same "
            "mesh or model from two threads at once" )
            .def( pybind11::init< const SolidMesh& >(), pybind11::arg( "mesh" ),
                pybind11::keep_alive< 1, 2 >(),
                "Create the inspector of the degenerated elements of this "
                "mesh. The inspector keeps the mesh alive" )
            .def( "is_mesh_degenerated",
                &SolidMeshDegeneration::is_mesh_degenerated, ReleaseGil(),
                "Return True if a polyhedron edge is shorter than "
                "GLOBAL_EPSILON (1e-6), or a polyhedron is degenerated" )
            .def( "small_edges", &SolidMeshDegeneration::small_edges,
                pybind11::arg( "threshold" ), ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "edges of the mesh shorter than threshold (a positive length). "
                "The edges of the mesh are enabled if they are not" )
            .def( "degenerated_edges",
                &SolidMeshDegeneration::degenerated_edges, ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "edges shorter than GLOBAL_EPSILON (1e-6), as small_edges" )
            .def( "small_height_polyhedra",
                &SolidMeshDegeneration::small_height_polyhedra,
                pybind11::arg( "threshold" ), ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "polyhedra whose minimum height is at most threshold" )
            .def( "degenerated_polyhedra",
                &SolidMeshDegeneration::degenerated_polyhedra, ReleaseGil(),
                "Return the InspectionIssuesIndex listing the indices of the "
                "polyhedra whose minimum height is at most GLOBAL_EPSILON "
                "(1e-6)" );
    }

    void define_solid_degeneration( pybind11::module& module )
    {
        do_define_solid_degeneration< 3 >( module );
    }
} // namespace geode
