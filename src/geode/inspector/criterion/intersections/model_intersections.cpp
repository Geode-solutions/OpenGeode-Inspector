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

#include <geode/inspector/criterion/intersections/model_intersections.h>

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

#include <geode/model/helpers/aabb_model_helpers.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>
#include <geode/model/representation/core/section.h>

namespace
{
    struct ComponentOverlap
    {
        bool operator()(
            geode::index_t first_component, geode::index_t second_component )
        {
            component_pairs.emplace_back( first_component, second_component );
            return false;
        }

        std::vector< std::pair< geode::index_t, geode::index_t > >
            component_pairs;
    };

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

    template < geode::index_t dimension, typename Model >
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
              mesh1_( surface1_.template mesh<
                      geode::TriangulatedSurface< dimension > >() ),
              mesh2_( same_surface_
                          ? mesh1_
                          : surface2_.template mesh<
                              geode::TriangulatedSurface< dimension > >() )
        {
        }

        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_triangles()
        {
            return std::move( intersecting_triangles_ );
        }

    protected:
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

        bool triangles_intersect( geode::index_t t1_id,
            geode::index_t t2_id,
            const geode::PolygonVertices& t1_vertices,
            const geode::PolygonVertices& t2_vertices,
            absl::Span< const std::array< geode::index_t, 2 > >
                common_vertices ) const;

        void emplace( geode::index_t t1_id, geode::index_t t2_id )
        {
            intersecting_triangles_.emplace_back( t1_id, t2_id );
        }

        const geode::TriangulatedSurface< dimension >& mesh1() const
        {
            return mesh1_;
        }

        const geode::TriangulatedSurface< dimension >& mesh2() const
        {
            return mesh2_;
        }

    private:
        const Model& model_;
        DEBUG_CONST bool same_surface_;
        const geode::Surface< dimension >& surface1_;
        const geode::Surface< dimension >& surface2_;
        const geode::TriangulatedSurface< dimension >& mesh1_;
        const geode::TriangulatedSurface< dimension >& mesh2_;
        std::vector< std::pair< geode::index_t, geode::index_t > >
            intersecting_triangles_;
    };

    template < geode::index_t dimension, typename Model >
    class OneModelSurfacesIntersection
        : public ModelSurfacesIntersectionBase< dimension, Model >
    {
    public:
        OneModelSurfacesIntersection( const Model& model,
            const geode::uuid& surface_id1,
            const geode::uuid& surface_id2 )
            : ModelSurfacesIntersectionBase< dimension, Model >(
                model, surface_id1, surface_id2 ),
              same_surface_{ surface_id1 == surface_id2 }
        {
        }

        bool operator()( geode::index_t t1_id, geode::index_t t2_id )
        {
            if( same_surface_ && t1_id == t2_id )
            {
                return false;
            }
            const auto t1_vertices = this->mesh1().polygon_vertices( t1_id );
            const auto t2_vertices = this->mesh2().polygon_vertices( t2_id );
            const auto common_vertices =
                this->triangles_common_vertices( t1_vertices, t2_vertices );
            if( common_vertices.size() == 3
                || this->triangles_intersect(
                    t1_id, t2_id, t1_vertices, t2_vertices, common_vertices ) )
            {
                this->emplace( t1_id, t2_id );
                return true;
            }
            return false;
        }

    private:
        DEBUG_CONST bool same_surface_;
    };

    template < geode::index_t dimension, typename Model >
    class AllModelSurfacesIntersection
        : public ModelSurfacesIntersectionBase< dimension, Model >
    {
    public:
        AllModelSurfacesIntersection( const Model& model,
            const geode::uuid& surface_id1,
            const geode::uuid& surface_id2 )
            : ModelSurfacesIntersectionBase< dimension, Model >(
                model, surface_id1, surface_id2 ),
              same_surface_{ surface_id1 == surface_id2 }
        {
        }

        bool operator()( geode::index_t t1_id, geode::index_t t2_id )
        {
            if( same_surface_ && t1_id == t2_id )
            {
                return false;
            }
            const auto t1_vertices = this->mesh1().polygon_vertices( t1_id );
            const auto t2_vertices = this->mesh2().polygon_vertices( t2_id );
            const auto common_vertices =
                this->triangles_common_vertices( t1_vertices, t2_vertices );
            if( common_vertices.size() == 3
                || this->triangles_intersect(
                    t1_id, t2_id, t1_vertices, t2_vertices, common_vertices ) )
            {
                this->emplace( t1_id, t2_id );
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
    bool
        ModelSurfacesIntersectionBase< 2, geode::Section >::triangles_intersect(
            geode::index_t t1_id,
            geode::index_t t2_id,
            const geode::PolygonVertices& t1_vertices,
            const geode::PolygonVertices& t2_vertices,
            absl::Span< const std::array< geode::index_t, 2 > >
                common_vertices ) const
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
                    != geode::Position::outside
                || geode::segment_segment_intersection_detection(
                       { common_pt1, t1_third_pt },
                       { common_pt0, t2_third_pt } )
                           .first
                       != geode::Position::outside )
            {
                return true;
            }
            const auto t1 = mesh1_.triangle( t1_id );
            const auto t2 = mesh2_.triangle( t2_id );
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
                if( edge_edge_inter.first == geode::Position::outside
                    || edge_edge_inter.first == geode::Position::parallel )
                {
                    continue;
                }
                if( common_vertices.size() != 1 )
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
            if( intersection.first != geode::Position::outside )
            {
                if( common_vertices.size() != 1 )
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
    bool ModelSurfacesIntersectionBase< 3, geode::BRep >::triangles_intersect(
        geode::index_t t1_id,
        geode::index_t t2_id,
        const geode::PolygonVertices& t1_vertices,
        const geode::PolygonVertices& t2_vertices,
        absl::Span< const std::array< geode::index_t, 2 > > common_vertices )
        const
    {
        const auto t2 = mesh2_.triangle( t2_id );
        if( common_vertices.size() == 2 )
        {
            const auto& t1_third_pt = mesh1_.point(
                third_point_index( t1_vertices, common_vertices, 0 ) );
            if( geode::segment_triangle_intersection_detection(
                    { mesh1_.point( common_vertices[0][0] ), t1_third_pt }, t2 )
                        .first
                    == geode::Position::parallel
                || geode::segment_triangle_intersection_detection(
                       { mesh1_.point( common_vertices[1][0] ), t1_third_pt },
                       t2 )
                           .first
                       == geode::Position::parallel )
            {
                return true;
            }
            return false;
        }
        const auto t1 = mesh1_.triangle( t1_id );
        if( triangle_intersects_other(
                t1, t2, t1_vertices, t2_vertices, common_vertices, 1 )
            || triangle_intersects_other(
                t2, t1, t2_vertices, t1_vertices, common_vertices, 0 ) )
        {
            return true;
        }
        return false;
    }
} // namespace

namespace geode
{
    template < index_t dimension, typename Model >
    class ModelMeshesIntersections< dimension, Model >::Impl
    {
    public:
        Impl( const Model& model, bool verbose )
            : model_( model ), verbose_( verbose )
        {
        }

        bool model_has_intersecting_surfaces() const
        {
            const auto intersections = intersecting_triangles<
                OneModelSurfacesIntersection< dimension, Model > >();
            if( intersections.empty() )
            {
                return false;
            }
            return true;
        }

        index_t nb_intersecting_surfaces_elements_pair() const
        {
            const auto intersections = intersecting_triangles<
                AllModelSurfacesIntersection< dimension, Model > >();
            if( verbose_ )
            {
                for( const auto& triangle_pair : intersections )
                {
                    Logger::info( "Triangles ", triangle_pair.first.element_id,
                        " of surface ",
                        triangle_pair.first.component_id.id().string(), " and ",
                        triangle_pair.second.element_id, " of surface ",
                        triangle_pair.first.component_id.id().string(),
                        " intersect each other." );
                }
            }
            return intersections.size();
        }

        std::vector< std::pair< ComponentMeshElement, ComponentMeshElement > >
            intersecting_surfaces_elements() const
        {
            const auto intersections = intersecting_triangles<
                AllModelSurfacesIntersection< dimension, Model > >();
            if( verbose_ )
            {
                for( const auto& triangle_pair : intersections )
                {
                    Logger::info( "Triangles ", triangle_pair.first.element_id,
                        " of surface ",
                        triangle_pair.first.component_id.id().string(), " and ",
                        triangle_pair.second.element_id, " of surface ",
                        triangle_pair.first.component_id.id().string(),
                        " intersect each other." );
                }
            }
            return intersections;
        }

    private:
        template < typename Action >
        std::vector< std::pair< ComponentMeshElement, ComponentMeshElement > >
            intersecting_triangles() const
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
                if( surface.mesh().type_name()
                    != TriangulatedSurface< dimension >::type_name_static() )
                {
                    continue;
                }
                Action surfaces_intersection_action{ model_, surface.id(),
                    surface.id() };
                model_tree
                    .mesh_trees_[model_tree.mesh_tree_ids_.at( surface.id() )]
                    .compute_self_element_bbox_intersections(
                        surfaces_intersection_action );
                for( const auto& triangle_pair :
                    surfaces_intersection_action.intersecting_triangles() )
                {
                    component_intersections.emplace_back(
                        ComponentMeshElement{
                            surface.component_id(), triangle_pair.first },
                        ComponentMeshElement{
                            surface.component_id(), triangle_pair.second } );
                }
            }
            ComponentOverlap surfaces_overlap;
            model_tree.components_tree_.compute_self_element_bbox_intersections(
                surfaces_overlap );
            for( const auto& components : surfaces_overlap.component_pairs )
            {
                const auto surface_uuid1 = model_tree.uuids_[components.first];
                const auto surface_uuid2 = model_tree.uuids_[components.second];
                if( model_.surface( surface_uuid1 ).mesh().type_name()
                        != TriangulatedSurface< dimension >::type_name_static()
                    || model_.surface( surface_uuid2 ).mesh().type_name()
                           != TriangulatedSurface<
                               dimension >::type_name_static() )
                {
                    continue;
                }
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
                for( const auto& triangle_pair :
                    surfaces_intersection_action.intersecting_triangles() )
                {
                    component_intersections.emplace_back(
                        ComponentMeshElement{
                            component_id1, triangle_pair.first },
                        ComponentMeshElement{
                            component_id2, triangle_pair.second } );
                }
            }
            return component_intersections;
        }

    private:
        const Model& model_;
        DEBUG_CONST bool verbose_;
    };

    template < index_t dimension, typename Model >
    ModelMeshesIntersections< dimension, Model >::ModelMeshesIntersections(
        const Model& model )
        : impl_( model, false )
    {
    }

    template < index_t dimension, typename Model >
    ModelMeshesIntersections< dimension, Model >::ModelMeshesIntersections(
        const Model& model, bool verbose )
        : impl_( model, verbose )
    {
    }

    template < index_t dimension, typename Model >
    ModelMeshesIntersections< dimension, Model >::~ModelMeshesIntersections()
    {
    }

    template < index_t dimension, typename Model >
    bool ModelMeshesIntersections< dimension,
        Model >::model_has_intersecting_surfaces() const
    {
        return impl_->model_has_intersecting_surfaces();
    }

    template < index_t dimension, typename Model >
    index_t ModelMeshesIntersections< dimension,
        Model >::nb_intersecting_surfaces_elements_pair() const
    {
        return impl_->nb_intersecting_surfaces_elements_pair();
    }

    template < index_t dimension, typename Model >
    std::vector< std::pair< ComponentMeshElement, ComponentMeshElement > >
        ModelMeshesIntersections< dimension,
            Model >::intersecting_surfaces_elements() const
    {
        return impl_->intersecting_surfaces_elements();
    }

    template class opengeode_inspector_inspector_api
        ModelMeshesIntersections< 2, Section >;
    template class opengeode_inspector_inspector_api
        ModelMeshesIntersections< 3, BRep >;
} // namespace geode
