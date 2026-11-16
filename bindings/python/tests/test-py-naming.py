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

"""Naming conventions of the bindings: the dimension suffix of a function is 2D
or 3D (never 2d or 3d) and appended to the name, the static methods are
static, and the library initialization is a static method. The bindings define
no enum."""

import enum
import inspect
import re

import geode_test_env

import opengeode_inspector_py_inspection as inspection
import opengeode_inspector_py_validity as validity

# Extension modules to check; the other modules are added with their commit
MODULES = [inspection, validity]
LOWERCASE_DIMENSION = re.compile(r"[a-z_][23]d(_|$)")
# OpenGeode appends the dimension: load_horizons_stack2D, not ..._2D
UNDERSCORE_DIMENSION = re.compile(r"_[23]D(_|$)")
CAMEL_CASE = re.compile(r"^[A-Z][A-Za-z0-9]*$")


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def test_no_enum():
    for module in MODULES:
        for name, member in vars(module).items():
            check(
                not (inspect.isclass(member) and issubclass(member, enum.Enum)),
                f"{module.__name__}.{name}: the bindings have no enum, "
                "a new one must be a CamelCase native_enum without export_values",
            )


def test_class_names():
    for module in MODULES:
        for name, member in vars(module).items():
            if inspect.isclass(member) and member.__module__ == module.__name__:
                check(CAMEL_CASE.match(name), f"class {name} should be CamelCase")


def test_dimension_suffix():
    for module in MODULES:
        for name, member in vars(module).items():
            if name.startswith("_"):
                continue
            check(
                not LOWERCASE_DIMENSION.search(name),
                f"{module.__name__}.{name}: use 2D/3D, not 2d/3d",
            )
            check(
                not UNDERSCORE_DIMENSION.search(name),
                f"{module.__name__}.{name}: append the dimension (names2D)",
            )
            if inspect.isclass(member) and member.__module__ == module.__name__:
                for member_name in vars(member):
                    check(
                        not LOWERCASE_DIMENSION.search(member_name),
                        f"{name}.{member_name}: use 2D/3D, not 2d/3d",
                    )


def test_static_methods():
    for module in MODULES:
        for name, member in vars(module).items():
            if inspect.isclass(member) and member.__module__ == module.__name__:
                for member_name, value in vars(member).items():
                    if member_name.endswith("_static"):
                        check(
                            isinstance(value, staticmethod),
                            f"{name}.{member_name} should be a static method",
                        )


def test_library_initialize_is_static():
    for module, name in [
        (inspection, "OpenGeodeInspectorInspectionLibrary"),
        (validity, "OpenGeodeInspectorValidityLibrary"),
    ]:
        library = getattr(module, name)
        check(
            isinstance(vars(library)["initialize"], staticmethod),
            f"{name}.initialize should be a static method",
        )


def test_dimension_suffix_of_functions():
    # Every function of one dimension has its counterpart: the dimension is
    # appended (inspect_surface2D), the 2D and 3D functions come together
    for module in MODULES:
        names = {
            name for name, member in vars(module).items() if callable(member)
        }
        for name in names:
            if name.endswith("2D"):
                check(
                    name[:-2] + "3D" in names,
                    f"{module.__name__}.{name} has no 3D counterpart",
                )
    check(
        hasattr(inspection, "inspect_solid3D")
        and not hasattr(inspection, "inspect_solid2D"),
        "inspect_solid3D is the only solid inspection function",
    )


if __name__ == "__main__":
    test_no_enum()
    test_class_names()
    test_dimension_suffix()
    test_static_methods()
    test_library_initialize_is_static()
    test_dimension_suffix_of_functions()
