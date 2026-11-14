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

#include <pybind11/iostream.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <geode/bindings/python/common.hpp>

#include "../inspection_helpers.hpp"

#include "criterion/adjacency/brep_meshes_adjacency.hpp"
#include "criterion/adjacency/section_meshes_adjacency.hpp"
#include "criterion/adjacency/solid_adjacency.hpp"
#include "criterion/adjacency/surface_adjacency.hpp"

#include "criterion/colocation/component_meshes_colocation.hpp"
#include "criterion/colocation/edgedcurve_colocation.hpp"
#include "criterion/colocation/pointset_colocation.hpp"
#include "criterion/colocation/solid_colocation.hpp"
#include "criterion/colocation/surface_colocation.hpp"
#include "criterion/colocation/unique_vertices_colocation.hpp"

#include "criterion/degeneration/brep_meshes_degeneration.hpp"
#include "criterion/degeneration/edgedcurve_degeneration.hpp"
#include "criterion/degeneration/section_meshes_degeneration.hpp"
#include "criterion/degeneration/solid_degeneration.hpp"
#include "criterion/degeneration/surface_degeneration.hpp"

#include "criterion/intersections/model_intersections.hpp"
#include "criterion/intersections/surface_curve_intersections.hpp"
#include "criterion/intersections/surface_intersections.hpp"

#include "criterion/isolation/edgedcurve_isolation.hpp"
#include "criterion/isolation/solid_isolation.hpp"
#include "criterion/isolation/surface_isolation.hpp"

#include "criterion/manifold/brep_meshes_manifold.hpp"
#include "criterion/manifold/section_meshes_manifold.hpp"
#include "criterion/manifold/solid_edge_manifold.hpp"
#include "criterion/manifold/solid_facet_manifold.hpp"
#include "criterion/manifold/solid_vertex_manifold.hpp"
#include "criterion/manifold/surface_edge_manifold.hpp"
#include "criterion/manifold/surface_vertex_manifold.hpp"

#include "criterion/negative_elements/brep_meshes_negative_elements.hpp"
#include "criterion/negative_elements/section_meshes_negative_elements.hpp"
#include "criterion/negative_elements/solid_negative_elements.hpp"
#include "criterion/negative_elements/surface_negative_elements.hpp"

#include "criterion/brep_meshes_inspector.hpp"
#include "criterion/section_meshes_inspector.hpp"

#include "topology/brep_topology.hpp"
#include "topology/section_topology.hpp"

#include "brep_inspector.hpp"
#include "edgedcurve_inspector.hpp"
#include "information.hpp"
#include "pointset_inspector.hpp"
#include "section_inspector.hpp"
#include "solid_inspector.hpp"
#include "surface_inspector.hpp"

PYBIND11_MODULE( opengeode_inspector_py_inspection, module )
{
    module.doc() =
        "Inspection of the validity of the meshes and models of OpenGeode: "
        "the inspectors check one criterion (adjacency, colocation, "
        "degeneration, intersections, isolation, manifold property, "
        "negative elements, topology) and return the issues found; the "
        "inspectors of a mesh type or of a model type (SurfaceMeshInspector2D, "
        "BRepInspector...) check every criterion, and the functions "
        "inspect_surface2D, inspect_brep... do it in one call. Call "
        "OpenGeodeInspectorInspectionLibrary.initialize() before using the "
        "module.";
    geode::import_binding_dependency( module, "opengeode" );
    pybind11::class_< geode::OpenGeodeInspectorInspectionLibrary >( module,
        "OpenGeodeInspectorInspectionLibrary",
        "Initialization of the inspection module" )
        .def_static( "initialize",
            &geode::OpenGeodeInspectorInspectionLibrary::initialize,
            "Initialize the inspection library, and OpenGeodeModelLibrary "
            "with it. Calling it again has no effect" );
    geode::define_information( module );
    geode::define_surface_adjacency( module );
    geode::define_solid_adjacency( module );
    geode::define_section_meshes_adjacency( module );
    geode::define_brep_meshes_adjacency( module );
    geode::define_edged_curve_colocation( module );
    geode::define_point_set_colocation( module );
    geode::define_surface_colocation( module );
    geode::define_solid_colocation( module );
    geode::define_models_meshes_colocation( module );
    geode::define_models_uv_colocation( module );
    geode::define_edged_curve_degeneration( module );
    geode::define_solid_degeneration( module );
    geode::define_surface_degeneration( module );
    geode::define_brep_meshes_degeneration( module );
    geode::define_section_meshes_degeneration( module );
    geode::define_surface_intersections( module );
    geode::define_surface_curve_intersections( module );
    geode::define_model_intersections( module );
    geode::define_edged_curve_isolation( module );
    geode::define_surface_isolation( module );
    geode::define_solid_isolation( module );
    geode::define_surface_edge_manifold( module );
    geode::define_surface_vertex_manifold( module );
    geode::define_solid_edge_manifold( module );
    geode::define_solid_vertex_manifold( module );
    geode::define_solid_facet_manifold( module );
    geode::define_brep_meshes_manifold( module );
    geode::define_section_meshes_manifold( module );
    geode::define_surface_negative_elements( module );
    geode::define_solid_negative_elements( module );
    geode::define_brep_meshes_negative_elements( module );
    geode::define_section_meshes_negative_elements( module );
    geode::define_brep_topology_inspector( module );
    geode::define_section_topology_inspector( module );
    geode::define_brep_meshes_inspector( module );
    geode::define_section_meshes_inspector( module );
    geode::define_brep_inspector( module );
    geode::define_section_inspector( module );
    geode::define_point_set_inspector( module );
    geode::define_edged_curve_inspector( module );
    geode::define_surface_inspector( module );
    geode::define_solid_inspector( module );
    geode::document_missing_constructors( module );
}
