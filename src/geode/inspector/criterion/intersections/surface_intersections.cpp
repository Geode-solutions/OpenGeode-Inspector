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

#include <geode/inspector/criterion/intersections/surface_intersections.hpp>

#include <absl/algorithm/container.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/geometry/aabb.hpp>
#include <geode/geometry/basic_objects/segment.hpp>
#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/intersection_detection.hpp>
#include <geode/geometry/position.hpp>

#include <geode/mesh/core/triangulated_surface.hpp>
#include <geode/mesh/helpers/aabb_surface_helpers.hpp>
#include <geode/mesh/helpers/detail/mesh_intersection_detection.hpp>

namespace
{
    template < geode::index_t dimension >
    class TriangleTriangleIntersection
    {
    public:
        TriangleTriangleIntersection(
            const geode::TriangulatedSurface< dimension >& mesh,
            bool stop_at_first_intersection )
            : mesh_( mesh ),
              stop_at_first_intersection_{ stop_at_first_intersection }
        {
        }

        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_triangles()
        {
            return std::move( intersecting_triangles_ );
        }

        bool operator()( geode::index_t t1_id, geode::index_t t2_id )
        {
            if( t1_id == t2_id )
            {
                return false;
            }
            const auto t1_vertices = this->mesh().polygon_vertices( t1_id );
            const auto t2_vertices = this->mesh().polygon_vertices( t2_id );
            if( geode::detail::polygons_intersection_detection<
                    geode::SurfaceMesh< dimension > >(
                    mesh_, t1_vertices, t2_vertices ) )
            {
                this->emplace( t1_id, t2_id );
                return stop_at_first_intersection_;
            }
            return false;
        }

    protected:
        void emplace( geode::index_t t1_id, geode::index_t t2_id )
        {
            intersecting_triangles_.emplace_back( t1_id, t2_id );
        }

        const geode::TriangulatedSurface< dimension >& mesh() const
        {
            return mesh_;
        }

    private:
        const geode::TriangulatedSurface< dimension >& mesh_;
        bool stop_at_first_intersection_;
        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_triangles_;
    };
} // namespace

namespace geode
{
    template < index_t dimension >
    class TriangulatedSurfaceIntersections< dimension >::Impl
    {
    public:
        Impl( const TriangulatedSurface< dimension >& mesh )
            : mesh_( mesh ), verbose_( false )
        {
        }

        bool mesh_has_self_intersections() const
        {
            const auto intersections = intersecting_triangles( true );
            if( intersections.empty() )
            {
                return false;
            }
            return true;
        }

        InspectionIssues< std::pair< index_t, index_t > >
            intersecting_elements() const
        {
            const auto intersections = intersecting_triangles( false );
            InspectionIssues< std::pair< index_t, index_t > > issues{
                "Triangle - triangle intersections."
            };

            for( const auto& triangle_pair : intersections )
            {
                issues.add_issue( triangle_pair,
                    absl::StrCat( "Triangles ", triangle_pair.first, " and ",
                        triangle_pair.second, " intersect each other." ) );
            }
            return issues;
        }

    private:
        std::vector< std::pair< index_t, index_t > > intersecting_triangles(
            bool stop_at_first_intersection ) const
        {
            const auto surface_aabb = create_aabb_tree( mesh_ );
            TriangleTriangleIntersection< dimension > action{ mesh_,
                stop_at_first_intersection };
            surface_aabb.compute_self_element_bbox_intersections( action );
            return action.intersecting_triangles();
        }

    private:
        const TriangulatedSurface< dimension >& mesh_;
        DEBUG_CONST bool verbose_;
    };

    template < index_t dimension >
    TriangulatedSurfaceIntersections< dimension >::
        TriangulatedSurfaceIntersections(
            const TriangulatedSurface< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceIntersections<
        dimension >::~TriangulatedSurfaceIntersections() = default;

    template < index_t dimension >
    bool TriangulatedSurfaceIntersections<
        dimension >::mesh_has_self_intersections() const
    {
        return impl_->mesh_has_self_intersections();
    }

    template < index_t dimension >
    InspectionIssues< std::pair< index_t, index_t > >
        TriangulatedSurfaceIntersections< dimension >::intersecting_elements()
            const
    {
        return impl_->intersecting_elements();
    }

    template class opengeode_inspector_inspector_api
        TriangulatedSurfaceIntersections< 2 >;
    template class opengeode_inspector_inspector_api
        TriangulatedSurfaceIntersections< 3 >;
} // namespace geode
