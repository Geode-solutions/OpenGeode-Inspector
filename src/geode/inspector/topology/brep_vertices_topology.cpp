/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/inspector/topology/brep_vertices_topology.h>

#include <geode/basic/pimpl_impl.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/relationships.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    class BRepVerticesTopology::Impl
    {
    public:
        Impl( const BRep& brep ) : brep_( brep ) {}

        bool brep_vertices_topology_is_valid() const
        {
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                const auto corners = brep_.mesh_component_vertices(
                    unique_vertex_id, Corner3D::component_type_static() );
                if( corners.empty() )
                {
                    continue;
                }
                if( corners.size() != 1 )
                {
                    return false;
                }
                const auto& corner_uuid = corners[0].component_id.id();
                if( brep_.nb_embeddings( corner_uuid ) > 1 )
                {
                    return false;
                }
                for( const auto& line : brep_.mesh_component_vertices(
                         unique_vertex_id, Line3D::component_type_static() ) )
                {
                    if( !brep_.Relationships::is_boundary(
                            corner_uuid, line.component_id.id() ) )
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        std::vector< index_t > multiple_corners_unique_vertices() const
        {
            std::vector< index_t > multiple_corners_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( brep_
                        .mesh_component_vertices( unique_vertex_id,
                            Corner3D::component_type_static() )
                        .size()
                    > 1 )
                {
                    multiple_corners_unique_vertices.push_back(
                        unique_vertex_id );
                }
            }
            return multiple_corners_unique_vertices;
        }

        std::vector< index_t > multiple_internals_corner_vertices() const
        {
            std::vector< index_t > multiple_internals_corners;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                const auto corners = brep_.mesh_component_vertices(
                    unique_vertex_id, Corner3D::component_type_static() );
                if( !corners.empty()
                    && brep_.nb_embeddings( corners[0].component_id.id() ) > 1 )
                {
                    multiple_internals_corners.push_back( unique_vertex_id );
                }
            }
            return multiple_internals_corners;
        }

        std::vector< index_t > line_corners_without_boundary_status() const
        {
            std::vector< index_t > line_corners_without_boundary_status;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                const auto corners = brep_.mesh_component_vertices(
                    unique_vertex_id, Corner3D::component_type_static() );
                if( !corners.empty() )
                {
                    const auto& corner_uuid = corners[0].component_id.id();
                    for( const auto& line :
                        brep_.mesh_component_vertices( unique_vertex_id,
                            Line3D::component_type_static() ) )
                    {
                        if( !brep_.Relationships::is_boundary(
                                corner_uuid, line.component_id.id() ) )
                        {
                            line_corners_without_boundary_status.push_back(
                                unique_vertex_id );
                        }
                    }
                }
            }
            return line_corners_without_boundary_status;
        }

    private:
        const BRep& brep_;
    };

    BRepVerticesTopology::BRepVerticesTopology( const BRep& brep )
        : impl_( brep )
    {
    }

    BRepVerticesTopology::~BRepVerticesTopology() {}

    bool BRepVerticesTopology::brep_vertices_topology_is_valid() const
    {
        return impl_->brep_vertices_topology_is_valid();
    }

    std::vector< index_t >
        BRepVerticesTopology::multiple_corners_unique_vertices() const
    {
        return impl_->multiple_corners_unique_vertices();
    }

    std::vector< index_t >
        BRepVerticesTopology::multiple_internals_corner_vertices() const
    {
        return impl_->multiple_internals_corner_vertices();
    }

    std::vector< index_t >
        BRepVerticesTopology::line_corners_without_boundary_status() const
    {
        return impl_->line_corners_without_boundary_status();
    }
} // namespace geode
