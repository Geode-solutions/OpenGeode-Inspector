/*
 * Copyright (c) 2019 - 2025 Geode-solutions
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#pragma once

#include <absl/container/flat_hash_map.h>
#include <geode/basic/pimpl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

namespace geode
{
    class Section;
    class BRep;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api MeshesColocationInspectionResult
    {
        InspectionIssuesMap< std::vector< index_t > > colocated_points_groups{
            "Model component meshes with colocated points"
        };

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };

    /*!
     * Class for inspecting the colocation of points in the Component Meshes of
     * a Model (BRep or Section).
     */
    template < typename Model >
    class ComponentMeshesColocation
    {
        OPENGEODE_DISABLE_COPY( ComponentMeshesColocation );

    public:
        explicit ComponentMeshesColocation( const Model& model );

        ~ComponentMeshesColocation();

        [[nodiscard]] MeshesColocationInspectionResult
            inspect_meshes_point_colocations() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    using SectionComponentMeshesColocation =
        ComponentMeshesColocation< Section >;
    using BRepComponentMeshesColocation = ComponentMeshesColocation< BRep >;
} // namespace geode