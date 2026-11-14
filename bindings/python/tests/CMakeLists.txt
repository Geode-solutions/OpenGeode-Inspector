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

add_subdirectory(inspection)
add_subdirectory(validity)

set(py_inspection_dependencies
    ${PROJECT_NAME}::py_inspection
    ${PROJECT_NAME}::py_validity
)

foreach(test_name
    "test-py-naming"
    "test-py-signatures"
    "test-py-module-import"
    "test-py-lifetime"
    "test-py-negative-elements"
    "test-py-results"
    "test-py-gil"
    "test-py-model-inspectors"
    "test-py-topology-checks"
)
    add_geode_python_test(
        SOURCE "${test_name}.py"
        DEPENDENCIES ${py_inspection_dependencies}
    )
endforeach()

add_geode_python_test(
    SOURCE "test-py-package-docs.py"
    ARGUMENTS
        "${PROJECT_SOURCE_DIR}/bindings/python/opengeode_inspector_doc.txt"
        "${PROJECT_BINARY_DIR}/wheel/opengeode_inspector"
    DEPENDENCIES ${py_inspection_dependencies}
)

# The example runs in the tests, it cannot become stale
add_geode_python_test(
    SOURCE "test-py-examples.py"
    ARGUMENTS
        "${PROJECT_SOURCE_DIR}/examples"
        "${PROJECT_SOURCE_DIR}/tests/data"
    DEPENDENCIES ${py_inspection_dependencies}
)

# Compile flags of a downstream binding (pybind11, OpenGeode mesh and model)
# for the test compiling the installed helpers: the transitive include
# directories and definitions, without the sources of the bindings
find_package(pybind11 REQUIRED)
add_library(geode_installed_helpers_flags INTERFACE)
target_link_libraries(geode_installed_helpers_flags
    INTERFACE pybind11::module OpenGeode::mesh OpenGeode::model
)
set(helpers_flags "${PROJECT_BINARY_DIR}/installed_helpers_flags.cmake")
file(GENERATE OUTPUT "${helpers_flags}" CONTENT
"set(CHECK_INCLUDES \"$<TARGET_PROPERTY:geode_installed_helpers_flags,INTERFACE_INCLUDE_DIRECTORIES>\")
set(CHECK_DEFINITIONS \"$<TARGET_PROPERTY:geode_installed_helpers_flags,INTERFACE_COMPILE_DEFINITIONS>\")
set(CMAKE_CXX_STANDARD ${CMAKE_CXX_STANDARD})
set(CMAKE_CXX_COMPILER \"${CMAKE_CXX_COMPILER}\")
set(CMAKE_BUILD_TYPE Release)
")
add_geode_python_test(
    SOURCE "test-py-installed-helpers.py"
    ARGUMENTS
        "${CMAKE_COMMAND}"
        "${PROJECT_BINARY_DIR}/bindings/python/src/cmake_install.cmake"
        "${PROJECT_SOURCE_DIR}/include/geode/inspector/bindings/python"
        "${helpers_flags}"
        "${CMAKE_GENERATOR}"
)
