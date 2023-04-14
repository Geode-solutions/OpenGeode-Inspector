# Copyright (c) 2019 - 2023 Geode-solutions
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

add_geode_test(
    SOURCE "test-surface-adjacency.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-solid-adjacency.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-pointset.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-edgedcurve-colocation.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-surface-colocation.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-solid-colocation.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-edgedcurve-degeneration.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-surface-degeneration.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-solid-degeneration.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-surface-intersections.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-surface-curve-intersections.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-surface-manifold.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-solid-manifold.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-brep.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        OpenGeode::model
        OpenGeode-Geosciences::explicit
        OpenGeode-GeosciencesIO::model
        ${PROJECT_NAME}::inspector
)

add_geode_test(
    SOURCE "test-section.cpp"
    DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        OpenGeode::model
        ${PROJECT_NAME}::inspector
)