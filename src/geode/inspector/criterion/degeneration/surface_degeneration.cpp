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

#include <geode/inspector/criterion/degeneration/surface_degeneration.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/basic_objects/triangle.hpp>
#include <geode/geometry/mensuration.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/inspector/criterion/internal/degeneration_impl.hpp>

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

        InspectionIssues< index_t > small_height_polygons(
            double tolerance ) const
        {
            InspectionIssues< index_t > wrong_polygons{
                "Degenerated Polygons."
            };
            for( const auto polygon_id : Range{ this->mesh().nb_polygons() } )
            {
                if( this->mesh().polygon_minimum_height( polygon_id )
                    <= tolerance )
                {
                    wrong_polygons.add_issue( polygon_id,
                        absl::StrCat( "Polygon ", polygon_id, " of Surface ",
                            this->mesh().id().string(), " is degenerated." ) );
                }
            }
            return wrong_polygons;
        }

        InspectionIssues< index_t > degenerated_polygons() const
        {
            return small_height_polygons( GLOBAL_EPSILON );
        }
    };

    template < index_t dimension >
    SurfaceMeshDegeneration< dimension >::SurfaceMeshDegeneration(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SurfaceMeshDegeneration< dimension >::~SurfaceMeshDegeneration() = default;

    template < index_t dimension >
    bool SurfaceMeshDegeneration< dimension >::is_mesh_degenerated() const
    {
        return impl_->is_mesh_degenerated();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SurfaceMeshDegeneration< dimension >::small_edges(
            double threshold ) const
    {
        return impl_->small_edges( threshold );
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

    template < index_t dimension >
    InspectionIssues< index_t >
        SurfaceMeshDegeneration< dimension >::small_height_polygons(
            double threshold ) const
    {
        return impl_->small_height_polygons( threshold );
    }

    template class opengeode_inspector_inspector_api
        SurfaceMeshDegeneration< 2 >;
    template class opengeode_inspector_inspector_api
        SurfaceMeshDegeneration< 3 >;
} // namespace geode
