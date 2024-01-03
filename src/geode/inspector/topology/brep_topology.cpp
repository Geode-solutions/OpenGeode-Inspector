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

    std::vector< geode::ComponentMeshVertex >
        brep_component_vertices_not_associated_to_unique_vertices(
            const geode::BRep& brep,
            const geode::ComponentID& component_id,
            const geode::VertexSet& component_mesh,
            bool verbose )
    {
        std::vector< geode::ComponentMeshVertex > result;
        for( const auto component_vertex :
            geode::Range{ component_mesh.nb_vertices() } )
        {
            geode::ComponentMeshVertex mesh_vertex{ component_id,
                component_vertex };
            if( brep.unique_vertex( mesh_vertex ) == geode::NO_ID )
            {
                if( verbose )
                {
                    geode::Logger::info( "Component vertex '",
                        mesh_vertex.string(),
                        "' is not linked to a unique vertex." );
                }
                result.push_back( std::move( mesh_vertex ) );
            }
        }
        return result;
    }
} // namespace

namespace geode
{
    class BRepTopologyInspector::Impl
    {
    public:
        Impl( const BRep& brep, bool verbose )
            : brep_( brep ), verbose_( verbose )
        {
        }

        bool brep_meshed_components_are_linked_to_unique_vertices() const
        {
            for( const auto& corner : brep_.corners() )
            {
                if( !brep_component_vertices_are_associated_to_unique_vertices(
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

        std::vector< ComponentMeshVertex >
            component_vertices_not_linked_to_a_unique_vertex() const
        {
            std::vector< ComponentMeshVertex > result;
            for( const auto& corner : brep_.corners() )
            {
                auto corner_result =
                    brep_component_vertices_not_associated_to_unique_vertices(
                        brep_, corner.component_id(), corner.mesh(), verbose_ );
                result.insert( result.end(),
                    std::make_move_iterator( corner_result.begin() ),
                    std::make_move_iterator( corner_result.end() ) );
            }
            for( const auto& line : brep_.lines() )
            {
                auto line_result =
                    brep_component_vertices_not_associated_to_unique_vertices(
                        brep_, line.component_id(), line.mesh(), verbose_ );
                result.insert( result.end(),
                    std::make_move_iterator( line_result.begin() ),
                    std::make_move_iterator( line_result.end() ) );
            }
            for( const auto& surface : brep_.surfaces() )
            {
                auto surface_result =
                    brep_component_vertices_not_associated_to_unique_vertices(
                        brep_, surface.component_id(), surface.mesh(),
                        verbose_ );
                result.insert( result.end(),
                    std::make_move_iterator( surface_result.begin() ),
                    std::make_move_iterator( surface_result.end() ) );
            }
            for( const auto& block : brep_.blocks() )
            {
                auto block_result =
                    brep_component_vertices_not_associated_to_unique_vertices(
                        brep_, block.component_id(), block.mesh(), verbose_ );
                result.insert( result.end(),
                    std::make_move_iterator( block_result.begin() ),
                    std::make_move_iterator( block_result.end() ) );
            }
            return result;
        }

        std::vector< index_t >
            unique_vertices_not_linked_to_a_component_vertex() const
        {
            std::vector< index_t > unlinked_uv;
            for( const auto uv_id : Range{ brep_.nb_unique_vertices() } )
            {
                if( brep_.component_mesh_vertices( uv_id ).empty() )
                {
                    unlinked_uv.push_back( uv_id );
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with id ", uv_id,
                            " is not linked to any component mesh vertex." );
                    }
                }
            }
            return unlinked_uv;
        }

    private:
        const BRep& brep_;
        DEBUG_CONST bool verbose_;
    };

    BRepTopologyInspector::BRepTopologyInspector( const BRep& brep )
        : BRepCornersTopology( brep, false ),
          BRepLinesTopology( brep, false ),
          BRepSurfacesTopology( brep, false ),
          BRepBlocksTopology( brep, false ),
          impl_( brep, false ),
          brep_( brep )
    {
    }

    BRepTopologyInspector::BRepTopologyInspector(
        const BRep& brep, bool verbose )
        : BRepCornersTopology( brep, verbose ),
          BRepLinesTopology( brep, verbose ),
          BRepSurfacesTopology( brep, verbose ),
          BRepBlocksTopology( brep, verbose ),
          impl_( brep, verbose ),
          brep_( brep )
    {
    }

    BRepTopologyInspector::~BRepTopologyInspector() {}

    bool BRepTopologyInspector::brep_topology_is_valid() const
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
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( !brep_corner_topology_is_valid( unique_vertex_id )
                || !brep_vertex_lines_topology_is_valid( unique_vertex_id )
                || !brep_vertex_surfaces_topology_is_valid( unique_vertex_id )
                || !brep_vertex_blocks_topology_is_valid( unique_vertex_id ) )
            {
                return false;
            }
        }
        return true;
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

    std::vector< ComponentMeshVertex > BRepTopologyInspector::
        component_vertices_not_linked_to_a_unique_vertex() const
    {
        return impl_->component_vertices_not_linked_to_a_unique_vertex();
    }

    std::vector< index_t > BRepTopologyInspector::
        unique_vertices_not_linked_to_a_component_vertex() const
    {
        return impl_->unique_vertices_not_linked_to_a_component_vertex();
    }

    std::vector< index_t >
        BRepTopologyInspector::invalid_components_topology_unique_vertices()
            const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( !brep_corner_topology_is_valid( unique_vertex_id )
                || !brep_vertex_lines_topology_is_valid( unique_vertex_id )
                || !brep_vertex_surfaces_topology_is_valid( unique_vertex_id )
                || !brep_vertex_blocks_topology_is_valid( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t >
        BRepTopologyInspector::multiple_corners_unique_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( unique_vertex_has_multiple_corners( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t >
        BRepTopologyInspector::multiple_internals_corner_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( corner_has_multiple_embeddings( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t >
        BRepTopologyInspector::not_internal_nor_boundary_corner_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( corner_is_not_internal_nor_boundary( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t >
        BRepTopologyInspector::line_corners_without_boundary_status() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( corner_is_part_of_line_but_not_boundary( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_not_boundary_nor_internal_line_unique_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( vertex_is_part_of_not_boundary_nor_internal_line(
                    unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_line_with_invalid_internal_topology_unique_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( vertex_is_part_of_line_with_invalid_internal_topology(
                    unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t >
        BRepTopologyInspector::part_of_invalid_unique_line_unique_vertices()
            const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( vertex_is_part_of_invalid_unique_line( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t >
        BRepTopologyInspector::part_of_lines_but_not_corner_unique_vertices()
            const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( vertex_has_lines_but_is_not_corner( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_not_boundary_nor_internal_surface_unique_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( vertex_is_part_of_not_boundary_nor_internal_surface(
                    unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_surface_with_invalid_internal_topology_unique_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( vertex_is_part_of_surface_with_invalid_internal_topology(
                    unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t >
        BRepTopologyInspector::part_of_invalid_unique_surface_unique_vertices()
            const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( vertex_is_part_of_invalid_unique_surface( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_invalid_multiple_surfaces_unique_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( vertex_is_part_of_invalid_multiple_surfaces(
                    unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_line_and_not_on_surface_border_unique_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( vertex_is_part_of_line_and_not_on_surface_border(
                    unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t >
        BRepTopologyInspector::part_of_invalid_blocks_unique_vertices() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id : Range{ brep_.nb_unique_vertices() } )
        {
            if( !brep_vertex_blocks_topology_is_valid( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }
} // namespace geode
