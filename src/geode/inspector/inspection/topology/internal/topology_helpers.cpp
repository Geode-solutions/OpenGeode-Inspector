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

#include <geode/inspector/inspection/topology/internal/topology_helpers.hpp>

#include <absl/strings/str_cat.h>

#include <geode/mesh/core/edged_curve.hpp>
#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/corner.hpp>
#include <geode/model/mixin/core/line.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

namespace
{
    template < typename Model >
    std::optional< std::string > line_end_not_a_corner(
        const Model& model, geode::index_t unique_vertex_index )
    {
        std::optional< geode::uuid > ending_line_id;
        for( const auto& cmv :
            model.component_mesh_vertices( unique_vertex_index ) )
        {
            if( cmv.component_id.type
                == geode::Corner< Model::dim >::component_type_static() )
            {
                return std::nullopt;
            }
            if( cmv.component_id.type
                != geode::Line< Model::dim >::component_type_static() )
            {
                continue;
            }
            const auto& line = model.line( cmv.component_id.id );
            if( line.is_active()
                && line.mesh().edges_around_vertex( cmv.vertex ).size() == 1 )
            {
                ending_line_id = line.id();
            }
        }
        if( !ending_line_id )
        {
            return std::nullopt;
        }
        const auto& line = model.line( ending_line_id.value() );
        return absl::StrCat( "unique vertex ", unique_vertex_index,
            " is an end of Line ", line.name().value_or( line.id().string() ),
            " (", line.id().string(), "), but is not a Corner." );
    }

    template < typename Model >
    std::optional< std::string > wrong_nb_boundary_corner_line_cmvs(
        const Model& model,
        geode::index_t unique_vertex_index,
        const geode::Corner< Model::dim >& corner,
        const geode::Line< Model::dim >& line,
        geode::index_t nb_line_cmvs )
    {
        const auto line_is_closed = model.is_closed( line );
        if( nb_line_cmvs == 1 || ( line_is_closed && nb_line_cmvs == 2 ) )
        {
            return std::nullopt;
        }
        return absl::StrCat( "unique vertex with index ", unique_vertex_index,
            " is associated with Corner ",
            corner.name().value_or( corner.id().string() ), " (",
            corner.id().string(), "), which is boundary to Line ",
            line.name().value_or( line.id().string() ), " (",
            line.id().string(), "), ",
            line_is_closed ? "a closed Line, so Line should have 1 or 2 cmvs"
                           : "an open Line, so Line should have 1 cmv",
            " on this unique vertex, but has ", nb_line_cmvs,
            " vertices on it instead." );
    }
} // namespace

namespace geode
{
    namespace internal
    {
        bool brep_blocks_are_meshed( const BRep& brep )
        {
            for( const auto& block : brep.blocks() )
            {
                if( block.mesh().nb_polyhedra() == 0 )
                {
                    return false;
                }
            }
            return true;
        }

        bool section_surfaces_are_meshed( const Section& section )
        {
            for( const auto& surface : section.surfaces() )
            {
                if( surface.mesh().nb_polygons() == 0 )
                {
                    return false;
                }
            }
            return true;
        }

        std::optional< std::string > line_end_not_a_corner_message(
            const BRep& brep, index_t unique_vertex_index )
        {
            return line_end_not_a_corner( brep, unique_vertex_index );
        }

        std::optional< std::string > line_end_not_a_corner_message(
            const Section& section, index_t unique_vertex_index )
        {
            return line_end_not_a_corner( section, unique_vertex_index );
        }

        std::optional< std::string > wrong_nb_boundary_corner_line_cmvs_message(
            const BRep& brep,
            index_t unique_vertex_index,
            const Corner3D& corner,
            const Line3D& line,
            index_t nb_line_cmvs )
        {
            return wrong_nb_boundary_corner_line_cmvs(
                brep, unique_vertex_index, corner, line, nb_line_cmvs );
        }

        std::optional< std::string > wrong_nb_boundary_corner_line_cmvs_message(
            const Section& section,
            index_t unique_vertex_index,
            const Corner2D& corner,
            const Line2D& line,
            index_t nb_line_cmvs )
        {
            return wrong_nb_boundary_corner_line_cmvs(
                section, unique_vertex_index, corner, line, nb_line_cmvs );
        }
    } // namespace internal
} // namespace geode