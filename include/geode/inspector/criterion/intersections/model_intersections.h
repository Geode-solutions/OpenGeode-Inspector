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

#include <geode/basic/pimpl.h>

#include <geode/inspector/common.h>
#include <geode/inspector/information.h>

#include <geode/model/mixin/core/component_mesh_element.h>

namespace geode
{
    class Section;
    class BRep;
} // namespace geode

namespace geode
{
    struct opengeode_inspector_inspector_api
        ElementsIntersectionsInspectionResult
    {
        InspectionIssues<
            std::pair< ComponentMeshElement, ComponentMeshElement > >
            elements_intersections;
        std::string string() const;
    };

    /*!
     * Class for inspecting the intersections of a Model meshes
     */
    template < index_t dimension, typename Model >
    class ModelMeshesIntersections
    {
        OPENGEODE_DISABLE_COPY( ModelMeshesIntersections );

    public:
        ModelMeshesIntersections( const Model& model );

        ~ModelMeshesIntersections();

        bool model_has_intersecting_surfaces() const;

        ElementsIntersectionsInspectionResult inspect_intersections() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    using SectionMeshesIntersections = ModelMeshesIntersections< 2, Section >;
    using BRepMeshesIntersections = ModelMeshesIntersections< 3, BRep >;
} // namespace geode