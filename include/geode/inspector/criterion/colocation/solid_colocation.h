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

#include <geode/basic/pimpl.h>

#include <geode/inspector/common.h>

namespace geode
{
    FORWARD_DECLARATION_DIMENSION_CLASS( SolidMesh );
} // namespace geode

namespace geode
{
    /*!
     * Class for inspecting the colocation of points in a SolidMesh
     */
    template < index_t dimension >
    class opengeode_inspector_inspector_api SolidMeshColocation
    {
        OPENGEODE_DISABLE_COPY( SolidMeshColocation );
        OPENGEODE_TEMPLATE_ASSERT_3D( dimension );

    public:
        SolidMeshColocation( const SolidMesh< dimension >& mesh );

        SolidMeshColocation( const SolidMesh< dimension >& mesh, bool verbose );

        ~SolidMeshColocation();

        bool mesh_has_colocated_points() const;

        index_t nb_colocated_points() const;

        std::vector< std::vector< index_t > > colocated_points_groups() const;

    private:
        IMPLEMENTATION_MEMBER( impl_ );
    };
    ALIAS_3D( SolidMeshColocation );
} // namespace geode