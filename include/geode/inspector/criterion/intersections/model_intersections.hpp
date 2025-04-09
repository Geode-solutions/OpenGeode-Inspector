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

#include <geode/basic/pimpl.hpp>

#include <geode/inspector/common.hpp>
#include <geode/inspector/information.hpp>

#include <geode/model/mixin/core/component_mesh_element.hpp>

namespace geode
{
    class Section;
    class BRep;
} // namespace geode

namespace geode
{
    struct
        opengeode_inspector_inspector_api ElementsIntersectionsInspectionResult
    {
        InspectionIssues<
            std::pair< ComponentMeshElement, ComponentMeshElement > >
            elements_intersections{ "Surface intersections." };

        [[nodiscard]] index_t nb_issues() const;

        [[nodiscard]] std::string string() const;

        [[nodiscard]] std::string inspection_type() const;
    };

    /*!
     * Class for inspecting the intersections of a Model meshes
     */
    template < typename Model >
    class ModelMeshesIntersections
    {
        OPENGEODE_DISABLE_COPY( ModelMeshesIntersections );

    public:
        explicit ModelMeshesIntersections( const Model& model );

        ~ModelMeshesIntersections();

        [[nodiscard]] bool model_has_intersecting_surfaces() const;

        [[nodiscard]] ElementsIntersectionsInspectionResult
            inspect_intersections() const;

        [[nodiscard]] ElementsIntersectionsInspectionResult
            inspect_surfaces_self_intersections() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };

    using SectionMeshesIntersections = ModelMeshesIntersections< Section >;
    using BRepMeshesIntersections = ModelMeshesIntersections< BRep >;
} // namespace geode