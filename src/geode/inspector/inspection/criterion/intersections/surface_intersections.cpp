/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/inspector/inspection/criterion/intersections/surface_intersections.hpp>

#include <absl/algorithm/container.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/information.hpp>

#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>
#include <geode/mesh/helpers/detail/mesh_intersection_detection.hpp>

namespace
{
    template < geode::index_t dimension >
    class PolygonPolygonIntersection
    {
    public:
        explicit PolygonPolygonIntersection(
            const geode::SurfaceMesh< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        [[nodiscard]] bool operator()(
            geode::index_t p1_id, geode::index_t p2_id ) const
        {
            if( p1_id == p2_id )
            {
                return false;
            }
            const auto p1_vertices = mesh_.polygon_vertices( p1_id );
            const auto p2_vertices = mesh_.polygon_vertices( p2_id );
            return geode::detail::polygons_intersection_detection<
                geode::SurfaceMesh< dimension > >(
                mesh_, p1_vertices, p2_vertices );
        }

    private:
        const geode::SurfaceMesh< dimension >& mesh_;
    };
} // namespace

namespace geode
{
    template < index_t dimension >
    class SurfaceMeshIntersections< dimension >::Impl
    {
    public:
        Impl( const SurfaceMesh< dimension >& mesh )
            : mesh_( mesh ), verbose_( false )
        {
        }

        bool mesh_has_self_intersections() const
        {
            const auto intersections = intersecting_polygons();
            if( intersections.empty() )
            {
                return false;
            }
            return true;
        }

        InspectionIssues< std::pair< index_t, index_t > >
            intersecting_elements() const
        {
            const auto intersections = intersecting_polygons();
            InspectionIssues< std::pair< index_t, index_t > > issues{
                "intersections between polygons"
            };

            for( const auto& polygon_pair : intersections )
            {
                issues.add_issue( polygon_pair,
                    absl::StrCat( "Polygons ", polygon_pair.first, " and ",
                        polygon_pair.second, " intersect" ) );
            }
            return issues;
        }

    private:
        std::vector< std::pair< index_t, index_t > >
            intersecting_polygons() const
        {
            const auto surface_aabb = create_aabb_tree( mesh_ );
            const PolygonPolygonIntersection< dimension > action{ mesh_ };
            return surface_aabb.compute_self_element_bbox_intersections(
                action );
        }

    private:
        const SurfaceMesh< dimension >& mesh_;
        bool verbose_;
    };

    template < index_t dimension >
    SurfaceMeshIntersections< dimension >::SurfaceMeshIntersections(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SurfaceMeshIntersections< dimension >::~SurfaceMeshIntersections() =
        default;

    template < index_t dimension >
    bool SurfaceMeshIntersections< dimension >::mesh_has_self_intersections()
        const
    {
        return impl_->mesh_has_self_intersections();
    }

    template < index_t dimension >
    InspectionIssues< std::pair< index_t, index_t > >
        SurfaceMeshIntersections< dimension >::intersecting_elements() const
    {
        return impl_->intersecting_elements();
    }

    template class opengeode_inspector_inspection_api
        SurfaceMeshIntersections< 2 >;
    template class opengeode_inspector_inspection_api
        SurfaceMeshIntersections< 3 >;
} // namespace geode
