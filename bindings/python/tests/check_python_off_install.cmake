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

# The helpers of the Python bindings are installed by every configure, also
# when the Python bindings are OFF: the downstream projects compile their
# bindings against archives built by C++-only jobs. Configure the project with
# the Python bindings OFF and check its install script installs the helper in
# the public component.
# Arguments: SOURCE_DIR, BINARY_DIR, PREFIX_PATH (separated by "|"), GENERATOR,
# CXX_COMPILER

string(REPLACE "|" ";" prefix_path "${PREFIX_PATH}")
file(REMOVE_RECURSE "${BINARY_DIR}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" -S "${SOURCE_DIR}" -B "${BINARY_DIR}"
        -G "${GENERATOR}"
        "-DCMAKE_PREFIX_PATH=${prefix_path}"
        "-DCMAKE_CXX_COMPILER=${CXX_COMPILER}"
        -DCMAKE_BUILD_TYPE=Release
        -DOPENGEODE_INSPECTOR_WITH_PYTHON=OFF
        -DOPENGEODE_INSPECTOR_WITH_TESTS=OFF
        -DOPENGEODE_INSPECTOR_WITH_EXECUTABLES=OFF
        "-DCMAKE_INSTALL_PREFIX=${BINARY_DIR}/install"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Configure without Python failed:\n${output}")
endif()
file(READ "${BINARY_DIR}/cmake_install.cmake" install_script)
# The install rule of the helper: the block of the install script guarded by
# the public component. It is run alone (the other rules install the built
# libraries) in a temporary prefix, and must install the header at its
# public place.
string(REGEX MATCHALL "if\\(CMAKE_INSTALL_COMPONENT[^\n]*\n(  [^\n]*\n)+endif\\(\\)"
    blocks "${install_script}")
set(helper_block)
foreach(block IN LISTS blocks)
    if(block MATCHES "inspection_helpers\\.hpp")
        set(helper_block "${block}")
    endif()
endforeach()
if(NOT helper_block)
    message(FATAL_ERROR
        "inspection_helpers.hpp is not installed when the Python bindings are OFF")
endif()
if(NOT helper_block MATCHES "CMAKE_INSTALL_COMPONENT STREQUAL \"public\"")
    message(FATAL_ERROR "The helper is not in the public component:\n${helper_block}")
endif()
set(prefix "${BINARY_DIR}/helper_prefix")
file(REMOVE_RECURSE "${prefix}")
file(WRITE "${BINARY_DIR}/install_helper.cmake" "${helper_block}\n")
execute_process(
    COMMAND "${CMAKE_COMMAND}" "-DCMAKE_INSTALL_PREFIX=${prefix}"
        -DCMAKE_INSTALL_COMPONENT=public -P "${BINARY_DIR}/install_helper.cmake"
    RESULT_VARIABLE result
    OUTPUT_VARIABLE output
    ERROR_VARIABLE output
)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "The install of the helper failed:\n${output}")
endif()
if(NOT EXISTS "${prefix}/include/geode/inspector/bindings/python/inspection_helpers.hpp")
    message(FATAL_ERROR "The helper is not installed in include/geode/inspector/bindings/python:\n${output}")
endif()
# Another component does not install it
file(REMOVE_RECURSE "${prefix}")
execute_process(
    COMMAND "${CMAKE_COMMAND}" "-DCMAKE_INSTALL_PREFIX=${prefix}"
        -DCMAKE_INSTALL_COMPONENT=other -P "${BINARY_DIR}/install_helper.cmake"
    RESULT_VARIABLE result
)
if(EXISTS "${prefix}/include")
    message(FATAL_ERROR "The helper is installed by any component")
endif()
string(FIND "${install_script}" "bindings/python/src" position)
if(NOT position EQUAL -1)
    message(FATAL_ERROR "The Python bindings are configured")
endif()
# Only the include/ file is needed: it must exist in the sources
if(NOT EXISTS "${SOURCE_DIR}/include/geode/inspector/bindings/python/inspection_helpers.hpp")
    message(FATAL_ERROR "The helper is missing from the sources")
endif()
