/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include "pybind11/iostream.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "criterion/adjacency/brep_meshes_adjacency.h"
#include "criterion/adjacency/section_meshes_adjacency.h"
#include "criterion/adjacency/solid_adjacency.h"
#include "criterion/adjacency/surface_adjacency.h"

#include "criterion/colocation/component_meshes_colocation.h"
#include "criterion/colocation/edgedcurve_colocation.h"
#include "criterion/colocation/pointset_colocation.h"
#include "criterion/colocation/solid_colocation.h"
#include "criterion/colocation/surface_colocation.h"
#include "criterion/colocation/unique_vertices_colocation.h"

#include "criterion/degeneration/brep_meshes_degeneration.h"
#include "criterion/degeneration/edgedcurve_degeneration.h"
#include "criterion/degeneration/section_meshes_degeneration.h"
#include "criterion/degeneration/solid_degeneration.h"
#include "criterion/degeneration/surface_degeneration.h"

#include "criterion/intersections/model_intersections.h"
#include "criterion/intersections/surface_curve_intersections.h"
#include "criterion/intersections/surface_intersections.h"

#include "criterion/manifold/brep_meshes_manifold.h"
#include "criterion/manifold/section_meshes_manifold.h"
#include "criterion/manifold/solid_edge_manifold.h"
#include "criterion/manifold/solid_facet_manifold.h"
#include "criterion/manifold/solid_vertex_manifold.h"
#include "criterion/manifold/surface_edge_manifold.h"
#include "criterion/manifold/surface_vertex_manifold.h"

#include "criterion/brep_meshes_inspector.h"
#include "criterion/section_meshes_inspector.h"

#include "topology/brep_topology.h"
#include "topology/section_topology.h"

#include "brep_inspector.h"
#include "edgedcurve_inspector.h"
#include "information.h"
#include "pointset_inspector.h"
#include "section_inspector.h"
#include "solid_inspector.h"
#include "surface_inspector.h"

namespace pybind11
{
    namespace detail
    {
        template < typename Key, typename Value >
        struct type_caster< absl::flat_hash_map< Key, Value > >
            : map_caster< absl::flat_hash_map< Key, Value >, Key, Value >
        {
        };

    } // namespace detail
} // namespace pybind11

PYBIND11_MODULE( opengeode_inspector_py_inspector, module )
{
    module.doc() = "OpenGeode-Inspector Python binding";
    pybind11::class_< geode::InspectorInspectorLibrary >(
        module, "InspectorInspectorLibrary" )
        .def( "initialize", &geode::InspectorInspectorLibrary::initialize );
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
    geode::define_surface_edge_manifold( module );
    geode::define_surface_vertex_manifold( module );
    geode::define_solid_edge_manifold( module );
    geode::define_solid_vertex_manifold( module );
    geode::define_solid_facet_manifold( module );
    geode::define_brep_meshes_manifold( module );
    geode::define_section_meshes_manifold( module );
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
}