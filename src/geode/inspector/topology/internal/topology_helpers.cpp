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

#include <geode/inspector/topology/internal/topology_helpers.hpp>

#include <geode/mesh/core/point_set.hpp>
#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/core/section.hpp>

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
    } // namespace internal
} // namespace geode