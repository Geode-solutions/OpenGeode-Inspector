/*
 * Copyright (c) 2019 - 2022 Geode-solutions
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

#include <geode/inspector/topology/private/topology_helpers.h>

#include <geode/basic/algorithm.h>

#include <geode/mesh/core/solid_mesh.h>

#include <geode/model/mixin/core/block.h>
#include <geode/model/representation/core/brep.h>

namespace geode
{
    namespace detail
    {
        bool brep_blocks_are_meshed( const geode::BRep& brep )
        {
            for( const auto& block : brep.blocks() )
            {
                if( block.mesh().nb_vertices() == 0 )
                {
                    return false;
                }
            }
            return true;
        }

        std::vector< uuid > components_uuids(
            absl::Span< const MeshComponentVertex > components )
        {
            std::vector< uuid > component_uuids;
            component_uuids.reserve( components.size() );
            for( const auto& mcv : components )
            {
                component_uuids.push_back( mcv.component_id.id() );
            }
            sort_unique( component_uuids );
            return component_uuids;
        }
    } // namespace detail
} // namespace geode