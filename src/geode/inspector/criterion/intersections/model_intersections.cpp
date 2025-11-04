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

#include <geode/inspector/criterion/intersections/model_intersections.hpp>

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

#include <geode/model/helpers/aabb_model_helpers.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    struct ComponentOverlap
    {
        bool operator()(
            geode::index_t first_component, geode::index_t second_component )
        {
            std::lock_guard< std::mutex > lock( mutex );
            component_pairs.emplace_back( first_component, second_component );
            return false;
        }

        std::mutex mutex;
        std::vector< std::pair< geode::index_t, geode::index_t > >
            component_pairs;
    };

    absl::InlinedVector< geode::PolygonVertices, 1 > polygon_fan_triangles(
        const geode::PolygonVertices& polygon, geode::local_index_t apex )
    {
        absl::InlinedVector< geode::PolygonVertices, 1 > triangles;
        for( const auto edge : geode::LRange{ 1, polygon.size() - 1 } )
        {
            const auto next_apex = ( apex + edge ) % polygon.size();
            const auto next_next_apex =
                next_apex + 1 == polygon.size() ? 0u : next_apex + 1;
            triangles.emplace_back( geode::PolygonVertices{
                polygon[apex], polygon[next_apex], polygon[next_next_apex] } );
        }
        return triangles;
    }

    template < typename Model >
    class ModelSurfacesIntersectionBase
    {
    public:
        ModelSurfacesIntersectionBase( const Model& model,
            const geode::uuid& surface_id1,
            const geode::uuid& surface_id2 )
            : model_( model ),
              same_surface_{ surface_id1 == surface_id2 },
              surface1_( model.surface( surface_id1 ) ),
              surface2_( model.surface( surface_id2 ) ),
              mesh1_( surface1_.mesh() ),
              mesh2_( same_surface_ ? mesh1_ : surface2_.mesh() )
        {
        }

        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_polygons()
        {
            return std::move( intersecting_polygons_ );
        }

    protected:
        bool polygons_intersect(
            geode::index_t p1_id, geode::index_t p2_id ) const
        {
            const auto p1_vertices = this->mesh1().polygon_vertices( p1_id );
            const auto p2_vertices = this->mesh2().polygon_vertices( p2_id );
            if( p1_vertices.size() < 3 || p2_vertices.size() < 3 )
            {
                return false;
            }
            if( p1_vertices.size() == 3 && p2_vertices.size() == 3 )
            {
                return triangles_intersection_detection(
                    p1_vertices, p2_vertices );
            }
            const auto p1_triangles = polygon_fan_triangles( p1_vertices, 0 );
            const auto p2_triangles = polygon_fan_triangles( p2_vertices, 0 );
            for( const auto& p1_triangle : p1_triangles )
            {
                for( const auto& p2_triangle : p2_triangles )
                {
                    if( triangles_intersection_detection(
                            p1_triangle, p2_triangle ) )
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        bool triangles_intersection_detection(
            const geode::PolygonVertices& t1_vertices,
            const geode::PolygonVertices& t2_vertices ) const
        {
            const auto common_vertices =
                this->triangles_common_vertices( t1_vertices, t2_vertices );
            return common_vertices.size() == 3
                   || this->triangles_intersect(
                       t1_vertices, t2_vertices, common_vertices );
        }

        absl::InlinedVector< std::array< geode::index_t, 2 >, 3 >
            triangles_common_vertices(
                const geode::PolygonVertices& t1_vertices,
                const geode::PolygonVertices& t2_vertices ) const
        {
            absl::InlinedVector< std::array< geode::index_t, 2 >, 3 >
                common_vertices;
            for( const auto v1_id : t1_vertices )
            {
                const auto v1_unique_vertex =
                    model_.unique_vertex( { surface1_.component_id(), v1_id } );
                for( const auto v2_id : t2_vertices )
                {
                    if( v1_unique_vertex
                        == model_.unique_vertex(
                            { surface2_.component_id(), v2_id } ) )
                    {
                        common_vertices.push_back( { v1_id, v2_id } );
                        break;
                    }
                }
            }
            return common_vertices;
        }

        bool triangles_intersect( const geode::PolygonVertices& t1_vertices,
            const geode::PolygonVertices& t2_vertices,
            absl::Span< const std::array< geode::index_t, 2 > >
                common_vertices ) const;

        void emplace( geode::index_t p1_id, geode::index_t p2_id )
        {
            std::lock_guard< std::mutex > lock( mutex_ );
            intersecting_polygons_.emplace_back( p1_id, p2_id );
        }

        const geode::SurfaceMesh< Model::dim >& mesh1() const
        {
            return mesh1_;
        }

        const geode::SurfaceMesh< Model::dim >& mesh2() const
        {
            return mesh2_;
        }

        geode::Triangle< Model::dim > mesh_triangle(
            const geode::SurfaceMesh< Model::dim >& mesh,
            const geode::PolygonVertices& triangle_vertices ) const
        {
            return { mesh.point( triangle_vertices[0] ),
                mesh.point( triangle_vertices[1] ),
                mesh.point( triangle_vertices[2] ) };
        }

    private:
        const Model& model_;
        DEBUG_CONST bool same_surface_;
        const geode::Surface< Model::dim >& surface1_;
        const geode::Surface< Model::dim >& surface2_;
        const geode::SurfaceMesh< Model::dim >& mesh1_;
        const geode::SurfaceMesh< Model::dim >& mesh2_;
        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_polygons_;
        std::mutex mutex_;
    };

    template < typename Model >
    class OneModelSurfacesIntersection
        : public ModelSurfacesIntersectionBase< Model >
    {
    public:
        OneModelSurfacesIntersection( const Model& model,
            const geode::uuid& surface_id1,
            const geode::uuid& surface_id2 )
            : ModelSurfacesIntersectionBase< Model >(
                  model, surface_id1, surface_id2 ),
              same_surface_{ surface_id1 == surface_id2 }
        {
        }

        bool operator()( geode::index_t p1_id, geode::index_t p2_id )
        {
            if( same_surface_ && p1_id == p2_id )
            {
                return false;
            }
            if( this->polygons_intersect( p1_id, p2_id ) )
            {
                this->emplace( p1_id, p2_id );
                return true;
            }
            return false;
        }

    private:
        DEBUG_CONST bool same_surface_;
    };

    template < typename Model >
    class AllModelSurfacesIntersection
        : public ModelSurfacesIntersectionBase< Model >
    {
    public:
        AllModelSurfacesIntersection( const Model& model,
            const geode::uuid& surface_id1,
            const geode::uuid& surface_id2 )
            : ModelSurfacesIntersectionBase< Model >(
                  model, surface_id1, surface_id2 ),
              same_surface_{ surface_id1 == surface_id2 }
        {
        }

        bool operator()( geode::index_t p1_id, geode::index_t p2_id )
        {
            if( same_surface_ && p1_id == p2_id )
            {
                return false;
            }
            if( this->polygons_intersect( p1_id, p2_id ) )
            {
                this->emplace( p1_id, p2_id );
            }
            return false;
        }

    private:
        DEBUG_CONST bool same_surface_;
    };

    template < typename Model >
    class AllModelSurfacesAutoIntersection
        : public ModelSurfacesIntersectionBase< Model >
    {
    public:
        AllModelSurfacesAutoIntersection( const Model& model,
            const geode::uuid& surface_id1,
            const geode::uuid& surface_id2 )
            : ModelSurfacesIntersectionBase< Model >(
                  model, surface_id1, surface_id2 ),
              same_surface_{ surface_id1 == surface_id2 }
        {
        }

        bool operator()( geode::index_t p1_id, geode::index_t p2_id )
        {
            if( !same_surface_ )
            {
                return false;
            }
            if( same_surface_ && p1_id == p2_id )
            {
                return false;
            }
            if( this->polygons_intersect( p1_id, p2_id ) )
            {
                this->emplace( p1_id, p2_id );
            }
            return false;
        }

    private:
        DEBUG_CONST bool same_surface_;
    };

    geode::index_t third_point_index( const geode::PolygonVertices& vertices,
        absl::Span< const std::array< geode::index_t, 2 > > common_vertices,
        geode::local_index_t vertex_position )
    {
        for( const auto vertex_id : vertices )
        {
            if( vertex_id == common_vertices[0][vertex_position]
                || vertex_id == common_vertices[1][vertex_position] )
            {
                continue;
            }
            return vertex_id;
        }
        OPENGEODE_ASSERT_NOT_REACHED( "Should have found a third point index "
                                      "in second PolygonVertices." );
        return geode::NO_ID;
    }

    template <>
    bool ModelSurfacesIntersectionBase< geode::Section >::triangles_intersect(
        const geode::PolygonVertices& t1_vertices,
        const geode::PolygonVertices& t2_vertices,
        absl::Span< const std::array< geode::index_t, 2 > > common_vertices )
        const
    {
        if( common_vertices.size() == 2 )
        {
            const auto& common_pt0 = mesh1_.point( common_vertices[0][0] );
            const auto& common_pt1 = mesh1_.point( common_vertices[1][1] );
            const auto& t1_third_pt = mesh1_.point(
                third_point_index( t1_vertices, common_vertices, 0 ) );
            const auto& t2_third_pt = mesh2_.point(
                third_point_index( t2_vertices, common_vertices, 1 ) );
            if( geode::segment_segment_intersection_detection(
                    { common_pt0, t1_third_pt }, { common_pt1, t2_third_pt } )
                        .first
                    != geode::POSITION::outside
                || geode::segment_segment_intersection_detection(
                       { common_pt1, t1_third_pt },
                       { common_pt0, t2_third_pt } )
                           .first
                       != geode::POSITION::outside )
            {
                return true;
            }
            const auto t1 = mesh_triangle( mesh1_, t1_vertices );
            const auto t2 = mesh_triangle( mesh2_, t2_vertices );
            return geode::point_triangle_position( t1_third_pt, t2 )
                       != geode::POSITION::outside
                   || geode::point_triangle_position( t2_third_pt, t1 )
                          != geode::POSITION::outside;
        }
        for( const auto t1_edge_v : geode::LRange{ 3 } )
        {
            const std::array< geode::index_t, 2 > edge1_vertices{
                t1_vertices[t1_edge_v],
                t1_vertices[t1_edge_v == 2 ? 0 : t1_edge_v + 1]
            };
            const geode::Segment2D t1_edge{ mesh1_.point( edge1_vertices[0] ),
                mesh1_.point( edge1_vertices[1] ) };
            for( const auto t2_edge_v : geode::LRange{ 3 } )
            {
                const std::array< geode::index_t, 2 > edge2_vertices{
                    t2_vertices[t2_edge_v],
                    t2_vertices[t2_edge_v == 2 ? 0 : t2_edge_v + 1]
                };
                const auto edge_edge_inter =
                    geode::segment_segment_intersection_detection(
                        t1_edge, { mesh2_.point( edge2_vertices[0] ),
                                     mesh2_.point( edge2_vertices[1] ) } );
                if( edge_edge_inter.first == geode::POSITION::outside
                    || edge_edge_inter.first == geode::POSITION::parallel )
                {
                    continue;
                }
                if( common_vertices.size() != 1 )
                {
                    return true;
                }
                const auto t1_edge_inter_pt_id =
                    geode::detail::position_to_index( edge_edge_inter.first );
                const auto t2_edge_inter_pt_id =
                    geode::detail::position_to_index( edge_edge_inter.second );
                if( t1_edge_inter_pt_id == geode::NO_LID
                    || t2_edge_inter_pt_id == geode::NO_LID )
                {
                    return true;
                }
                if( edge1_vertices[t1_edge_inter_pt_id] != common_vertices[0][0]
                    || edge2_vertices[t2_edge_inter_pt_id]
                           != common_vertices[0][1] )
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
        absl::Span< const std::array< geode::index_t, 2 > > common_vertices,
        geode::local_index_t t2_vertex_position )
    {
        for( const auto v_id : geode::LRange{ 3 } )
        {
            const auto v2_id = v_id == 2 ? 0 : v_id + 1;
            const auto intersection = segment_triangle_intersection_detection(
                { t1.vertices()[v_id], t1.vertices()[v2_id] }, t2 );
            if( intersection.first != geode::POSITION::outside )
            {
                if( common_vertices.size() != 1 )
                {
                    return true;
                }
                const auto edge_inter_pt_id =
                    geode::detail::position_to_index( intersection.first );
                const auto t2_inter_pt_id =
                    geode::detail::position_to_index( intersection.second );
                if( edge_inter_pt_id == geode::NO_LID
                    || t2_inter_pt_id == geode::NO_LID )
                {
                    return true;
                }
                if( t1_vertices[edge_inter_pt_id == 0 ? v_id : v2_id]
                        != common_vertices[0][t2_vertex_position == 0 ? 1 : 0]
                    || t2_vertices[t2_inter_pt_id]
                           != common_vertices[0][t2_vertex_position] )
                {
                    return true;
                }
            }
        }
        return false;
    }

    template <>
    bool ModelSurfacesIntersectionBase< geode::BRep >::triangles_intersect(
        const geode::PolygonVertices& t1_vertices,
        const geode::PolygonVertices& t2_vertices,
        absl::Span< const std::array< geode::index_t, 2 > > common_vertices )
        const
    {
        const auto t2 = mesh_triangle( mesh2_, t2_vertices );
        if( common_vertices.size() == 2 )
        {
            const auto& t1_third_pt = mesh1_.point(
                third_point_index( t1_vertices, common_vertices, 0 ) );
            return geode::segment_triangle_intersection_detection(
                       { mesh1_.point( common_vertices[0][0] ), t1_third_pt },
                       t2 )
                           .first
                       == geode::POSITION::parallel
                   || geode::segment_triangle_intersection_detection(
                          { mesh1_.point( common_vertices[1][0] ),
                              t1_third_pt },
                          t2 )
                              .first
                          == geode::POSITION::parallel;
        }
        const auto t1 = mesh_triangle( mesh1_, t1_vertices );
        return triangle_intersects_other(
                   t1, t2, t1_vertices, t2_vertices, common_vertices, 1 )
               || triangle_intersects_other(
                   t2, t1, t2_vertices, t1_vertices, common_vertices, 0 );
    }
} // namespace

namespace geode
{
    index_t ElementsIntersectionsInspectionResult::nb_issues() const
    {
        return elements_intersections.nb_issues();
    }

    std::string ElementsIntersectionsInspectionResult::string() const
    {
        return elements_intersections.string();
    }

    std::string ElementsIntersectionsInspectionResult::inspection_type() const
    {
        return "Intersections inspection";
    }

    template < typename Model >
    class ModelMeshesIntersections< Model >::Impl
    {
    public:
        Impl( const Model& model ) : model_( model ) {}

        bool model_has_intersecting_surfaces() const
        {
            const auto intersections = intersecting_polygons<
                OneModelSurfacesIntersection< Model > >();
            if( intersections.empty() )
            {
                return false;
            }
            return true;
        }

        void add_intersecting_surfaces_elements(
            InspectionIssues< std::pair< ComponentMeshElement,
                ComponentMeshElement > >& intersection_issues ) const
        {
            const auto intersections = intersecting_polygons<
                AllModelSurfacesIntersection< Model > >();
            for( const auto& polygon_pair : intersections )
            {
                const auto& surface1 =
                    model_.surface( polygon_pair.first.component_id.id() );
                const auto& surface2 =
                    model_.surface( polygon_pair.second.component_id.id() );
                intersection_issues.add_issue( polygon_pair,
                    absl::StrCat( "Surfaces ", surface1.name(), " (",
                        polygon_pair.first.component_id.id().string(), ") and ",
                        surface2.name(), " (",
                        polygon_pair.second.component_id.id().string(),
                        ") intersect on polygons ",
                        polygon_pair.first.element_id, " and ",
                        polygon_pair.second.element_id ) );
            }
        }

        void add_surface_auto_intersecting_elements(
            InspectionIssues< std::pair< ComponentMeshElement,
                ComponentMeshElement > >& intersection_issues ) const
        {
            const auto intersections = intersecting_polygons<
                AllModelSurfacesAutoIntersection< Model > >();
            for( const auto& polygon_pair : intersections )
            {
                const auto& surface =
                    model_.surface( polygon_pair.first.component_id.id() );
                intersection_issues.add_issue( polygon_pair,
                    absl::StrCat( "Surface ", surface.name(), " (",
                        polygon_pair.first.component_id.id().string(),
                        ") has a self intersection on polygons ",
                        polygon_pair.first.element_id, "and ",
                        polygon_pair.second.element_id ) );
            }
        }

    private:
        template < typename Action >
        std::vector< std::pair< ComponentMeshElement, ComponentMeshElement > >
            intersecting_polygons() const
        {
            std::vector<
                std::pair< ComponentMeshElement, ComponentMeshElement > >
                component_intersections;
            for( const auto& surface : model_.surfaces() )
            {
                if( surface.mesh().nb_polygons() == 0 )
                {
                    geode::Logger::warn(
                        "One of the surface meshes has an empty mesh, cannot "
                        "compute the AABBTree used for detecting the mesh "
                        "intersections, no intersections will be computed." );
                    return component_intersections;
                }
            }
            const auto model_tree = create_surface_meshes_aabb_trees( model_ );
            for( const auto& surface : model_.surfaces() )
            {
                Action surfaces_intersection_action{ model_, surface.id(),
                    surface.id() };
                model_tree
                    .mesh_trees_[model_tree.mesh_tree_ids_.at( surface.id() )]
                    .compute_self_element_bbox_intersections(
                        surfaces_intersection_action );
                for( const auto& polygon_pair :
                    surfaces_intersection_action.intersecting_polygons() )
                {
                    component_intersections.emplace_back(
                        ComponentMeshElement{
                            surface.component_id(), polygon_pair.first },
                        ComponentMeshElement{
                            surface.component_id(), polygon_pair.second } );
                }
            }
            ComponentOverlap surfaces_overlap;
            model_tree.components_tree_.compute_self_element_bbox_intersections(
                surfaces_overlap );
            for( const auto& components : surfaces_overlap.component_pairs )
            {
                const auto surface_uuid1 = model_tree.uuids_[components.first];
                const auto surface_uuid2 = model_tree.uuids_[components.second];
                Action surfaces_intersection_action{ model_, surface_uuid1,
                    surface_uuid2 };
                model_tree.mesh_trees_[components.first]
                    .compute_other_element_bbox_intersections(
                        model_tree.mesh_trees_[components.second],
                        surfaces_intersection_action );
                const auto component_id1 =
                    model_.surface( surface_uuid1 ).component_id();
                const auto component_id2 =
                    model_.surface( surface_uuid2 ).component_id();
                for( const auto& polygon_pair :
                    surfaces_intersection_action.intersecting_polygons() )
                {
                    component_intersections.emplace_back(
                        ComponentMeshElement{
                            component_id1, polygon_pair.first },
                        ComponentMeshElement{
                            component_id2, polygon_pair.second } );
                }
            }
            return component_intersections;
        }

    private:
        const Model& model_;
    };

    template < typename Model >
    ModelMeshesIntersections< Model >::ModelMeshesIntersections(
        const Model& model )
        : impl_( model )
    {
    }

    template < typename Model >
    ModelMeshesIntersections< Model >::~ModelMeshesIntersections() = default;

    template < typename Model >
    bool ModelMeshesIntersections< Model >::model_has_intersecting_surfaces()
        const
    {
        return impl_->model_has_intersecting_surfaces();
    }

    template < typename Model >
    ElementsIntersectionsInspectionResult
        ModelMeshesIntersections< Model >::inspect_intersections() const
    {
        ElementsIntersectionsInspectionResult results;
        impl_->add_intersecting_surfaces_elements(
            results.elements_intersections );
        return results;
    }

    template < typename Model >
    ElementsIntersectionsInspectionResult
        ModelMeshesIntersections< Model >::inspect_surfaces_self_intersections()
            const
    {
        ElementsIntersectionsInspectionResult results;
        impl_->add_surface_auto_intersecting_elements(
            results.elements_intersections );
        return results;
    }

    template class opengeode_inspector_inspector_api
        ModelMeshesIntersections< Section >;
    template class opengeode_inspector_inspector_api
        ModelMeshesIntersections< BRep >;
} // namespace geode
