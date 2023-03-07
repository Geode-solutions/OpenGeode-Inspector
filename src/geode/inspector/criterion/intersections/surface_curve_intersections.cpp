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

#include <geode/inspector/criterion/intersections/surface_curve_intersections.h>

#include <absl/algorithm/container.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/information.h>
#include <geode/geometry/intersection_detection.h>
#include <geode/geometry/position.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/aabb_edged_curve_helpers.h>
#include <geode/mesh/helpers/aabb_surface_helpers.h>

namespace
{
    template < geode::index_t dimension >
    class TriangleEdgeIntersectionBase
    {
    public:
        TriangleEdgeIntersectionBase(
            const geode::TriangulatedSurface< dimension >& surface,
            const geode::EdgedCurve< dimension >& curve )
            : surface_( surface ), curve_( curve )
        {
        }

        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_elements()
        {
            return std::move( intersecting_elements_ );
        }

    protected:
        bool edge_intersects_triangle(
            geode::index_t triangle_id, geode::index_t edge_id ) const;

        void emplace( geode::index_t triangle_id, geode::index_t edge_id )
        {
            intersecting_elements_.emplace_back( triangle_id, edge_id );
        }

    private:
        const geode::TriangulatedSurface< dimension >& surface_;
        const geode::EdgedCurve< dimension >& curve_;
        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_elements_;
    };

    template < geode::index_t dimension >
    class OneTriangleEdgeIntersection
        : public TriangleEdgeIntersectionBase< dimension >
    {
    public:
        OneTriangleEdgeIntersection(
            const geode::TriangulatedSurface< dimension >& surface,
            const geode::EdgedCurve< dimension >& curve )
            : TriangleEdgeIntersectionBase< dimension >( surface, curve )
        {
        }

        bool operator()( geode::index_t triangle_id, geode::index_t edge_id )
        {
            if( this->edge_intersects_triangle( triangle_id, edge_id ) )
            {
                this->emplace( triangle_id, edge_id );
                return true;
            }
            return false;
        }
    };

    template < geode::index_t dimension >
    class AllTriangleEdgeIntersection
        : public TriangleEdgeIntersectionBase< dimension >
    {
    public:
        AllTriangleEdgeIntersection(
            const geode::TriangulatedSurface< dimension >& surface,
            const geode::EdgedCurve< dimension >& curve )
            : TriangleEdgeIntersectionBase< dimension >( surface, curve )
        {
        }

        bool operator()( geode::index_t triangle_id, geode::index_t edge_id )
        {
            if( this->edge_intersects_triangle( triangle_id, edge_id ) )
            {
                this->emplace( triangle_id, edge_id );
            }
            return false;
        }
    };

    template <>
    bool TriangleEdgeIntersectionBase< 2 >::edge_intersects_triangle(
        geode::index_t triangle_id, geode::index_t edge_id ) const
    {
        const auto triangle = surface_.triangle( triangle_id );
        const auto segment = curve_.segment( edge_id );
        for( const auto ev : geode::LRange{ 2 } )
        {
            if( geode::point_triangle_position(
                    segment.vertices()[ev].get(), triangle )
                == geode::Position::inside )
            {
                return true;
            }
        }
        for( const auto e : geode::LRange{ 3 } )
        {
            const auto edge_vertices =
                surface_.polygon_edge_vertices( { triangle_id, e } );
            const geode::Segment2D edge{ surface_.point( edge_vertices[0] ),
                surface_.point( edge_vertices[1] ) };
            const auto result =
                geode::segment_segment_intersection_detection( segment, edge );
            if( result.first == geode::Position::inside )
            {
                return true;
            }
            if( result.second == geode::Position::inside )
            {
                return true;
            }
        }
        return false;
    }

    template <>
    bool TriangleEdgeIntersectionBase< 3 >::edge_intersects_triangle(
        geode::index_t triangle_id, geode::index_t edge_id ) const
    {
        const auto triangle = surface_.triangle( triangle_id );
        const auto segment = curve_.segment( edge_id );
        for( const auto ev : geode::LRange{ 2 } )
        {
            if( geode::point_triangle_position(
                    segment.vertices()[ev].get(), triangle )
                == geode::Position::inside )
            {
                return true;
            }
        }
        const auto result =
            geode::segment_triangle_intersection_detection( segment, triangle );
        if( result.first == geode::Position::outside
            || result.second == geode::Position::outside )
        {
            return false;
        }

        if( result.first == geode::Position::inside )
        {
            return true;
        }

        if( result.second == geode::Position::inside
            || result.second == geode::Position::edge0
            || result.second == geode::Position::edge1
            || result.second == geode::Position::edge2 )
        {
            return true;
        }

        if( result.first == geode::Position::parallel )
        {
            for( const auto ev : geode::LRange{ 2 } )
            {
                const auto position = geode::point_triangle_position(
                    segment.vertices()[ev].get(), triangle );
                if( position != geode::Position::vertex0
                    && position != geode::Position::vertex1
                    && position != geode::Position::vertex2 )
                {
                    return true;
                }
            }
        }
        return false;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class SurfaceCurveIntersections< dimension >::Impl
    {
    public:
        Impl( const TriangulatedSurface< dimension >& surface,
            const EdgedCurve< dimension >& curve,
            bool verbose )
            : surface_( surface ), curve_( curve ), verbose_( verbose )
        {
        }

        bool meshes_have_intersections() const
        {
            const auto intersections = intersecting_triangles_with_edges<
                OneTriangleEdgeIntersection< dimension > >();
            if( intersections.empty() )
            {
                return false;
            }
            return true;
        }

        index_t nb_intersecting_elements_pair() const
        {
            const auto intersections = intersecting_triangles_with_edges<
                AllTriangleEdgeIntersection< dimension > >();
            if( verbose_ )
            {
                for( const auto& pair : intersections )
                {
                    Logger::info( "Triangle ", pair.first, " and edge ",
                        pair.second, " intersect each other." );
                }
            }
            return intersections.size();
        }

        std::vector< std::pair< index_t, index_t > >
            intersecting_elements() const
        {
            const auto intersections = intersecting_triangles_with_edges<
                AllTriangleEdgeIntersection< dimension > >();
            if( verbose_ )
            {
                for( const auto& pair : intersections )
                {
                    Logger::info( "Triangle ", pair.first, " and edge",
                        pair.second, " intersect each other." );
                }
            }
            return intersections;
        }

    private:
        template < typename Action >
        std::vector< std::pair< index_t, index_t > >
            intersecting_triangles_with_edges() const
        {
            const auto surface_aabb = create_aabb_tree( surface_ );
            const auto curve_aabb = create_aabb_tree( curve_ );
            Action action{ surface_, curve_ };
            surface_aabb.compute_other_element_bbox_intersections(
                curve_aabb, action );
            return action.intersecting_elements();
        }

    private:
        const TriangulatedSurface< dimension >& surface_;
        const EdgedCurve< dimension >& curve_;
        DEBUG_CONST bool verbose_;
    };

    template < index_t dimension >
    SurfaceCurveIntersections< dimension >::SurfaceCurveIntersections(
        const TriangulatedSurface< dimension >& surface,
        const EdgedCurve< dimension >& curve )
        : impl_( surface, curve, false )
    {
    }

    template < index_t dimension >
    SurfaceCurveIntersections< dimension >::SurfaceCurveIntersections(
        const TriangulatedSurface< dimension >& surface,
        const EdgedCurve< dimension >& curve,
        bool verbose )
        : impl_( surface, curve, verbose )
    {
    }

    template < index_t dimension >
    SurfaceCurveIntersections< dimension >::~SurfaceCurveIntersections()
    {
    }

    template < index_t dimension >
    bool SurfaceCurveIntersections< dimension >::meshes_have_intersections()
        const
    {
        return impl_->meshes_have_intersections();
    }

    template < index_t dimension >
    index_t
        SurfaceCurveIntersections< dimension >::nb_intersecting_elements_pair()
            const
    {
        return impl_->nb_intersecting_elements_pair();
    }

    template < index_t dimension >
    std::vector< std::pair< index_t, index_t > >
        SurfaceCurveIntersections< dimension >::intersecting_elements() const
    {
        return impl_->intersecting_elements();
    }

    template class opengeode_inspector_inspector_api
        SurfaceCurveIntersections< 2 >;
    template class opengeode_inspector_inspector_api
        SurfaceCurveIntersections< 3 >;
} // namespace geode
