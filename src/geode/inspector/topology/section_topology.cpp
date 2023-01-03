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

#include <geode/inspector/topology/section_topology.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/mesh/core/edged_curve.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/section.h>

#include <geode/inspector/topology/private/section_corners_topology_impl.h>
#include <geode/inspector/topology/private/section_lines_topology_impl.h>
#include <geode/inspector/topology/private/section_surfaces_topology_impl.h>

namespace
{
    bool section_line_is_meshed(
        const geode::Section& section, const geode::uuid& line_id )
    {
        return section.line( line_id ).mesh().nb_vertices() != 0;
    }

    bool section_surface_is_meshed(
        const geode::Section& section, const geode::uuid& surface_id )
    {
        return section.surface( surface_id ).mesh().nb_vertices() != 0;
    }

    bool section_has_unique_vertex_associated_to_component(
        const geode::Section& section, const geode::uuid& component_id )
    {
        for( const auto unique_vertex_id :
            geode::Range{ section.nb_unique_vertices() } )
        {
            if( section.has_component_mesh_vertices(
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
    class SectionTopologyInspector::Impl
        : public detail::SectionCornersTopologyImpl,
          public detail::SectionLinesTopologyImpl,
          public detail::SectionSurfacesTopologyImpl
    {
    public:
        Impl( const Section& section, bool verbose )
            : detail::SectionCornersTopologyImpl( section, verbose ),
              detail::SectionLinesTopologyImpl( section, verbose ),
              detail::SectionSurfacesTopologyImpl( section, verbose ),
              section_( section ),
              verbose_( verbose )
        {
        }

        bool section_topology_is_valid() const
        {
            if( section_.nb_unique_vertices() == 0 )
            {
                return false;
            }
            if( !section_meshed_components_are_linked_to_a_unique_vertex() )
            {
                return false;
            }
            if( !section_unique_vertices_are_linked_to_a_component_vertex() )
            {
                return false;
            }
            for( const auto unique_vertex_id :
                Range{ section_.nb_unique_vertices() } )
            {
                if( !section_corner_topology_is_valid( unique_vertex_id )
                    || !section_vertex_lines_topology_is_valid(
                        unique_vertex_id )
                    || !section_vertex_surfaces_topology_is_valid(
                        unique_vertex_id ) )
                {
                    return false;
                }
            }
            return true;
        }

        bool section_meshed_components_are_linked_to_a_unique_vertex() const
        {
            for( const auto& corner : section_.corners() )
            {
                if( !section_has_unique_vertex_associated_to_component(
                        section_, corner.id() ) )
                {
                    return false;
                }
            }
            for( const auto& line : section_.lines() )
            {
                if( section_line_is_meshed( section_, line.id() )
                    && !section_has_unique_vertex_associated_to_component(
                        section_, line.id() ) )
                {
                    return false;
                }
            }
            for( const auto& surface : section_.surfaces() )
            {
                if( section_surface_is_meshed( section_, surface.id() )
                    && !section_has_unique_vertex_associated_to_component(
                        section_, surface.id() ) )
                {
                    return false;
                }
            }
            return true;
        }

        bool section_unique_vertices_are_linked_to_a_component_vertex() const
        {
            for( const auto uv_id : Range{ section_.nb_unique_vertices() } )
            {
                if( section_.component_mesh_vertices( uv_id ).empty() )
                {
                    return false;
                }
            }
            return true;
        }

        index_t nb_corners_not_linked_to_a_unique_vertex() const
        {
            index_t counter{ 0 };
            for( const auto& corner : section_.corners() )
            {
                if( !section_has_unique_vertex_associated_to_component(
                        section_, corner.id() ) )
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
            for( const auto& line : section_.lines() )
            {
                if( section_line_is_meshed( section_, line.id() )
                    && !section_has_unique_vertex_associated_to_component(
                        section_, line.id() ) )
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
            for( const auto& surface : section_.surfaces() )
            {
                if( section_surface_is_meshed( section_, surface.id() )
                    && !section_has_unique_vertex_associated_to_component(
                        section_, surface.id() ) )
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

        index_t nb_unique_vertices_not_linked_to_a_component_vertex() const
        {
            index_t nb_unlinked{ 0 };
            for( const auto uv_id : Range{ section_.nb_unique_vertices() } )
            {
                if( section_.component_mesh_vertices( uv_id ).empty() )
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
            for( const auto uv_id : Range{ section_.nb_unique_vertices() } )
            {
                if( section_.component_mesh_vertices( uv_id ).empty() )
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
                Range{ section_.nb_unique_vertices() } )
            {
                if( !section_corner_topology_is_valid( unique_vertex_id )
                    || !section_vertex_lines_topology_is_valid(
                        unique_vertex_id )
                    || !section_vertex_surfaces_topology_is_valid(
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
                Range{ section_.nb_unique_vertices() } )
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
                Range{ section_.nb_unique_vertices() } )
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
                Range{ section_.nb_unique_vertices() } )
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
                Range{ section_.nb_unique_vertices() } )
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
                Range{ section_.nb_unique_vertices() } )
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
                Range{ section_.nb_unique_vertices() } )
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
                Range{ section_.nb_unique_vertices() } )
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
                Range{ section_.nb_unique_vertices() } )
            {
                if( vertex_has_lines_but_is_not_corner( unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

        std::vector< index_t > part_of_invalid_surfaces_unique_vertices() const
        {
            std::vector< index_t > invalid_unique_vertices;
            for( const auto unique_vertex_id :
                Range{ section_.nb_unique_vertices() } )
            {
                if( vertex_is_part_of_invalid_surfaces_topology(
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
                Range{ section_.nb_unique_vertices() } )
            {
                if( vertex_is_part_of_line_and_not_on_surface_border(
                        unique_vertex_id ) )
                {
                    invalid_unique_vertices.push_back( unique_vertex_id );
                }
            }
            return invalid_unique_vertices;
        }

    private:
        const Section& section_;
        DEBUG_CONST bool verbose_;
    };

    SectionTopologyInspector::SectionTopologyInspector( const Section& section )
        : impl_( section, false )
    {
    }

    SectionTopologyInspector::SectionTopologyInspector(
        const Section& section, bool verbose )
        : impl_( section, verbose )
    {
    }

    SectionTopologyInspector::~SectionTopologyInspector() {}

    bool SectionTopologyInspector::section_topology_is_valid() const
    {
        return impl_->section_topology_is_valid();
    }

    bool SectionTopologyInspector::
        section_meshed_components_are_linked_to_a_unique_vertex() const
    {
        return impl_->section_meshed_components_are_linked_to_a_unique_vertex();
    }

    bool SectionTopologyInspector::
        section_unique_vertices_are_linked_to_a_component_vertex() const
    {
        return impl_
            ->section_unique_vertices_are_linked_to_a_component_vertex();
    }

    index_t SectionTopologyInspector::nb_corners_not_linked_to_a_unique_vertex()
        const
    {
        return impl_->nb_corners_not_linked_to_a_unique_vertex();
    }

    index_t SectionTopologyInspector::
        nb_lines_meshed_but_not_linked_to_a_unique_vertex() const
    {
        return impl_->nb_lines_meshed_but_not_linked_to_a_unique_vertex();
    }

    index_t SectionTopologyInspector::
        nb_surfaces_meshed_but_not_linked_to_a_unique_vertex() const
    {
        return impl_->nb_surfaces_meshed_but_not_linked_to_a_unique_vertex();
    }

    index_t SectionTopologyInspector::
        nb_unique_vertices_not_linked_to_a_component_vertex() const
    {
        return impl_->nb_unique_vertices_not_linked_to_a_component_vertex();
    }

    std::vector< index_t > SectionTopologyInspector::
        unique_vertices_not_linked_to_a_component_vertex() const
    {
        return impl_->unique_vertices_not_linked_to_a_component_vertex();
    }

    std::vector< index_t >
        SectionTopologyInspector::invalid_components_topology_unique_vertices()
            const
    {
        return impl_->invalid_components_topology_unique_vertices();
    }

    std::vector< index_t >
        SectionTopologyInspector::multiple_corners_unique_vertices() const
    {
        return impl_->multiple_corners_unique_vertices();
    }

    std::vector< index_t >
        SectionTopologyInspector::multiple_internals_corner_vertices() const
    {
        return impl_->multiple_internals_corner_vertices();
    }

    std::vector< index_t >
        SectionTopologyInspector::not_internal_nor_boundary_corner_vertices()
            const
    {
        return impl_->not_internal_nor_boundary_corner_vertices();
    }

    std::vector< index_t >
        SectionTopologyInspector::line_corners_without_boundary_status() const
    {
        return impl_->line_corners_without_boundary_status();
    }

    std::vector< index_t > SectionTopologyInspector::
        part_of_not_boundary_nor_internal_line_unique_vertices() const
    {
        return impl_->part_of_not_boundary_nor_internal_line_unique_vertices();
    }

    std::vector< index_t > SectionTopologyInspector::
        part_of_line_with_invalid_internal_topology_unique_vertices() const
    {
        return impl_
            ->part_of_line_with_invalid_internal_topology_unique_vertices();
    }

    std::vector< index_t >
        SectionTopologyInspector::part_of_invalid_unique_line_unique_vertices()
            const
    {
        return impl_->part_of_invalid_unique_line_unique_vertices();
    }

    std::vector< index_t >
        SectionTopologyInspector::part_of_lines_but_not_corner_unique_vertices()
            const
    {
        return impl_->part_of_lines_but_not_corner_unique_vertices();
    }

    std::vector< index_t >
        SectionTopologyInspector::part_of_invalid_surfaces_unique_vertices()
            const
    {
        return impl_->part_of_invalid_surfaces_unique_vertices();
    }

    std::vector< index_t > SectionTopologyInspector::
        part_of_line_and_not_on_surface_border_unique_vertices() const
    {
        return impl_->part_of_line_and_not_on_surface_border_unique_vertices();
    }
} // namespace geode
