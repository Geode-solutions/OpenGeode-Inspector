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

#include <geode/inspector/topology/section_topology.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/section.hpp>

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
} // namespace

namespace geode
{
    index_t SectionTopologyInspectionResult::nb_issues() const
    {
        return corners.nb_issues() + lines.nb_issues() + surfaces.nb_issues()
               + unique_vertices_not_linked_to_any_component.nb_issues();
    }

    std::string SectionTopologyInspectionResult::string() const
    {
        return absl::StrCat( corners.string(), lines.string(),
            surfaces.string(),
            unique_vertices_not_linked_to_any_component.string() );
    }

    std::string SectionTopologyInspectionResult::inspection_type() const
    {
        return "Model topology inspection";
    }

    class SectionTopologyInspector::Impl
    {
    public:
        Impl( const Section& section ) : section_( section ) {}

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

        void add_unique_vertices_not_linked_to_a_component_vertex(
            SectionTopologyInspectionResult& section_issues ) const
        {
            auto& inspect_result =
                section_issues.unique_vertices_not_linked_to_any_component;
            for( const auto uv_id : Range{ section_.nb_unique_vertices() } )
            {
                if( section_.component_mesh_vertices( uv_id ).empty() )
                {
                    inspect_result.add_issue( uv_id,
                        absl::StrCat( "Unique vertex with id ", uv_id,
                            " is not linked to any component mesh vertex." ) );
                }
            }
        }

        bool section_topology_is_valid(
            const SectionTopologyInspector& section_topology_inspector ) const
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
                if( !section_topology_inspector
                         .section_corner_topology_is_valid( unique_vertex_id )
                    || !section_topology_inspector
                            .section_lines_topology_is_valid( unique_vertex_id )
                    || !section_topology_inspector
                            .section_vertex_surfaces_topology_is_valid(
                                unique_vertex_id ) )
                {
                    return false;
                }
            }
            return true;
        }

        SectionTopologyInspectionResult inspect_section_topology(
            const SectionTopologyInspector& section_topology_inspector ) const
        {
            SectionTopologyInspectionResult result;
            result.corners =
                section_topology_inspector.inspect_corners_topology();
            result.lines = section_topology_inspector.inspect_lines_topology();
            result.surfaces = section_topology_inspector.inspect_surfaces();
            add_unique_vertices_not_linked_to_a_component_vertex( result );
            return result;
        }

    private:
        const Section& section_;
    };

    SectionTopologyInspector::SectionTopologyInspector( const Section& section )
        : SectionCornersTopology( section ),
          SectionLinesTopology( section ),
          SectionSurfacesTopology( section ),
          impl_( section )
    {
    }

    SectionTopologyInspector::~SectionTopologyInspector() = default;

    bool SectionTopologyInspector::section_topology_is_valid() const
    {
        return impl_->section_topology_is_valid( *this );
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

    SectionTopologyInspectionResult
        SectionTopologyInspector::inspect_section_topology() const
    {
        return impl_->inspect_section_topology( *this );
    }
} // namespace geode
