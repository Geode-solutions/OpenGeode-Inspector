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

"""The Python examples of the repository run against the current API: they
are executed here, so that they cannot become stale.

Arguments: the examples directory, the data directory."""

import contextlib
import importlib.util
import io
import os
import sys

import geode_test_env

# The example must not leave a __pycache__ in the source tree
sys.dont_write_bytecode = True

import opengeode
import opengeode_inspector as inspector


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def load_example(examples_directory, name):
    spec = importlib.util.spec_from_file_location(
        name, os.path.join(examples_directory, name + ".py")
    )
    module = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(module)
    return module


def run(function, *arguments):
    output = io.StringIO()
    with contextlib.redirect_stdout(output):
        result = function(*arguments)
    return result, output.getvalue()


def test_model_inspection(examples_directory, data_directory):
    example = load_example(examples_directory, "model_inspection")
    default = example.default_filename()
    check(
        os.path.samefile(default, os.path.join(data_directory, "model_A1_valid.og_brep")),
        f"the default model of the example: {default}",
    )
    for name, valid in [("model_A1_valid.og_brep", None), ("model_A1.og_brep", False)]:
        filename = os.path.join(data_directory, name)
        result, output = run(example.inspect_model, filename)
        brep = opengeode.load(opengeode.BRep, filename)
        expected = inspector.inspect_brep(brep)
        check(
            isinstance(result, inspector.BRepInspectionResult),
            f"the example returns a BRepInspectionResult for {name}",
        )
        check(
            result.nb_issues() == expected.nb_issues(),
            f"the example inspects the whole model {name}",
        )
        topology_valid = inspector.BRepTopologyInspector(
            brep
        ).brep_topology_is_valid()
        check(
            ("Model topology is valid." in output) == topology_valid
            and ("Model topology is invalid." in output) != topology_valid,
            f"the example tells the topology validity of {name}: {output}",
        )
        if valid is not None:
            check(topology_valid == valid, f"validity of {name}")
        check(
            "unique vertices not linked to any component" in output
            and "corner vertices not linked to a unique vertex" in output
            and "negative elements in the component meshes" in output,
            f"the example prints the inspection of {name}: {output}",
        )
        check(repr(expected) in output, "the example prints the summary of the result")
        check(
            repr(inspector.BRepTopologyInspector(brep).inspect_brep_topology())
            in output,
            "the example prints the summary of the topology",
        )


if __name__ == "__main__":
    test_model_inspection(sys.argv[1], sys.argv[2])
