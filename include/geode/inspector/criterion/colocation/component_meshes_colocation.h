/*
 * Copyright (c) 2019 - 2023 Geode-solutions
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

#include <geode/basic/pimpl.h>

#include <geode/inspector/common.h>

namespace geode
{
    class Section;
    class BRep;
    struct uuid;
} // namespace geode

namespace geode
{
    /*!
     * Class for inspecting the colocation of points in the Component Meshes of
     * a Model (BRep or Section).
     */
    template < index_t dimension, typename Model >
    class opengeode_inspector_inspector_api ComponentMeshesColocation
    {
        OPENGEODE_DISABLE_COPY( ComponentMeshesColocation );

    public:
        ComponentMeshesColocation( const Model& model );

        ComponentMeshesColocation( const Model& model, bool verbose );

        ~ComponentMeshesColocation();

        std::vector< uuid > components_with_colocated_points() const;

        absl::flat_hash_map< uuid, index_t >
            components_nb_colocated_points() const;

        absl::flat_hash_map< uuid, std::vector< std::vector< index_t > > >
            components_colocated_points_groups() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    using SectionComponentMeshesColocation =
        ComponentMeshesColocation< 2, Section >;
    using BRepComponentMeshesColocation = ComponentMeshesColocation< 3, BRep >;
} // namespace geode