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
#include <geode/mesh/core/point_set.h>
#include <geode/mesh/core/surface_mesh.h>

#include <geode/model/mixin/core/corner.h>
#include <geode/model/mixin/core/line.h>
#include <geode/model/mixin/core/surface.h>
#include <geode/model/representation/core/section.h>

namespace
{
    bool section_corner_is_meshed(
        const geode::Section& section, const geode::uuid& corner_id )
    {
        return section.corner( corner_id ).mesh().nb_vertices() != 0;
    }

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

    bool section_component_vertices_are_associated_to_unique_vertices(
        const geode::Section& section,
        const geode::ComponentID& component_id,
        const geode::VertexSet& component_mesh )
    {
        for( const auto component_vertex :
            geode::Range{ component_mesh.nb_vertices() } )
        {
            if( section.unique_vertex( { component_id, component_vertex } )
                == geode::NO_ID )
            {
                return false;
            }
        }
        return true;
    }

    std::vector< geode::ComponentMeshVertex >
        section_component_vertices_not_associated_to_unique_vertices(
            const geode::Section& section,
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
            if( section.unique_vertex( mesh_vertex ) == geode::NO_ID )
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
    class SectionTopologyInspector::Impl
    {
    public:
        Impl( const Section& section, bool verbose )
            : section_( section ), verbose_( verbose )
        {
        }

        bool section_meshed_components_are_linked_to_unique_vertices() const
        {
            for( const auto& corner : section_.corners() )
            {
                if( section_corner_is_meshed( section_, corner.id() )
                    && !section_component_vertices_are_associated_to_unique_vertices(
                        section_, corner.component_id(), corner.mesh() ) )
                {
                    return false;
                }
            }
            for( const auto& line : section_.lines() )
            {
                if( section_line_is_meshed( section_, line.id() )
                    && !section_component_vertices_are_associated_to_unique_vertices(
                        section_, line.component_id(), line.mesh() ) )
                {
                    return false;
                }
            }
            for( const auto& surface : section_.surfaces() )
            {
                if( section_surface_is_meshed( section_, surface.id() )
                    && !section_component_vertices_are_associated_to_unique_vertices(
                        section_, surface.component_id(), surface.mesh() ) )
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

        std::vector< ComponentMeshVertex >
            component_vertices_not_linked_to_a_unique_vertex() const
        {
            std::vector< ComponentMeshVertex > result;
            for( const auto& corner : section_.corners() )
            {
                auto corner_result =
                    section_component_vertices_not_associated_to_unique_vertices(
                        section_, corner.component_id(), corner.mesh(),
                        verbose_ );
                result.insert( result.end(),
                    std::make_move_iterator( corner_result.begin() ),
                    std::make_move_iterator( corner_result.end() ) );
            }
            for( const auto& line : section_.lines() )
            {
                auto line_result =
                    section_component_vertices_not_associated_to_unique_vertices(
                        section_, line.component_id(), line.mesh(), verbose_ );
                result.insert( result.end(),
                    std::make_move_iterator( line_result.begin() ),
                    std::make_move_iterator( line_result.end() ) );
            }
            for( const auto& surface : section_.surfaces() )
            {
                auto surface_result =
                    section_component_vertices_not_associated_to_unique_vertices(
                        section_, surface.component_id(), surface.mesh(),
                        verbose_ );
                result.insert( result.end(),
                    std::make_move_iterator( surface_result.begin() ),
                    std::make_move_iterator( surface_result.end() ) );
            }
            return result;
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

    private:
        const Section& section_;
        DEBUG_CONST bool verbose_;
    };

    SectionTopologyInspector::SectionTopologyInspector( const Section& section )
        : SectionCornersTopology( section, false ),
          SectionLinesTopology( section, false ),
          SectionSurfacesTopology( section, false ),
          impl_( section, false ),
          section_( section )
    {
    }

    SectionTopologyInspector::SectionTopologyInspector(
        const Section& section, bool verbose )
        : SectionCornersTopology( section, verbose ),
          SectionLinesTopology( section, verbose ),
          SectionSurfacesTopology( section, verbose ),
          impl_( section, verbose ),
          section_( section )
    {
    }

    SectionTopologyInspector::~SectionTopologyInspector() {}

    bool SectionTopologyInspector::section_topology_is_valid() const
    {
        if( section_.nb_unique_vertices() == 0 )
        {
            return false;
        }
        if( !section_meshed_components_are_linked_to_unique_vertices() )
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
                || !section_vertex_lines_topology_is_valid( unique_vertex_id )
                || !section_vertex_surfaces_topology_is_valid(
                    unique_vertex_id ) )
            {
                return false;
            }
        }
        return true;
    }

    bool SectionTopologyInspector::
        section_meshed_components_are_linked_to_unique_vertices() const
    {
        return impl_->section_meshed_components_are_linked_to_unique_vertices();
    }

    bool SectionTopologyInspector::
        section_unique_vertices_are_linked_to_a_component_vertex() const
    {
        return impl_
            ->section_unique_vertices_are_linked_to_a_component_vertex();
    }

    std::vector< ComponentMeshVertex > SectionTopologyInspector::
        component_vertices_not_linked_to_a_unique_vertex() const
    {
        return impl_->component_vertices_not_linked_to_a_unique_vertex();
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
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id :
            Range{ section_.nb_unique_vertices() } )
        {
            if( !section_corner_topology_is_valid( unique_vertex_id )
                || !section_vertex_lines_topology_is_valid( unique_vertex_id )
                || !section_vertex_surfaces_topology_is_valid(
                    unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t >
        SectionTopologyInspector::multiple_corners_unique_vertices() const
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

    std::vector< index_t >
        SectionTopologyInspector::multiple_internals_corner_vertices() const
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

    std::vector< index_t >
        SectionTopologyInspector::not_internal_nor_boundary_corner_vertices()
            const
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

    std::vector< index_t >
        SectionTopologyInspector::line_corners_without_boundary_status() const
    {
        std::vector< index_t > invalid_unique_vertices;
        for( const auto unique_vertex_id :
            Range{ section_.nb_unique_vertices() } )
        {
            if( corner_is_part_of_line_but_not_boundary( unique_vertex_id ) )
            {
                invalid_unique_vertices.push_back( unique_vertex_id );
            }
        }
        return invalid_unique_vertices;
    }

    std::vector< index_t > SectionTopologyInspector::
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

    std::vector< index_t > SectionTopologyInspector::
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
        SectionTopologyInspector::part_of_invalid_unique_line_unique_vertices()
            const
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
        SectionTopologyInspector::part_of_lines_but_not_corner_unique_vertices()
            const
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

    std::vector< index_t >
        SectionTopologyInspector::part_of_invalid_surfaces_unique_vertices()
            const
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

    std::vector< index_t > SectionTopologyInspector::
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

} // namespace geode
