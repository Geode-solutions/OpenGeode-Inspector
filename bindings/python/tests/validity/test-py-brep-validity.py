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

def data_dir():
    test_dir = os.path.dirname(__file__)
    return os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))


def check_a1():
    model_brep = geode.load_brep(data_dir() + "/model_A1.og_brep")
    result = validity.is_brep_valid(model_brep)
    if result.nb_issues()!=7:
        raise ValueError( "[Test] model model_A1 should have 7 issues." )


def check_a1_valid():
    model_brep = geode.load_brep(data_dir() + "/model_A1_valid.og_brep")
    result = validity.is_brep_valid(model_brep)
    if result.nb_issues()!=7:
        raise ValueError( "[Test] model model_A1_valid should have 7 issues." )


def check_model_mss():
    model_brep = geode.load_brep(data_dir() + "/mss.og_brep")
    result = validity.is_brep_valid(model_brep)
    if result.nb_issues()!=5:
        raise ValueError( "[Test] model_mss should have 5 issues." )


def check_model_D():
    model_brep = geode.load_brep(data_dir() + "/model_D.og_brep")
    result = validity.is_brep_valid(model_brep)
    if result.nb_issues()!=0:
        raise ValueError( "[Test] model_D should have 0 issues." )


if __name__ == "__main__":
    validity.OpenGeodeInspectorValidityLibrary.initialize()
    check_a1()
    check_a1_valid()
    check_model_mss()
    check_model_D()
