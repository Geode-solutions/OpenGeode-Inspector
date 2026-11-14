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

"""The inspections release the GIL: another Python thread keeps running while
an inspection of a large model is in progress."""

import os
import sys
import threading

import geode_test_env

import opengeode
import opengeode_inspector_py_inspection as inspection
import opengeode_inspector_py_validity as validity


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def data_dir():
    return os.path.abspath(
        os.path.join(os.path.dirname(__file__), "../../../tests/data")
    )


# The checks are deterministic thanks to a huge interpreter switch interval:
# the interpreter never forces a GIL hand-over, so a thread keeps the GIL until
# it blocks or exits. If a bound call holds the GIL, the worker thread runs the
# whole work (and `finished.set()`) before the main thread can run. If the call
# releases the GIL, the main thread is scheduled as soon as the call is inside
# C++ and sees the work still running. Holding the GIL can therefore never
# pass; the only possible flakiness is the work finishing before the main
# thread wakes up, hence the work repeats the call and ATTEMPTS retries are
# allowed.
TIMEOUT = 60
ATTEMPTS = 3
REPETITIONS = 20


class HugeSwitchInterval:
    def __enter__(self):
        self.previous = sys.getswitchinterval()
        sys.setswitchinterval(1000)

    def __exit__(self, *arguments):
        sys.setswitchinterval(self.previous)


def run_in_worker(work):
    """Run work in a thread, return True if the main thread could run before
    the work finished"""
    ready = threading.Event()
    finished = threading.Event()
    box = {}

    def target():
        ready.set()
        try:
            work()
        except BaseException as error:
            box["error"] = error
        finished.set()

    thread = threading.Thread(target=target)
    thread.start()
    check(ready.wait(TIMEOUT), "the worker thread did not start")
    main_ran_during_work = not finished.is_set()
    thread.join(TIMEOUT)
    if "error" in box:
        raise box["error"]
    return main_ran_during_work


def check_releases_gil(name, call):
    def work():
        for _ in range(REPETITIONS):
            call()

    with HugeSwitchInterval():
        for _ in range(ATTEMPTS):
            if run_in_worker(work):
                return
    raise ValueError(f"[Test] {name} kept the GIL during the whole call")


def test_model_inspections():
    brep = opengeode.load(
        opengeode.BRep, os.path.join(data_dir(), "model_A1.og_brep")
    )
    check_releases_gil("inspect_brep", lambda: inspection.inspect_brep(brep))
    check_releases_gil("is_brep_valid", lambda: validity.is_brep_valid(brep))
    check_releases_gil(
        "BRepInspector.inspect_brep",
        lambda: inspection.BRepInspector(brep).inspect_brep(),
    )
    check_releases_gil(
        "BRepMeshesIntersections",
        lambda: inspection.BRepMeshesIntersections(brep).inspect_intersections(),
    )


def test_mesh_inspections():
    brep = opengeode.load(
        opengeode.BRep, os.path.join(data_dir(), "model_D.og_brep")
    )
    surfaces = sorted(
        (surface.mesh() for surface in brep.surfaces()),
        key=lambda mesh: mesh.nb_polygons(),
    )
    biggest = surfaces[-1]
    check_releases_gil("inspect_surface3D", lambda: inspection.inspect_surface3D(biggest))
    check_releases_gil(
        "SurfaceMeshIntersections3D",
        lambda: inspection.SurfaceMeshIntersections3D(biggest).intersecting_elements(),
    )
    check_releases_gil(
        "SurfaceMeshColocation3D",
        lambda: inspection.SurfaceMeshColocation3D(biggest).colocated_points_groups(),
    )
    check_releases_gil(
        "is_surface_valid3D", lambda: validity.is_surface_valid3D(biggest)
    )


if __name__ == "__main__":
    test_model_inspections()
    test_mesh_inspections()
