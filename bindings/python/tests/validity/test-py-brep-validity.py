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

import geode_test_env

import opengeode as geode
import opengeode_inspector_py_validity as validity

def data_dir():
    test_dir = os.path.dirname(__file__)
    return os.path.abspath(os.path.join(test_dir, "../../../../tests/data"))


def check_a1():
    model_brep = geode.load_brep(data_dir() + "/model_A1.og_brep")
    result = validity.is_brep_valid(model_brep)
    if result.nb_issues()!=8:
        raise ValueError( "[Test] model model_A1 should have 8 issues." )
    if str(result) != result.string():
        raise ValueError("[Test] str should return the validity report")
    if repr(result) != "<ObjectValidity: 8 issues>":
        raise ValueError("[Test] repr should summarize the validity report")


def check_a1_valid():
    model_brep = geode.load_brep(data_dir() + "/model_A1_valid.og_brep")
    result = validity.is_brep_valid(model_brep)
    if result.nb_issues()!=8:
        raise ValueError( "[Test] model model_A1_valid should have 8 issues." )


def check_model_mss():
    model_brep = geode.load_brep(data_dir() + "/mss.og_brep")
    result = validity.is_brep_valid(model_brep)
    if result.nb_issues()!=4:
        raise ValueError( "[Test] model_mss should have 4 issues." )


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
