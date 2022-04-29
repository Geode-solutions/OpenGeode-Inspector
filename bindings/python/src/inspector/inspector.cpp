/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include "adjacency/surface_adjacency.h"

#include "colocation/edgedcurve_colocation.h"
#include "colocation/pointset_colocation.h"
#include "colocation/solid_colocation.h"
#include "colocation/surface_colocation.h"

#include "degeneration/edgedcurve_degeneration.h"
#include "degeneration/solid_degeneration.h"
#include "degeneration/surface_degeneration.h"

#include "manifold/surface_edge_manifold.h"
#include "manifold/surface_vertex_manifold.h"

#include "topology/brep_topology.h"
#include "topology/section_topology.h"

PYBIND11_MODULE( opengeode_inspector_py_inspector, module )
{
    module.doc() = "OpenGeode-Inspector Python binding";
    geode::define_surface_adjacency( module );
    geode::define_edgedcurve_colocation( module );
    geode::define_pointset_colocation( module );
    geode::define_solid_colocation( module );
    geode::define_surface_colocation( module );
    geode::define_edgedcurve_degeneration( module );
    geode::define_solid_degeneration( module );
    geode::define_surface_degeneration( module );
    geode::define_surface_edge_manifold( module );
    geode::define_surface_vertex_manifold( module );
    geode::define_brep_topology_inspector( module );
    geode::define_section_topology_inspector( module );
}