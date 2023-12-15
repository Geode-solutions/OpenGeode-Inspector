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

#include <geode/inspector/topology/brep_topology.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

namespace
{
    bool brep_corner_is_meshed(
        const geode::BRep& brep, const geode::uuid& corner_id )
    {
        return brep.corner( corner_id ).mesh().nb_vertices() != 0;
    }

    bool brep_line_is_meshed(
        const geode::BRep& brep, const geode::uuid& line_id )
    {
        return brep.line( line_id ).mesh().nb_vertices() != 0;
    }

    bool brep_surface_is_meshed(
        const geode::BRep& brep, const geode::uuid& surface_id )
    {
        return brep.surface( surface_id ).mesh().nb_vertices() != 0;
    }

    bool brep_block_is_meshed(
        const geode::BRep& brep, const geode::uuid& block_id )
    {
        return brep.block( block_id ).mesh().nb_vertices() != 0;
    }

    bool brep_component_vertices_are_associated_to_unique_vertices(
        const geode::BRep& brep,
        const geode::ComponentID& component_id,
        const geode::VertexSet& component_mesh )
    {
        for( const auto component_vertex :
            geode::Range{ component_mesh.nb_vertices() } )
        {
            if( brep.unique_vertex( { component_id, component_vertex } )
                == geode::NO_ID )
            {
                return false;
            }
        }
        return true;
    }

} // namespace

namespace geode
{
    class BRepTopologyInspector::Impl
    {
    public:
        Impl( const BRep& brep ) : brep_( brep ) {}

        bool brep_meshed_components_are_linked_to_unique_vertices() const
        {
            for( const auto& corner : brep_.corners() )
            {
                if( brep_corner_is_meshed( brep_, corner.id() )
                    && !brep_component_vertices_are_associated_to_unique_vertices(
                        brep_, corner.component_id(), corner.mesh() ) )
                {
                    return false;
                }
            }
            for( const auto& line : brep_.lines() )
            {
                if( brep_line_is_meshed( brep_, line.id() )
                    && !brep_component_vertices_are_associated_to_unique_vertices(
                        brep_, line.component_id(), line.mesh() ) )
                {
                    return false;
                }
            }
            for( const auto& surface : brep_.surfaces() )
            {
                if( brep_surface_is_meshed( brep_, surface.id() )
                    && !brep_component_vertices_are_associated_to_unique_vertices(
                        brep_, surface.component_id(), surface.mesh() ) )
                {
                    return false;
                }
            }
            for( const auto& block : brep_.blocks() )
            {
                if( brep_block_is_meshed( brep_, block.id() )
                    && !brep_component_vertices_are_associated_to_unique_vertices(
                        brep_, block.component_id(), block.mesh() ) )
                {
                    return false;
                }
            }
            return true;
        }

        bool brep_unique_vertices_are_linked_to_a_component_vertex() const
        {
            for( const auto uv_id : Range{ brep_.nb_unique_vertices() } )
            {
                if( brep_.component_mesh_vertices( uv_id ).empty() )
                {
                    return false;
                }
            }
            return true;
        }

        std::pair< std::vector< index_t >, std::vector< std::string > >
            unique_vertices_not_linked_to_a_component_vertex() const
        {
            std::pair< std::vector< index_t >, std::vector< std::string > >
                result;
            for( const auto uv_id : Range{ brep_.nb_unique_vertices() } )
            {
                if( brep_.component_mesh_vertices( uv_id ).empty() )
                {
                    result.first.push_back( uv_id );
                    result.second.push_back(
                        absl::StrCat( "Unique vertex with id ", uv_id,
                            " is not linked to any component mesh vertex." ) );
                }
            }
            return result;
        }

        bool brep_topology_is_valid(
            const BRepTopologyInspector& brep_topology_inspector ) const
        {
            if( brep_.nb_unique_vertices() == 0 )
            {
                return false;
            }
            if( !brep_meshed_components_are_linked_to_unique_vertices() )
            {
                return false;
            }
            if( !brep_unique_vertices_are_linked_to_a_component_vertex() )
            {
                return false;
            }
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( !brep_topology_inspector.brep_corner_topology_is_valid(
                        unique_vertex_id )
                    || !brep_topology_inspector.brep_lines_topology_is_valid(
                        unique_vertex_id )
                    || !brep_topology_inspector.brep_surfaces_topology_is_valid(
                        unique_vertex_id )
                    || !brep_topology_inspector.brep_blocks_topology_is_valid(
                        unique_vertex_id ) )
                {
                    return false;
                }
            }
            return true;
        }

        BRepTopologyInspectionResult inspect_brep_topology(
            const BRepTopologyInspector& brep_topology_inspector ) const
        {
            BRepTopologyInspectionResult result;
            result.corners = brep_topology_inspector.inspect_corners_topology();
            result.lines = brep_topology_inspector.inspect_lines_topology();
            result.surfaces =
                brep_topology_inspector.inspect_surfaces_topology();
            result.blocks = brep_topology_inspector.inspect_blocks();
            const auto res = unique_vertices_not_linked_to_a_component_vertex();
            result.unique_vertices_not_linked_to_any_component.problems =
                std::move( res.first );
            result.unique_vertices_not_linked_to_any_component.messages =
                std::move( res.second );
            return result;
        }

    private:
        const BRep& brep_;
    };

    BRepTopologyInspector::BRepTopologyInspector( const BRep& brep )
        : BRepCornersTopology( brep ),
          BRepLinesTopology( brep ),
          BRepSurfacesTopology( brep ),
          BRepBlocksTopology( brep ),
          impl_( brep )
    {
    }

    BRepTopologyInspector::~BRepTopologyInspector() {}

    bool BRepTopologyInspector::brep_topology_is_valid() const
    {
        return impl_->brep_topology_is_valid( *this );
    }

    bool BRepTopologyInspector::
        brep_meshed_components_are_linked_to_unique_vertices() const
    {
        return impl_->brep_meshed_components_are_linked_to_unique_vertices();
    }

    bool BRepTopologyInspector::
        brep_unique_vertices_are_linked_to_a_component_vertex() const
    {
        return impl_->brep_unique_vertices_are_linked_to_a_component_vertex();
    }

    BRepTopologyInspectionResult
        BRepTopologyInspector::inspect_brep_topology() const
    {
        return impl_->inspect_brep_topology( *this );
    }
} // namespace geode
