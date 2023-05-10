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

#include <geode/inspector/criterion/intersections/surface_intersections.h>

#include <absl/algorithm/container.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/aabb.h>
#include <geode/geometry/basic_objects/segment.h>
#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/information.h>
#include <geode/geometry/intersection_detection.h>
#include <geode/geometry/position.h>

#include <geode/mesh/core/triangulated_surface.h>
#include <geode/mesh/helpers/aabb_surface_helpers.h>

namespace
{
    geode::local_index_t vertex_position_to_index( geode::Position position )
    {
        if( position == geode::Position::vertex0 )
        {
            return 0;
        }
        if( position == geode::Position::vertex1 )
        {
            return 1;
        }
        if( position == geode::Position::vertex2 )
        {
            return 2;
        }
        return geode::NO_LID;
    }

    template < geode::index_t dimension >
    class TriangleTriangleIntersectionBase
    {
    public:
        TriangleTriangleIntersectionBase(
            const geode::TriangulatedSurface< dimension >& mesh )
            : mesh_( mesh )
        {
        }

        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_triangles()
        {
            return std::move( intersecting_triangles_ );
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

        geode::PolygonVertices triangles_common_points(
            const geode::PolygonVertices& t1_vertices,
            const geode::PolygonVertices& t2_vertices ) const
        {
            geode::PolygonVertices common_points;
            for( const auto vertex_id : t1_vertices )
            {
                if( absl::c_find( t2_vertices, vertex_id )
                    != t2_vertices.end() )
                {
                    common_points.push_back( vertex_id );
                }
            }
            return common_points;
        }

        bool triangles_intersect( geode::index_t t1_id,
            geode::index_t t2_id,
            const geode::PolygonVertices& t1_vertices,
            const geode::PolygonVertices& t2_vertices,
            const geode::PolygonVertices& common_points ) const;

    private:
        const geode::TriangulatedSurface< dimension >& mesh_;
        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_triangles_;
    };

    template < geode::index_t dimension >
    class OneTriangleTriangleIntersection
        : public TriangleTriangleIntersectionBase< dimension >
    {
    public:
        OneTriangleTriangleIntersection(
            const geode::TriangulatedSurface< dimension >& surface )
            : TriangleTriangleIntersectionBase< dimension >( surface )
        {
        }

        bool operator()( geode::index_t t1_id, geode::index_t t2_id )
        {
            if( t1_id == t2_id )
            {
                return false;
            }
            const auto t1_vertices = this->mesh().polygon_vertices( t1_id );
            const auto t2_vertices = this->mesh().polygon_vertices( t2_id );
            const auto common_points =
                this->triangles_common_points( t1_vertices, t2_vertices );
            if( common_points.size() == 3
                || this->triangles_intersect(
                    t1_id, t2_id, t1_vertices, t2_vertices, common_points ) )
            {
                this->emplace( t1_id, t2_id );
                return true;
            }
            return false;
        }
    };

    template < geode::index_t dimension >
    class AllTriangleTriangleIntersection
        : public TriangleTriangleIntersectionBase< dimension >
    {
    public:
        AllTriangleTriangleIntersection(
            const geode::TriangulatedSurface< dimension >& surface )
            : TriangleTriangleIntersectionBase< dimension >( surface )
        {
        }

        bool operator()( geode::index_t t1_id, geode::index_t t2_id )
        {
            if( t1_id == t2_id )
            {
                return false;
            }
            const auto t1_vertices = this->mesh().polygon_vertices( t1_id );
            const auto t2_vertices = this->mesh().polygon_vertices( t2_id );
            const auto common_points =
                this->triangles_common_points( t1_vertices, t2_vertices );
            if( common_points.size() == 3
                || this->triangles_intersect(
                    t1_id, t2_id, t1_vertices, t2_vertices, common_points ) )
            {
                this->emplace( t1_id, t2_id );
            }
            return false;
        }
    };

    geode::index_t third_point_index( const geode::PolygonVertices& vertices,
        const geode::PolygonVertices& vertices_to_avoid )
    {
        for( const auto vertex_id : vertices )
        {
            if( vertex_id == vertices_to_avoid[0]
                || vertex_id == vertices_to_avoid[1] )
            {
                continue;
            }
            return vertex_id;
        }
        OPENGEODE_ASSERT_NOT_REACHED( "Should have found a third point index" );
        return geode::NO_ID;
    }

    template <>
    bool TriangleTriangleIntersectionBase< 2 >::triangles_intersect(
        geode::index_t t1_id,
        geode::index_t t2_id,
        const geode::PolygonVertices& t1_vertices,
        const geode::PolygonVertices& t2_vertices,
        const geode::PolygonVertices& common_points ) const
    {
        if( common_points.size() == 2 )
        {
            const auto& common_pt0 = mesh_.point( common_points[0] );
            const auto& common_pt1 = mesh_.point( common_points[1] );
            const auto& t1_third_pt =
                mesh_.point( third_point_index( t1_vertices, common_points ) );
            const auto& t2_third_pt =
                mesh_.point( third_point_index( t2_vertices, common_points ) );
            if( geode::segment_segment_intersection_detection(
                    { common_pt0, t1_third_pt }, { common_pt1, t2_third_pt } )
                        .first
                    != geode::Position::outside
                || geode::segment_segment_intersection_detection(
                       { common_pt1, t1_third_pt },
                       { common_pt0, t2_third_pt } )
                           .first
                       != geode::Position::outside )
            {
                return true;
            }
            const auto t1 = mesh_.triangle( t1_id );
            const auto t2 = mesh_.triangle( t2_id );
            if( geode::point_triangle_position( t1_third_pt, t2 )
                    != geode::Position::outside
                || geode::point_triangle_position( t2_third_pt, t1 )
                       != geode::Position::outside )
            {
                return true;
            }
            return false;
        }
        for( const auto t1_edge_v : geode::LRange{ 3 } )
        {
            const std::array< geode::index_t, 2 > edge1_vertices{
                t1_vertices[t1_edge_v],
                t1_vertices[t1_edge_v == 2 ? 0 : t1_edge_v + 1]
            };
            const geode::Segment2D t1_edge{ mesh_.point( edge1_vertices[0] ),
                mesh_.point( edge1_vertices[1] ) };
            for( const auto t2_edge_v : geode::LRange{ 3 } )
            {
                const std::array< geode::index_t, 2 > edge2_vertices{
                    t2_vertices[t2_edge_v],
                    t2_vertices[t2_edge_v == 2 ? 0 : t2_edge_v + 1]
                };
                const auto edge_edge_inter =
                    geode::segment_segment_intersection_detection(
                        t1_edge, { mesh_.point( edge2_vertices[0] ),
                                     mesh_.point( edge2_vertices[1] ) } );
                if( edge_edge_inter.first == geode::Position::outside
                    || edge_edge_inter.first == geode::Position::parallel )
                {
                    continue;
                }
                if( common_points.size() != 1 )
                {
                    return true;
                }
                const auto t1_edge_inter_pt_id =
                    vertex_position_to_index( edge_edge_inter.first );
                const auto t2_edge_inter_pt_id =
                    vertex_position_to_index( edge_edge_inter.second );
                if( t1_edge_inter_pt_id == geode::NO_LID
                    || t2_edge_inter_pt_id == geode::NO_LID )
                {
                    return true;
                }
                if( edge1_vertices[t1_edge_inter_pt_id]
                        != edge2_vertices[t2_edge_inter_pt_id]
                    || edge1_vertices[t1_edge_inter_pt_id] != common_points[0] )
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool triangle_intersects_other( const geode::Triangle3D& t1,
        const geode::Triangle3D& t2,
        const geode::PolygonVertices& t1_vertices,
        const geode::PolygonVertices& t2_vertices,
        const geode::PolygonVertices& common_points )
    {
        for( const auto v_id : geode::LRange{ 3 } )
        {
            const auto v2_id = v_id == 2 ? 0 : v_id + 1;
            const auto intersection = segment_triangle_intersection_detection(
                { t1.vertices()[v_id], t1.vertices()[v2_id] }, t2 );
            if( intersection.first != geode::Position::outside )
            {
                if( common_points.size() != 1 )
                {
                    return true;
                }
                const auto edge_inter_pt_id =
                    vertex_position_to_index( intersection.first );
                const auto t2_inter_pt_id =
                    vertex_position_to_index( intersection.second );
                if( edge_inter_pt_id == geode::NO_LID
                    || t2_inter_pt_id == geode::NO_LID )
                {
                    return true;
                }
                if( t1_vertices[edge_inter_pt_id == 0 ? v_id : v2_id]
                        != t2_vertices[t2_inter_pt_id]
                    || t2_vertices[t2_inter_pt_id] != common_points[0] )
                {
                    return true;
                }
            }
        }
        return false;
    }

    template <>
    bool TriangleTriangleIntersectionBase< 3 >::triangles_intersect(
        geode::index_t t1_id,
        geode::index_t t2_id,
        const geode::PolygonVertices& t1_vertices,
        const geode::PolygonVertices& t2_vertices,
        const geode::PolygonVertices& common_points ) const
    {
        const auto t2 = mesh_.triangle( t2_id );
        if( common_points.size() == 2 )
        {
            const auto& t1_third_pt =
                mesh_.point( third_point_index( t1_vertices, common_points ) );
            if( geode::segment_triangle_intersection_detection(
                    { mesh_.point( common_points[0] ), t1_third_pt }, t2 )
                        .first
                    == geode::Position::parallel
                || geode::segment_triangle_intersection_detection(
                       { mesh_.point( common_points[1] ), t1_third_pt }, t2 )
                           .first
                       == geode::Position::parallel )
            {
                return true;
            }
            return false;
        }
        const auto t1 = mesh_.triangle( t1_id );
        if( triangle_intersects_other(
                t1, t2, t1_vertices, t2_vertices, common_points )
            || triangle_intersects_other(
                t2, t1, t2_vertices, t1_vertices, common_points ) )
        {
            return true;
        }
        return false;
    }
} // namespace

namespace geode
{
    template < index_t dimension >
    class TriangulatedSurfaceIntersections< dimension >::Impl
    {
    public:
        Impl( const TriangulatedSurface< dimension >& mesh, bool verbose )
            : mesh_( mesh ), verbose_( verbose )
        {
        }

        bool mesh_has_self_intersections() const
        {
            const auto intersections = intersecting_triangles<
                OneTriangleTriangleIntersection< dimension > >();
            if( intersections.empty() )
            {
                return false;
            }
            return true;
        }

        index_t nb_intersecting_elements_pair() const
        {
            const auto intersections = intersecting_triangles<
                AllTriangleTriangleIntersection< dimension > >();
            if( verbose_ )
            {
                for( const auto& triangle_pair : intersections )
                {
                    Logger::info( "Triangles ", triangle_pair.first, " and ",
                        triangle_pair.second, " intersect each other." );
                }
            }
            return intersections.size();
        }

        std::vector< std::pair< index_t, index_t > >
            intersecting_elements() const
        {
            const auto intersections = intersecting_triangles<
                AllTriangleTriangleIntersection< dimension > >();
            if( verbose_ )
            {
                for( const auto& triangle_pair : intersections )
                {
                    Logger::info( "Triangles ", triangle_pair.first, " and ",
                        triangle_pair.second, " intersect each other." );
                }
            }
            return intersections;
        }

    private:
        template < typename Action >
        std::vector< std::pair< index_t, index_t > >
            intersecting_triangles() const
        {
            const auto surface_aabb = create_aabb_tree( mesh_ );
            Action action{ mesh_ };
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
        : impl_( mesh, false )
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceIntersections< dimension >::
        TriangulatedSurfaceIntersections(
            const TriangulatedSurface< dimension >& mesh, bool verbose )
        : impl_( mesh, verbose )
    {
    }

    template < index_t dimension >
    TriangulatedSurfaceIntersections<
        dimension >::~TriangulatedSurfaceIntersections()
    {
    }

    template < index_t dimension >
    bool TriangulatedSurfaceIntersections<
        dimension >::mesh_has_self_intersections() const
    {
        return impl_->mesh_has_self_intersections();
    }

    template < index_t dimension >
    index_t TriangulatedSurfaceIntersections<
        dimension >::nb_intersecting_elements_pair() const
    {
        return impl_->nb_intersecting_elements_pair();
    }

    template < index_t dimension >
    std::vector< std::pair< index_t, index_t > >
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
