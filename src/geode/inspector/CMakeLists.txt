# Copyright (c) 2019 - 2021 Geode-solutions
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

add_geode_library(
    NAME inspector
    FOLDER "geode/inspector"
    SOURCES
        "common.cpp"
        "criterion/degeneration/surface_degeneration.cpp"
        "criterion/degeneration/solid_degeneration.cpp"
    PUBLIC_HEADERS
        "common.h"
        "criterion/degeneration/surface_degeneration.h"
        "criterion/degeneration/solid_degeneration.h"
    ADVANCED_HEADERS
        # "mixin/core/detail/components_storage.h"
    PRIVATE_HEADERS
        # "helpers/private/simplicial_model_creator.h"
    PUBLIC_DEPENDENCIES
        OpenGeode::basic
        OpenGeode::geometry
        OpenGeode::mesh
        # absl::flat_hash_map
        # Bitsery::bitsery
        # ghcFilesystem::ghc_filesystem
        # ${PROJECT_NAME}::geometry
        # ${PROJECT_NAME}::mesh
    PRIVATE_DEPENDENCIES
        # Async++
)

