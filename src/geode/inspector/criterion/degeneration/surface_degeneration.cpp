/*
 * Copyright (c) 2019 - 2024 Geode-solutions
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

#include <geode/inspector/criterion/degeneration/surface_degeneration.h>
#include <geode/inspector/criterion/internal/degeneration_impl.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>
#include <geode/basic/uuid.h>

#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/mensuration.h>

#include <geode/mesh/core/surface_mesh.h>

namespace geode
{
    template < index_t dimension >
    class SurfaceMeshDegeneration< dimension >::Impl
        : public internal::DegenerationImpl< SurfaceMesh< dimension > >
    {
    public:
        Impl( const SurfaceMesh< dimension >& mesh )
            : internal::DegenerationImpl< SurfaceMesh< dimension > >{ mesh }
        {
        }

        bool is_mesh_degenerated() const final
        {
            if( this->internal::DegenerationImpl<
                    SurfaceMesh< dimension > >::is_mesh_degenerated() )
            {
                return true;
            }
            for( const auto polygon_id : Range{ this->mesh().nb_polygons() } )
            {
                if( this->mesh().is_polygon_degenerated( polygon_id ) )
                {
                    return true;
                }
            }
            return false;
        }

        InspectionIssues< index_t > degenerated_polygons() const
        {
            InspectionIssues< index_t > wrong_polygons{
                "Degenerated Polygons."
            };
            for( const auto polygon_id : Range{ this->mesh().nb_polygons() } )
            {
                if( this->mesh().is_polygon_degenerated( polygon_id ) )
                {
                    wrong_polygons.add_issue( polygon_id,
                        absl::StrCat( "Polygon ", polygon_id, " of Surface ",
                            this->mesh().id().string(), " is degenerated." ) );
                }
            }
            return wrong_polygons;
        }
    };

    template < index_t dimension >
    SurfaceMeshDegeneration< dimension >::SurfaceMeshDegeneration(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SurfaceMeshDegeneration< dimension >::~SurfaceMeshDegeneration()
    {
    }

    template < index_t dimension >
    bool SurfaceMeshDegeneration< dimension >::is_mesh_degenerated() const
    {
        return impl_->is_mesh_degenerated();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SurfaceMeshDegeneration< dimension >::degenerated_edges() const
    {
        return impl_->degenerated_edges();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SurfaceMeshDegeneration< dimension >::degenerated_polygons() const
    {
        return impl_->degenerated_polygons();
    }

    template class opengeode_inspector_inspector_api
        SurfaceMeshDegeneration< 2 >;
    template class opengeode_inspector_inspector_api
        SurfaceMeshDegeneration< 3 >;
} // namespace geode
