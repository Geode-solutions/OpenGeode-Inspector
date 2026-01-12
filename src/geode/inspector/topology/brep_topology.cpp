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

#include <geode/inspector/topology/brep_topology.hpp>

#include <async++.h>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>

namespace geode
{
    index_t BRepTopologyInspectionResult::nb_issues() const
    {
        return corners.nb_issues() + lines.nb_issues() + surfaces.nb_issues()
               + blocks.nb_issues()
               + unique_vertices_not_linked_to_any_component.nb_issues()
               + unique_vertices_linked_to_inexistant_cmv.nb_issues()
               + unique_vertices_nonbijectively_linked_to_cmv.nb_issues();
    }

    std::string BRepTopologyInspectionResult::string() const
    {
        return absl::StrCat( corners.string(), lines.string(),
            surfaces.string(), blocks.string(),
            unique_vertices_not_linked_to_any_component.string(),
            unique_vertices_linked_to_inexistant_cmv.string(),
            unique_vertices_nonbijectively_linked_to_cmv.string() );
    }

    std::string BRepTopologyInspectionResult::inspection_type() const
    {
        return "Model topology inspection";
    }

    class BRepTopologyInspector::Impl
    {
    public:
        Impl( const BRep& brep ) : brep_( brep ) {}

        bool
            brep_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex()
                const
        {
            for( const auto uv_id : Range{ brep_.nb_unique_vertices() } )
            {
                const auto& unique_vertex_cmvs =
                    brep_.component_mesh_vertices( uv_id );
                if( unique_vertex_cmvs.empty() )
                {
                    return false;
                }
                for( const auto& cmv : unique_vertex_cmvs )
                {
                    if( !cmv_exists_in_brep( cmv ) )
                    {
                        return false;
                    }
                    if( brep_.component( cmv.component_id.id() ).is_active()
                        && brep_.unique_vertex( cmv ) != uv_id )
                    {
                        return false;
                    }
                }
            }
            return true;
        }

        void add_unique_vertices_with_wrong_cmv_link(
            BRepTopologyInspectionResult& brep_issues ) const
        {
            auto& not_linked_result =
                brep_issues.unique_vertices_not_linked_to_any_component;
            auto& linked_to_inexistant_result =
                brep_issues.unique_vertices_linked_to_inexistant_cmv;
            auto& linked_to_nonbijective_result =
                brep_issues.unique_vertices_nonbijectively_linked_to_cmv;
            for( const auto uv_id : Range{ brep_.nb_unique_vertices() } )
            {
                const auto& unique_vertex_cmvs =
                    brep_.component_mesh_vertices( uv_id );
                if( unique_vertex_cmvs.empty() )
                {
                    not_linked_result.add_issue(
                        uv_id, absl::StrCat( "unique vertex ", uv_id,
                                   " is not linked to any mesh vertex." ) );
                    continue;
                }
                for( const auto& cmv : unique_vertex_cmvs )
                {
                    if( !cmv_exists_in_brep( cmv ) )
                    {
                        linked_to_inexistant_result.add_issue(
                            uv_id, absl::StrCat( "unique vertex ", uv_id,
                                       " is linked to inexistant mesh vertex [",
                                       cmv.string(), "]." ) );
                        continue;
                    }
                    if( brep_.component( cmv.component_id.id() ).is_active()
                        && brep_.unique_vertex( cmv ) != uv_id )
                    {
                        linked_to_nonbijective_result.add_issue(
                            uv_id, absl::StrCat( "unique vertex ", uv_id,
                                       " is linked to inexistant mesh vertex [",
                                       cmv.string(), "]." ) );
                    }
                }
            }
        }

        bool brep_topology_is_valid(
            const BRepTopologyInspector& brep_topology_inspector ) const
        {
            if( brep_.nb_unique_vertices() == 0 )
            {
                return false;
            }
            if( !brep_meshed_components_are_linked_to_unique_vertices(
                    brep_topology_inspector ) )
            {
                return false;
            }
            if( !brep_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex() )
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
            try
            {
                async::parallel_invoke(
                    [&result, &brep_topology_inspector] {
                        result.corners =
                            brep_topology_inspector.inspect_corners_topology();
                    },
                    [&result, &brep_topology_inspector] {
                        result.lines =
                            brep_topology_inspector.inspect_lines_topology();
                    },
                    [&result, &brep_topology_inspector] {
                        result.surfaces =
                            brep_topology_inspector.inspect_surfaces_topology();
                    },
                    [&result, &brep_topology_inspector] {
                        result.blocks =
                            brep_topology_inspector.inspect_blocks();
                    } );
            }
            catch( OpenGeodeException& )
            {
            }
            add_unique_vertices_with_wrong_cmv_link( result );
            return result;
        }

    private:
        bool cmv_exists_in_brep( const ComponentMeshVertex& cmv ) const
        {
            if( cmv.component_id.type() == Corner3D::component_type_static() )
            {
                if( brep_.has_corner( cmv.component_id.id() )
                    && cmv.vertex < brep_.corner( cmv.component_id.id() )
                               .mesh()
                               .nb_vertices() )
                {
                    return true;
                }
            }
            if( cmv.component_id.type() == Line3D::component_type_static() )
            {
                if( brep_.has_line( cmv.component_id.id() )
                    && cmv.vertex < brep_.line( cmv.component_id.id() )
                               .mesh()
                               .nb_vertices() )
                {
                    return true;
                }
            }
            if( cmv.component_id.type() == Surface3D::component_type_static() )
            {
                if( brep_.has_surface( cmv.component_id.id() )
                    && cmv.vertex < brep_.surface( cmv.component_id.id() )
                               .mesh()
                               .nb_vertices() )
                {
                    return true;
                }
            }
            if( cmv.component_id.type() == Block3D::component_type_static() )
            {
                if( brep_.has_block( cmv.component_id.id() )
                    && cmv.vertex < brep_.block( cmv.component_id.id() )
                               .mesh()
                               .nb_vertices() )
                {
                    return true;
                }
            }
            return false;
        }

        bool brep_meshed_components_are_linked_to_unique_vertices(
            const BRepTopologyInspector& topology_inspector ) const
        {
            for( const auto& corner : brep_.active_corners() )
            {
                if( topology_inspector.corner_is_meshed( corner )
                    && !topology_inspector
                        .corner_vertices_are_associated_to_unique_vertices(
                            corner ) )
                {
                    return false;
                }
            }
            for( const auto& line : brep_.active_lines() )
            {
                if( topology_inspector.line_is_meshed( line )
                    && !topology_inspector
                        .line_vertices_are_associated_to_unique_vertices(
                            line ) )
                {
                    return false;
                }
            }
            for( const auto& surface : brep_.active_surfaces() )
            {
                if( topology_inspector.surface_is_meshed( surface )
                    && !topology_inspector
                        .surface_vertices_are_associated_to_unique_vertices(
                            surface ) )
                {
                    return false;
                }
            }
            for( const auto& block : brep_.active_blocks() )
            {
                if( topology_inspector.block_is_meshed( block )
                    && !topology_inspector
                        .block_vertices_are_associated_to_unique_vertices(
                            block ) )
                {
                    return false;
                }
            }
            return true;
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

    BRepTopologyInspector::~BRepTopologyInspector() = default;

    bool BRepTopologyInspector::brep_topology_is_valid() const
    {
        return impl_->brep_topology_is_valid( *this );
    }

    bool BRepTopologyInspector::
        brep_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex()
            const
    {
        return impl_
            ->brep_unique_vertices_are_bijectively_linked_to_an_existing_component_vertex();
    }

    BRepTopologyInspectionResult
        BRepTopologyInspector::inspect_brep_topology() const
    {
        return impl_->inspect_brep_topology( *this );
    }
} // namespace geode
