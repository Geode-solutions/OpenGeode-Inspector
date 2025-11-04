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

namespace geode
{
    index_t SectionTopologyInspectionResult::nb_issues() const
    {
        return corners.nb_issues() + lines.nb_issues() + surfaces.nb_issues()
               + unique_vertices_not_linked_to_any_component.nb_issues()
               + unique_vertices_linked_to_inexistant_cmv.nb_issues()
               + unique_vertices_nonbijectively_linked_to_cmv.nb_issues();
    }

    std::string SectionTopologyInspectionResult::string() const
    {
        return absl::StrCat( corners.string(), lines.string(),
            surfaces.string(),
            unique_vertices_not_linked_to_any_component.string(),
            unique_vertices_linked_to_inexistant_cmv.string(),
            unique_vertices_nonbijectively_linked_to_cmv.string() );
    }

    std::string SectionTopologyInspectionResult::inspection_type() const
    {
        return "Model topology inspection";
    }

    class SectionTopologyInspector::Impl
    {
    public:
        Impl( const Section& section ) : section_( section ) {}

        bool
            section_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex()
                const
        {
            for( const auto uv_id : Range{ section_.nb_unique_vertices() } )
            {
                const auto& unique_vertex_cmvs =
                    section_.component_mesh_vertices( uv_id );
                if( unique_vertex_cmvs.empty() )
                {
                    return false;
                }
                for( const auto& cmv : unique_vertex_cmvs )
                {
                    if( cmv_exists_in_section( cmv ) )
                    {
                        if( section_.unique_vertex( cmv ) != uv_id )
                        {
                            return false;
                        }
                        continue;
                    }
                    return false;
                }
            }
            return true;
        }

        void add_unique_vertices_with_wrong_cmv_link(
            SectionTopologyInspectionResult& section_issues ) const
        {
            auto& not_linked_result =
                section_issues.unique_vertices_not_linked_to_any_component;
            auto& linked_to_inexistant_result =
                section_issues.unique_vertices_linked_to_inexistant_cmv;
            auto& linked_to_nonbijective_result =
                section_issues.unique_vertices_nonbijectively_linked_to_cmv;
            for( const auto uv_id : Range{ section_.nb_unique_vertices() } )
            {
                const auto& unique_vertex_cmvs =
                    section_.component_mesh_vertices( uv_id );
                if( unique_vertex_cmvs.empty() )
                {
                    not_linked_result.add_issue(
                        uv_id, absl::StrCat( "unique vertex ", uv_id,
                                   " is not linked to any mesh vertex." ) );
                    continue;
                }
                for( const auto& cmv : unique_vertex_cmvs )
                {
                    if( cmv_exists_in_section( cmv ) )
                    {
                        if( section_.unique_vertex( cmv ) != uv_id )
                        {
                            linked_to_nonbijective_result.add_issue( uv_id,
                                absl::StrCat( "unique vertex ", uv_id,
                                    " is linked to inexistant mesh vertex [",
                                    cmv.string(), "]." ) );
                        }
                        continue;
                    }
                    linked_to_inexistant_result.add_issue(
                        uv_id, absl::StrCat( "unique vertex ", uv_id,
                                   " is linked to inexistant mesh vertex [",
                                   cmv.string(), "]." ) );
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
            if( !section_meshed_components_are_linked_to_unique_vertices(
                    section_topology_inspector ) )
            {
                return false;
            }
            if( !section_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex() )
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
            add_unique_vertices_with_wrong_cmv_link( result );
            return result;
        }

    private:
        bool cmv_exists_in_section( const ComponentMeshVertex& cmv ) const
        {
            if( cmv.component_id.type() == Corner2D::component_type_static() )
            {
                if( section_.has_corner( cmv.component_id.id() )
                    && cmv.vertex < section_.corner( cmv.component_id.id() )
                               .mesh()
                               .nb_vertices() )
                {
                    return true;
                }
            }
            if( cmv.component_id.type() == Line2D::component_type_static() )
            {
                if( section_.has_line( cmv.component_id.id() )
                    && cmv.vertex < section_.line( cmv.component_id.id() )
                               .mesh()
                               .nb_vertices() )
                {
                    return true;
                }
            }
            if( cmv.component_id.type() == Surface2D::component_type_static() )
            {
                if( section_.has_surface( cmv.component_id.id() )
                    && cmv.vertex < section_.surface( cmv.component_id.id() )
                               .mesh()
                               .nb_vertices() )
                {
                    return true;
                }
            }
            return false;
        }

        bool section_meshed_components_are_linked_to_unique_vertices(
            const SectionTopologyInspector& topology_inspector ) const
        {
            for( const auto& corner : section_.corners() )
            {
                if( topology_inspector.corner_is_meshed( corner )
                    && !topology_inspector
                        .corner_vertices_are_associated_to_unique_vertices(
                            corner ) )
                {
                    return false;
                }
            }
            for( const auto& line : section_.lines() )
            {
                if( topology_inspector.line_is_meshed( line )
                    && !topology_inspector
                        .line_vertices_are_associated_to_unique_vertices(
                            line ) )
                {
                    return false;
                }
            }
            for( const auto& surface : section_.surfaces() )
            {
                if( topology_inspector.surface_is_meshed( surface )
                    && !topology_inspector
                        .surface_vertices_are_associated_to_unique_vertices(
                            surface ) )
                {
                    return false;
                }
            }
            return true;
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
        section_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex()
            const
    {
        return impl_
            ->section_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex();
    }

    SectionTopologyInspectionResult
        SectionTopologyInspector::inspect_section_topology() const
    {
        return impl_->inspect_section_topology( *this );
    }
} // namespace geode
