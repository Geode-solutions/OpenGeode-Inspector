/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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
#include <geode/basic/uuid.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>

namespace geode
{
    class Section;
    class BRep;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api DegeneratedElements
    {
        InspectionIssues< index_t > degenerated_edges;
        InspectionIssues< index_t > degenerated_polygons;
        InspectionIssues< index_t > degenerated_polyhedra;
    };

    struct opengeode_inspector_inspector_api DegeneratedElementsInspectionResult
    {
        absl::flat_hash_map< uuid, DegeneratedElements > elements;
        std::string string() const;
    };

    /*!
     * Class for inspecting the degeneration of elements in the Component Meshes
     * of a Model (BRep or Section).
     */
    template < index_t dimension, typename Model >
    class ComponentMeshesDegeneration
    {
        OPENGEODE_DISABLE_COPY( ComponentMeshesDegeneration );

    public:
        ComponentMeshesDegeneration( const Model& model );

        ~ComponentMeshesDegeneration();

        DegeneratedElementsInspectionResult inspect_elements() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    using SectionComponentMeshesDegeneration =
        ComponentMeshesDegeneration< 2, Section >;
    using BRepComponentMeshesDegeneration =
        ComponentMeshesDegeneration< 3, BRep >;
} // namespace geode