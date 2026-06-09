# -*- coding: utf-8 -*-
# Copyright (c) 2019 - 2026 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

import os
import sys
import platform

if sys.version_info >= (3, 8, 0) and platform.system() == "Windows":
    for path in [x.strip() for x in os.environ["PATH"].split("") if x]:
        os.add_dll_directory(path)

import opengeode as geode
import opengeode_inspector_py_validity as validity

def check_validity():
    curve = geode.EdgedCurve3D.create()
    builder = geode.EdgedCurveBuilder3D.create( curve )
    builder.create_vertices( 7 )
    builder.set_point( 0, geode.Point3D([ 0., 2., 1. ]) )
    builder.set_point( 1, geode.Point3D([ 0., 2., 1. ]) )
    builder.set_point( 2, geode.Point3D([ 0., 0., 0. ]) )
    builder.set_point( 3, geode.Point3D([ 2., 0., 0. ]) )
    builder.set_point( 4, geode.Point3D([ 1., 4., 3. ]) )
    builder.set_point( 5, geode.Point3D([ 2., geode.GLOBAL_EPSILON / 2, geode.GLOBAL_EPSILON / 2 ]) )
    builder.set_point( 6, geode.Point3D([ geode.GLOBAL_EPSILON / 1.1, 2., 1. ]) )

    first_object_validity = validity.is_edged_curve_valid3D( curve )
    print( "ObjectValidity: \n" + first_object_validity.string() )
    if not first_object_validity.nb_issues() == 1:
        raise ValueError( "EdgedCurve should have 1 invalidity due to colocation, not "+ str(first_object_validity.nb_issues()) )

    builder.create_edge_with_vertices( 0, 1 )
    builder.create_edge_with_vertices( 1, 2 )
    builder.create_edge_with_vertices( 2, 3 )
    builder.create_edge_with_vertices( 3, 4 )
    builder.create_edge_with_vertices( 4, 5 )
    builder.create_edge_with_vertices( 5, 6 )

    object_validity = validity.is_edged_curve_valid3D( curve )
    print( "ObjectValidity: \n" + object_validity.string() );
    if not object_validity.nb_issues() == 2:
        raise ValueError( "EdgedCurve should have 2 invalidity reasons, not "+ str(object_validity.nb_issues()) )


if __name__ == "__main__":
    validity.OpenGeodeInspectorValidityLibrary.initialize()
    check_validity()
