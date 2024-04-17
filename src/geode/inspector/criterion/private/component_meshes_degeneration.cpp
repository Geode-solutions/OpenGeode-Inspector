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

#include <geode/inspector/criterion/private/component_meshes_degeneration.h>

#include <geode/basic/logger.h>

#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/criterion/degeneration/edgedcurve_degeneration.h>
#include <geode/inspector/criterion/degeneration/surface_degeneration.h>

namespace geode
{
    template < typename Model >
    ComponentMeshesDegeneration< Model >::ComponentMeshesDegeneration(
        const Model& model )
        : model_( model )
    {
    }

    template < typename Model >
    void ComponentMeshesDegeneration< Model >::add_degenerated_edges(
        InspectionIssuesMap< index_t >& components_degenerated_edges ) const
    {
        for( const auto& line : model_.lines() )
        {
            const EdgedCurveDegeneration< Model::dim > inspector{ line.mesh() };
            auto issues = inspector.degenerated_edges();
            issues.set_description( absl::StrCat(
                "Line ", line.id().string(), " degenerated edges" ) );
            components_degenerated_edges.add_issues_to_map(
                line.id(), std::move( issues ) );
        }
        for( const auto& surface : model_.surfaces() )
        {
            const geode::SurfaceMeshDegeneration< Model::dim > inspector{
                surface.mesh()
            };
            auto issues = inspector.degenerated_edges();
            issues.set_description( absl::StrCat(
                "Surface ", surface.id().string(), " degenerated edges" ) );
            components_degenerated_edges.add_issues_to_map(
                surface.id(), std::move( issues ) );
        }
    }

    template < typename Model >
    void ComponentMeshesDegeneration< Model >::add_degenerated_polygons(
        InspectionIssuesMap< index_t >& components_degenerated_polygons ) const
    {
        for( const auto& surface : model_.surfaces() )
        {
            const geode::SurfaceMeshDegeneration< Model::dim > inspector{
                surface.mesh()
            };
            auto issues = inspector.degenerated_polygons();
            issues.set_description( absl::StrCat(
                "Surface ", surface.id().string(), " degenerated polygons" ) );
            components_degenerated_polygons.add_issues_to_map(
                surface.id(), std::move( issues ) );
        }
    }

    template < typename Model >
    const Model& ComponentMeshesDegeneration< Model >::model() const
    {
        return model_;
    }

    template class opengeode_inspector_inspector_api
        ComponentMeshesDegeneration< Section >;
    template class opengeode_inspector_inspector_api
        ComponentMeshesDegeneration< BRep >;
} // namespace geode
