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
#include <geode/mesh/core/solid_mesh.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/brep.h>

#include <geode/inspector/topology/private/brep_blocks_topology_impl.h>
#include <geode/inspector/topology/private/brep_corners_topology_impl.h>
#include <geode/inspector/topology/private/brep_lines_topology_impl.h>
#include <geode/inspector/topology/private/brep_surfaces_topology_impl.h>

namespace
{
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

    bool brep_has_unique_vertex_associated_to_component(
        const geode::BRep& brep, const geode::uuid& component_id )
    {
        for( const auto unique_vertex_id :
            geode::Range{ brep.nb_unique_vertices() } )
        {
            if( brep.has_component_mesh_vertices(
                    unique_vertex_id, component_id ) )
            {
                return true;
            }
        }
        return false;
    }
} // namespace

namespace geode
{
    class BRepTopologyInspector::Impl : public detail::BRepCornersTopologyImpl,
                                        public detail::BRepLinesTopologyImpl,
                                        public detail::BRepSurfacesTopologyImpl,
                                        public detail::BRepBlocksTopologyImpl
    {
    public:
        Impl( const BRep& brep, bool verbose )
            : detail::BRepCornersTopologyImpl( brep, verbose ),
              detail::BRepLinesTopologyImpl( brep, verbose ),
              detail::BRepSurfacesTopologyImpl( brep, verbose ),
              detail::BRepBlocksTopologyImpl( brep, verbose ),
              brep_( brep ),
              verbose_( verbose )
        {
        }

        bool brep_topology_is_valid() const
        {
            if( brep_.nb_unique_vertices() == 0 )
            {
                return false;
            }
            if( !brep_meshed_components_are_linked_to_a_unique_vertex() )
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
                if( !brep_corner_topology_is_valid( unique_vertex_id )
                    || !brep_vertex_lines_topology_is_valid( unique_vertex_id )
                    || !brep_vertex_surfaces_topology_is_valid(
                        unique_vertex_id )
                    || !brep_vertex_blocks_topology_is_valid(
                        unique_vertex_id ) )
                {
                    return false;
                }
            }
            return true;
        }

        bool brep_meshed_components_are_linked_to_a_unique_vertex() const
        {
            for( const auto& corner : brep_.corners() )
            {
                if( !brep_has_unique_vertex_associated_to_component(
                        brep_, corner.id() ) )
                {
                    return false;
                }
            }
            for( const auto& line : brep_.lines() )
            {
                if( brep_line_is_meshed( brep_, line.id() )
                    && !brep_has_unique_vertex_associated_to_component(
                        brep_, line.id() ) )
                {
                    return false;
                }
            }
            for( const auto& surface : brep_.surfaces() )
            {
                if( brep_surface_is_meshed( brep_, surface.id() )
                    && !brep_has_unique_vertex_associated_to_component(
                        brep_, surface.id() ) )
                {
                    return false;
                }
            }
            for( const auto& block : brep_.blocks() )
            {
                if( brep_block_is_meshed( brep_, block.id() )
                    && !brep_has_unique_vertex_associated_to_component(
                        brep_, block.id() ) )
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

        index_t nb_corners_not_linked_to_a_unique_vertex() const
        {
            index_t counter{ 0 };
            for( const auto& corner : brep_.corners() )
            {
                if( !brep_has_unique_vertex_associated_to_component(
                        brep_, corner.id() ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Corner with uuid '",
                            corner.id().string(),
                            "' is not linked to a unique vertex." );
                    }
                    counter++;
                }
            }
            return counter;
        }

        index_t nb_lines_meshed_but_not_linked_to_a_unique_vertex() const
        {
            index_t counter{ 0 };
            for( const auto& line : brep_.lines() )
            {
                if( brep_line_is_meshed( brep_, line.id() )
                    && !brep_has_unique_vertex_associated_to_component(
                        brep_, line.id() ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Line with uuid '", line.id().string(),
                            "' is not linked to a unique vertex." );
                    }
                    counter++;
                }
            }
            return counter;
        }

        index_t nb_surfaces_meshed_but_not_linked_to_a_unique_vertex() const
        {
            index_t counter{ 0 };
            for( const auto& surface : brep_.surfaces() )
            {
                if( brep_surface_is_meshed( brep_, surface.id() )
                    && !brep_has_unique_vertex_associated_to_component(
                        brep_, surface.id() ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Surface with uuid '",
                            surface.id().string(),
                            "' is not linked to a unique vertex." );
                    }
                    counter++;
                }
            }
            return counter;
        }

        index_t nb_blocks_meshed_but_not_linked_to_a_unique_vertex() const
        {
            index_t counter{ 0 };
            for( const auto& block : brep_.blocks() )
            {
                if( brep_block_is_meshed( brep_, block.id() )
                    && !brep_has_unique_vertex_associated_to_component(
                        brep_, block.id() ) )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Block with uuid '", block.id().string(),
                            "' is not linked to a unique vertex." );
                    }
                    counter++;
                }
            }
            return counter;
        }

        index_t nb_unique_vertices_not_linked_to_a_component_vertex() const
        {
            index_t nb_unlinked{ 0 };
            for( const auto uv_id : Range{ brep_.nb_unique_vertices() } )
            {
                if( brep_.component_mesh_vertices( uv_id ).empty() )
                {
                    nb_unlinked++;
                    if( verbose_ )
                    {
                        Logger::info( "Unique vertex with id ", uv_id,
                            " is not linked to any component mesh vertex." );
                    }
                }
            }
            return nb_unlinked;
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

        std::vector< index_t >
            invalid_components_topology_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( !brep_corner_topology_is_valid( unique_vertex_id )
                    || !brep_vertex_lines_topology_is_valid( unique_vertex_id )
                    || !brep_vertex_surfaces_topology_is_valid(
                        unique_vertex_id )
                    || !brep_vertex_blocks_topology_is_valid(
                        unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t > multiple_corners_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( unique_vertex_has_multiple_corners( unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t > multiple_internals_corner_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( corner_has_multiple_embeddings( unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t > not_internal_nor_boundary_corner_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( corner_is_not_internal_nor_boundary( unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t > line_corners_without_boundary_status() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( corner_is_part_of_line_but_not_boundary(
                        unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t >
            part_of_not_boundary_nor_internal_line_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( vertex_is_part_of_not_boundary_nor_internal_line(
                        unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t >
            part_of_line_with_invalid_internal_topology_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
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
            part_of_invalid_unique_line_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( vertex_is_part_of_invalid_unique_line( unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t >
            part_of_lines_but_not_corner_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( vertex_has_lines_but_is_not_corner( unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t >
            part_of_not_boundary_nor_internal_surface_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( vertex_is_part_of_not_boundary_nor_internal_surface(
                        unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t >
            part_of_surface_with_invalid_internal_topology_unique_vertices()
                const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
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
            part_of_invalid_unique_surface_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( vertex_is_part_of_invalid_unique_surface(
                        unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t >
            part_of_invalid_multiple_surfaces_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( vertex_is_part_of_invalid_multiple_surfaces(
                        unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t >
            part_of_line_and_not_on_surface_border_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( vertex_is_part_of_line_and_not_on_surface_border(
                        unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t > part_of_invalid_blocks_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ brep_.nb_unique_vertices() } )
            {
                if( !brep_vertex_blocks_topology_is_valid( unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

    private:
        const BRep& brep_;
        DEBUG_CONST bool verbose_;
    };

    BRepTopologyInspector::BRepTopologyInspector( const BRep& brep )
        : impl_( brep, false )
    {
    }

    BRepTopologyInspector::BRepTopologyInspector(
        const BRep& brep, bool verbose )
        : impl_( brep, verbose )
    {
    }

    BRepTopologyInspector::~BRepTopologyInspector() {}

    bool BRepTopologyInspector::brep_topology_is_valid() const
    {
        return impl_->brep_topology_is_valid();
    }

    bool BRepTopologyInspector::
        brep_meshed_components_are_linked_to_a_unique_vertex() const
    {
        return impl_->brep_meshed_components_are_linked_to_a_unique_vertex();
    }

    bool BRepTopologyInspector::
        brep_unique_vertices_are_linked_to_a_component_vertex() const
    {
        return impl_->brep_unique_vertices_are_linked_to_a_component_vertex();
    }

    index_t
        BRepTopologyInspector::nb_corners_not_linked_to_a_unique_vertex() const
    {
        return impl_->nb_corners_not_linked_to_a_unique_vertex();
    }

    index_t BRepTopologyInspector::
        nb_lines_meshed_but_not_linked_to_a_unique_vertex() const
    {
        return impl_->nb_lines_meshed_but_not_linked_to_a_unique_vertex();
    }

    index_t BRepTopologyInspector::
        nb_surfaces_meshed_but_not_linked_to_a_unique_vertex() const
    {
        return impl_->nb_surfaces_meshed_but_not_linked_to_a_unique_vertex();
    }

    index_t BRepTopologyInspector::
        nb_blocks_meshed_but_not_linked_to_a_unique_vertex() const
    {
        return impl_->nb_blocks_meshed_but_not_linked_to_a_unique_vertex();
    }

    index_t BRepTopologyInspector::
        nb_unique_vertices_not_linked_to_a_component_vertex() const
    {
        return impl_->nb_unique_vertices_not_linked_to_a_component_vertex();
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
        return impl_->invalid_components_topology_unique_vertices();
    }

    std::vector< index_t >
        BRepTopologyInspector::multiple_corners_unique_vertices() const
    {
        return impl_->multiple_corners_unique_vertices();
    }

    std::vector< index_t >
        BRepTopologyInspector::multiple_internals_corner_vertices() const
    {
        return impl_->multiple_internals_corner_vertices();
    }

    std::vector< index_t >
        BRepTopologyInspector::not_internal_nor_boundary_corner_vertices() const
    {
        return impl_->not_internal_nor_boundary_corner_vertices();
    }

    std::vector< index_t >
        BRepTopologyInspector::line_corners_without_boundary_status() const
    {
        return impl_->line_corners_without_boundary_status();
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_not_boundary_nor_internal_line_unique_vertices() const
    {
        return impl_->part_of_not_boundary_nor_internal_line_unique_vertices();
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_line_with_invalid_internal_topology_unique_vertices() const
    {
        return impl_
            ->part_of_line_with_invalid_internal_topology_unique_vertices();
    }

    std::vector< index_t >
        BRepTopologyInspector::part_of_invalid_unique_line_unique_vertices()
            const
    {
        return impl_->part_of_invalid_unique_line_unique_vertices();
    }

    std::vector< index_t >
        BRepTopologyInspector::part_of_lines_but_not_corner_unique_vertices()
            const
    {
        return impl_->part_of_lines_but_not_corner_unique_vertices();
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_not_boundary_nor_internal_surface_unique_vertices() const
    {
        return impl_
            ->part_of_not_boundary_nor_internal_surface_unique_vertices();
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_surface_with_invalid_internal_topology_unique_vertices() const
    {
        return impl_
            ->part_of_surface_with_invalid_internal_topology_unique_vertices();
    }

    std::vector< index_t >
        BRepTopologyInspector::part_of_invalid_unique_surface_unique_vertices()
            const
    {
        return impl_->part_of_invalid_unique_surface_unique_vertices();
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_invalid_multiple_surfaces_unique_vertices() const
    {
        return impl_->part_of_invalid_multiple_surfaces_unique_vertices();
    }

    std::vector< index_t > BRepTopologyInspector::
        part_of_line_and_not_on_surface_border_unique_vertices() const
    {
        return impl_->part_of_line_and_not_on_surface_border_unique_vertices();
    }

    std::vector< index_t >
        BRepTopologyInspector::part_of_invalid_blocks_unique_vertices() const
    {
        return impl_->part_of_invalid_blocks_unique_vertices();
    }
} // namespace geode
