/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/inspector/validity/brep_validity.hpp>

#include <geode/mesh/core/point_set.hpp>

#include <geode/inspector/inspection/brep_inspector.hpp>
#include <geode/inspector/validity/object_validity.hpp>

namespace
{
    void add_brep_colocation_issues(
        const geode::BRepMeshesInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        const auto& uv_coloc = inspection.unique_vertices_colocation;
        if( uv_coloc.colocated_unique_vertices_groups.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                uv_coloc.colocated_unique_vertices_groups.string() );
        }
        if( uv_coloc.unique_vertices_linked_to_different_points.nb_issues()
            > 0 )
        {
            invalidities.invalidities.push_back(
                uv_coloc.unique_vertices_linked_to_different_points.string() );
        }
        const auto& meshes_coloc =
            inspection.meshes_colocation.colocated_points_groups;
        if( meshes_coloc.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back( meshes_coloc.string() );
        }
    }

    void add_brep_adjacencies_issues(
        const geode::BRepMeshesInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        const auto& meshes_adj = inspection.meshes_adjacencies;
        if( meshes_adj.surfaces_edges_with_wrong_adjacencies.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                meshes_adj.surfaces_edges_with_wrong_adjacencies.string() );
        }
        if( meshes_adj.blocks_facets_with_wrong_adjacencies.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                meshes_adj.blocks_facets_with_wrong_adjacencies.string() );
        }
    }

    void add_brep_degeneration_issues(
        const geode::BRepMeshesInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        const auto& meshes_degen = inspection.meshes_degenerations;
        if( meshes_degen.degenerated_edges.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                meshes_degen.degenerated_edges.string() );
        }
        if( meshes_degen.degenerated_polygons.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                meshes_degen.degenerated_polygons.string() );
        }
        if( meshes_degen.degenerated_polyhedra.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                meshes_degen.degenerated_polyhedra.string() );
        }
    }

    void add_brep_manifold_issues(
        const geode::BRepMeshesInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        const auto& manifolds = inspection.meshes_non_manifolds;
        if( manifolds.meshes_non_manifold_vertices.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                manifolds.meshes_non_manifold_vertices.string() );
        }
        if( manifolds.meshes_non_manifold_edges.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                manifolds.meshes_non_manifold_edges.string() );
        }
        if( manifolds.meshes_non_manifold_facets.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                manifolds.meshes_non_manifold_facets.string() );
        }
        if( manifolds.brep_non_manifold_edges.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                manifolds.brep_non_manifold_edges.string() );
        }
        if( manifolds.brep_non_manifold_facets.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                manifolds.brep_non_manifold_facets.string() );
        }
    }

    void add_brep_meshes_invalidities(
        const geode::BRepMeshesInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        add_brep_colocation_issues( inspection, invalidities );
        add_brep_adjacencies_issues( inspection, invalidities );
        add_brep_degeneration_issues( inspection, invalidities );
        const auto& meshes_inters = inspection.meshes_intersections;
        if( meshes_inters.elements_intersections.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                meshes_inters.elements_intersections.string() );
        }
        add_brep_manifold_issues( inspection, invalidities );
        const auto& negatives =
            inspection.meshes_negative_elements.negative_polyhedra;
        if( negatives.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back( negatives.string() );
        }
    }

    void add_brep_corners_topology_issues(
        const geode::BRepTopologyInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        const auto& corners_topo = inspection.corners;
        if( corners_topo.corners_not_meshed.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                corners_topo.corners_not_meshed.string() );
        }
        if( corners_topo.corners_not_linked_to_a_unique_vertex.nb_issues() > 0 )
        {
            invalidities.invalidities.push_back(
                corners_topo.corners_not_linked_to_a_unique_vertex.string() );
        }
        if( corners_topo.unique_vertices_linked_to_multiple_corners.nb_issues()
            > 0 )
        {
            invalidities.invalidities.push_back( corners_topo
                    .unique_vertices_linked_to_multiple_corners.string() );
        }
        if( corners_topo.unique_vertices_linked_to_multiply_embedded_corner
                .nb_issues()
            > 0 )
        {
            invalidities.invalidities.push_back(
                corners_topo.unique_vertices_linked_to_multiply_embedded_corner
                    .string() );
        }
        if( corners_topo
                .unique_vertices_linked_to_not_internal_nor_boundary_corner
                .nb_issues()
            > 0 )
        {
            invalidities.invalidities.push_back( corners_topo
                    .unique_vertices_linked_to_not_internal_nor_boundary_corner
                    .string() );
        }
        if( corners_topo.unique_vertices_liked_to_not_boundary_line_corner
                .nb_issues()
            > 0 )
        {
            invalidities.invalidities.push_back(
                corners_topo.unique_vertices_liked_to_not_boundary_line_corner
                    .string() );
        }
    }

    void add_brep_lines_topology_issues(
        const geode::BRepTopologyInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        const auto& lines_topo = inspection.lines;
        if( lines_topo.lines_not_meshed.nb_issues() != 0 )
        {
            invalidities.invalidities.push_back(
                lines_topo.lines_not_meshed.string() );
        }
        if( lines_topo
                .unique_vertices_linked_to_line_with_wrong_relationship_to_surface
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( lines_topo
                    .unique_vertices_linked_to_line_with_wrong_relationship_to_surface
                    .string() );
        }
        if( lines_topo.lines_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            invalidities.invalidities.push_back(
                lines_topo.lines_not_linked_to_a_unique_vertex.string() );
        }
        if( lines_topo.unique_vertices_linked_to_a_line_with_invalid_embeddings
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( lines_topo
                    .unique_vertices_linked_to_a_line_with_invalid_embeddings
                    .string() );
        }
        if( lines_topo.unique_vertices_linked_to_a_single_and_invalid_line
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back(
                lines_topo.unique_vertices_linked_to_a_single_and_invalid_line
                    .string() );
        }
        if( lines_topo
                .unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( lines_topo
                    .unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner
                    .string() );
        }
        if( lines_topo.line_edges_with_wrong_component_edges_around.nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( lines_topo
                    .line_edges_with_wrong_component_edges_around.string() );
        }
    }

    void add_brep_surfaces_topology_issues(
        const geode::BRepTopologyInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        const auto& surfaces_topo = inspection.surfaces;
        if( surfaces_topo.surfaces_not_meshed.nb_issues() != 0 )
        {
            invalidities.invalidities.push_back(
                surfaces_topo.surfaces_not_meshed.string() );
        }
        if( surfaces_topo.surfaces_not_linked_to_a_unique_vertex.nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back(
                surfaces_topo.surfaces_not_linked_to_a_unique_vertex.string() );
        }
        if( surfaces_topo
                .unique_vertices_linked_to_a_surface_with_invalid_embbedings
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( surfaces_topo
                    .unique_vertices_linked_to_a_surface_with_invalid_embbedings
                    .string() );
        }
        if( surfaces_topo.unique_vertices_linked_to_several_and_invalid_surfaces
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( surfaces_topo
                    .unique_vertices_linked_to_several_and_invalid_surfaces
                    .string() );
        }
        if( surfaces_topo
                .unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( surfaces_topo
                    .unique_vertices_linked_to_a_line_but_is_not_on_a_surface_border
                    .string() );
        }
        if( surfaces_topo.surface_polygons_with_wrong_component_facets_around
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( surfaces_topo
                    .surface_polygons_with_wrong_component_facets_around
                    .string() );
        }
    }

    void add_brep_blocks_topology_issues(
        const geode::BRepTopologyInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        const auto& blocks_topo = inspection.blocks;
        if( blocks_topo.some_blocks_not_meshed.nb_issues() != 0 )
        {
            invalidities.invalidities.push_back(
                blocks_topo.some_blocks_not_meshed.string() );
        }
        if( blocks_topo.wrong_block_boundary_surface.nb_issues() != 0 )
        {
            invalidities.invalidities.push_back(
                blocks_topo.wrong_block_boundary_surface.string() );
        }
        if( blocks_topo.blocks_not_linked_to_a_unique_vertex.nb_issues() != 0 )
        {
            invalidities.invalidities.push_back(
                blocks_topo.blocks_not_linked_to_a_unique_vertex.string() );
        }
        if( blocks_topo
                .unique_vertices_part_of_two_blocks_and_no_boundary_surface
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( blocks_topo
                    .unique_vertices_part_of_two_blocks_and_no_boundary_surface
                    .string() );
        }
        if( blocks_topo.unique_vertices_with_incorrect_block_cmvs_count
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( blocks_topo
                    .unique_vertices_with_incorrect_block_cmvs_count.string() );
        }
        if( blocks_topo
                .unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( blocks_topo
                    .unique_vertices_linked_to_surface_with_wrong_relationship_to_blocks
                    .string() );
        }
        if( blocks_topo.unique_vertices_linked_to_a_single_and_invalid_surface
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( blocks_topo
                    .unique_vertices_linked_to_a_single_and_invalid_surface
                    .string() );
        }
        if( blocks_topo.blocks_with_not_closed_boundary_surfaces.nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back(
                blocks_topo.blocks_with_not_closed_boundary_surfaces.string() );
        }
        if( blocks_topo.model_boundaries_dont_form_a_closed_surface.nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( blocks_topo
                    .model_boundaries_dont_form_a_closed_surface.string() );
        }
        if( blocks_topo.unique_vertex_linked_to_multiple_invalid_surfaces
                .nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back(
                blocks_topo.unique_vertex_linked_to_multiple_invalid_surfaces
                    .string() );
        }
    }

    void add_brep_uv_topology_issues(
        const geode::BRepTopologyInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        if( inspection.unique_vertices_not_linked_to_any_component.nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( inspection
                    .unique_vertices_not_linked_to_any_component.string() );
        }
        if( inspection.unique_vertices_linked_to_inexistant_cmv.nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back(
                inspection.unique_vertices_linked_to_inexistant_cmv.string() );
        }
        if( inspection.unique_vertices_nonbijectively_linked_to_cmv.nb_issues()
            != 0 )
        {
            invalidities.invalidities.push_back( inspection
                    .unique_vertices_nonbijectively_linked_to_cmv.string() );
        }
    }

    void add_brep_topology_invalidities(
        const geode::BRepTopologyInspectionResult& inspection,
        geode::ObjectValidity& invalidities )
    {
        add_brep_corners_topology_issues( inspection, invalidities );
        add_brep_lines_topology_issues( inspection, invalidities );
        add_brep_surfaces_topology_issues( inspection, invalidities );
        add_brep_blocks_topology_issues( inspection, invalidities );
        add_brep_uv_topology_issues( inspection, invalidities );
    }
} // namespace

namespace geode
{
    ObjectValidity is_brep_valid( const BRep& brep )
    {
        BRepInspector brep_inspector{ brep };
        const auto inspection_result = brep_inspector.inspect_brep();
        ObjectValidity invalidities;
        add_brep_meshes_invalidities( inspection_result.meshes, invalidities );
        add_brep_topology_invalidities(
            inspection_result.topology, invalidities );
        return invalidities;
    }
} // namespace geode
