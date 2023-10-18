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

#pragma once

#include <absl/types/span.h>

#include <geode/inspector/common.h>

namespace geode
{
    class BRep;
    class Section;
    struct uuid;
    struct ComponentMeshVertex;
    class ComponentID;
    class VertexSet;
} // namespace geode

namespace geode
{
    namespace detail
    {
        bool brep_blocks_are_meshed( const geode::BRep& brep );

        bool section_surfaces_are_meshed( const geode::Section& section );

        std::vector< uuid > components_uuids(
            absl::Span< const ComponentMeshVertex > components );

        std::pair< std::vector< geode::ComponentMeshVertex >,
            std::vector< std::string > >
            brep_component_vertices_not_associated_to_unique_vertices(
                const geode::BRep& brep,
                const geode::ComponentID& component_id,
                const geode::VertexSet& component_mesh );
    } // namespace detail
} // namespace geode