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

#include <geode/inspector/common.hpp>
#include <geode/inspector/criterion/brep_meshes_inspector.hpp>
#include <geode/inspector/mixin/add_inspectors.hpp>
#include <geode/inspector/topology/brep_topology.hpp>

namespace geode
{
    class BRep;
}

namespace geode
{
    struct opengeode_inspector_inspector_api BRepInspectionResult
    {
        BRepMeshesInspectionResult meshes;
        BRepTopologyInspectionResult topology;

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };
    /*!
     * Class for inspecting a BRep model
     * @extends BRepTopologyInspector
     */
    class opengeode_inspector_inspector_api BRepInspector
        : public AddInspectors< BRep,
              BRepMeshesInspector,
              BRepTopologyInspector >
    {
        OPENGEODE_DISABLE_COPY( BRepInspector );

    public:
        explicit BRepInspector( const BRep& brep );

        [[nodiscard]] BRepInspectionResult inspect_brep() const;
    };
} // namespace geode