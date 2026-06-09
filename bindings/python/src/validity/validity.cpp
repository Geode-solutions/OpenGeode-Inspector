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

#include "pybind11/iostream.h"
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "brep_validity.hpp"
#include "edgedcurve_validity.hpp"
#include "object_validity.hpp"
#include "pointset_validity.hpp"
#include "section_validity.hpp"
#include "solid_validity.hpp"
#include "surface_validity.hpp"

PYBIND11_MODULE( opengeode_inspector_py_validity, module )
{
    module.doc() = "OpenGeode-Inspector Python binding";
    pybind11::class_< geode::OpenGeodeInspectorValidityLibrary >(
        module, "OpenGeodeInspectorValidityLibrary" )
        .def( "initialize",
            &geode::OpenGeodeInspectorValidityLibrary::initialize );
    geode::define_object_validity( module );
    geode::define_brep_validity( module );
    geode::define_edged_curve_validity( module );
    geode::define_point_set_validity( module );
    geode::define_section_validity( module );
    geode::define_solid_mesh_validity( module );
    geode::define_surface_mesh_validity( module );
}