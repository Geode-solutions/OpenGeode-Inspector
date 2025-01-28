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

#include <geode/inspector/criterion/negative_elements/surface_negative_elements.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/sign.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

namespace geode
{
    template < index_t dimension >
    class SurfaceMeshNegativeElements< dimension >::Impl
    {
    public:
        Impl( const SurfaceMesh< dimension >& mesh ) : mesh_( mesh ) {}

        bool mesh_has_negative_elements() const
        {
            for( const auto polygon_id : Range{ mesh_.nb_polygons() } )
            {
                if( polygon_has_negative_area( polygon_id ) )
                {
                    return true;
                }
            }
            return false;
        }

        InspectionIssues< index_t > negative_polygons() const
        {
            InspectionIssues< index_t > wrong_polygons{ "Negative Polygons." };
            for( const auto polygon_id : Range{ mesh_.nb_polygons() } )
            {
                if( polygon_has_negative_area( polygon_id ) )
                {
                    wrong_polygons.add_issue( polygon_id,
                        absl::StrCat( "Polygon ", polygon_id, " of Surface ",
                            mesh_.id().string(), " has a negative area." ) );
                }
            }
            return wrong_polygons;
        }

    private:
        bool polygon_has_negative_area( index_t polygon_id ) const
        {
            if( mesh_.nb_polygon_vertices( polygon_id ) == 4 )
            {
                Triangle< dimension > triangle{
                    mesh_.point( mesh_.polygon_vertex( { polygon_id, 0 } ) ),
                    mesh_.point( mesh_.polygon_vertex( { polygon_id, 1 } ) ),
                    mesh_.point( mesh_.polygon_vertex( { polygon_id, 2 } ) )
                };
                return triangle_area_sign( triangle ) == Sign::negative;
            }
            return mesh_.polygon_area( polygon_id ) < 0;
        }

    private:
        const SurfaceMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    SurfaceMeshNegativeElements< dimension >::SurfaceMeshNegativeElements(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SurfaceMeshNegativeElements< dimension >::~SurfaceMeshNegativeElements() =
        default;

    template < index_t dimension >
    bool SurfaceMeshNegativeElements< dimension >::mesh_has_negative_elements()
        const
    {
        return impl_->mesh_has_negative_elements();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SurfaceMeshNegativeElements< dimension >::negative_polygons() const
    {
        return impl_->negative_polygons();
    }

    template class opengeode_inspector_inspector_api
        SurfaceMeshNegativeElements< 2 >;
} // namespace geode
