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
    solid = geode.TetrahedralSolid3D.create()
    builder = geode.TetrahedralSolidBuilder3D.create( solid )
    builder.create_vertices( 7 )
    builder.set_point( 0, geode.Point3D([ 0., 0., 2. ]) )
    builder.set_point( 1, geode.Point3D([ 3., .5, 0. ]) )
    builder.set_point( 2, geode.Point3D([ .5, 3., .5 ]) )
    builder.set_point( 3, geode.Point3D([ 2., 1.5, 3. ]) )
    builder.set_point( 4, geode.Point3D([ 3.5, 2.5, -.5 ]) )
    builder.set_point( 5, geode.Point3D([ 3., .5, 0. ]) )
    builder.set_point( 6, geode.Point3D([ .5, 3., .5 ]) )

    object_validity = validity.is_solid_valid3D( solid )
    print( "ObjectValidity: \n" + object_validity.string() )
    if not object_validity.nb_issues() == 1:
        raise ValueError( "SolidMesh should have 1 invalidity due to colocation, not " +str(object_validity.nb_issues()) )

    builder.create_tetrahedron( [ 0, 1, 2, 3 ] )
    builder.create_tetrahedron( [ 5, 4, 6, 3 ] )

    object_validity = validity.is_solid_valid3D( solid )
    print( "ObjectValidity: \n" + object_validity.string() )
    if not object_validity.nb_issues() == 2:
        raise ValueError( "SolidMesh should have 2 object_validity due to colocation and non manifold vertices, not " + str(object_validity.nb_issues()) )

    builder.create_tetrahedron( [ 2, 1, 6, 3 ] )
    builder.create_tetrahedron( [ 1, 5, 3, 6 ] )

    object_validity = validity.is_solid_valid3D( solid )
    print( "ObjectValidity: \n" + object_validity.string() )
    if not object_validity.nb_issues() == 5:
        raise ValueError( "SolidMesh should have 5 object_validity due to colocation, degenerated edges and polyhedra, and non manifold vertices and edges, not "+str(object_validity.nb_issues()) )

    builder.create_tetrahedron( [ 1, 4, 3, 6 ] )

    object_validity = validity.is_solid_valid3D( solid )
    print( "ObjectValidity: \n" + object_validity.string() )
    if not object_validity.nb_issues() == 7:
        raise ValueError( "SolidMesh should have 7 object_validity due to colocation, degenerated edges and polyhedra, non manifold vertices, edges, and facets, and negative polyhedra, not "+str(object_validity.nb_issues()))

    builder.set_polyhedron_adjacent( geode.PolyhedronFacet( 0, 0 ), 1 )

    object_validity = validity.is_solid_valid3D( solid )
    print( "ObjectValidity: \n" + object_validity.string() )
    if not object_validity.nb_issues() == 8:
        raise ValueError( "SolidMesh should have 8 object_validity due to adjacencies, colocation, degenerated edges and polyhedra, non manifold vertices, edges, and facets, and negative polyhedra, not "+str(object_validity.nb_issues()) )


if __name__ == "__main__":
    validity.OpenGeodeInspectorValidityLibrary.initialize()
    check_validity()
