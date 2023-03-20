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

#include <geode/inspector/criterion/degeneration/surface_degeneration.h>
#include <geode/inspector/criterion/private/degeneration_impl.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/basic_objects/triangle.h>
#include <geode/geometry/mensuration.h>

#include <geode/mesh/core/surface_mesh.h>

namespace geode
{
    template < index_t dimension >
    class SurfaceMeshDegeneration< dimension >::Impl
        : public detail::DegenerationImpl< SurfaceMesh< dimension > >
    {
    public:
        Impl( const SurfaceMesh< dimension >& mesh, bool verbose )
            : detail::DegenerationImpl< SurfaceMesh< dimension > >{ mesh,
                  verbose }
        {
        }

        bool is_mesh_degenerated() const final
        {
            if( this->detail::DegenerationImpl<
                    SurfaceMesh< dimension > >::is_mesh_degenerated() )
            {
                return true;
            }
            for( const auto polygon_id : Range{ this->mesh().nb_polygons() } )
            {
                if( polygon_is_degenerated( polygon_id ) )
                {
                    return true;
                }
            }
            return false;
        }

        index_t nb_degenerated_polygons() const
        {
            index_t counter{ 0 };
            for( const auto polygon_id : Range{ this->mesh().nb_polygons() } )
            {
                if( polygon_is_degenerated( polygon_id ) )
                {
                    counter++;
                }
            }
            return counter;
        }

        std::vector< index_t > degenerated_polygons() const
        {
            std::vector< index_t > wrong_polygons;
            for( const auto polygon_id : Range{ this->mesh().nb_polygons() } )
            {
                if( polygon_is_degenerated( polygon_id ) )
                {
                    wrong_polygons.push_back( polygon_id );
                }
            }
            return wrong_polygons;
        }

    private:
        bool polygon_is_degenerated( index_t polygon_id ) const
        {
            const auto& mesh = this->mesh();
            if( mesh.polygon_area( polygon_id ) > global_epsilon )
            {
                return false;
            }
            if( this->verbose() )
            {
                Logger::info( "Polygon ", polygon_id, " is degenerated." );
            }
            return true;
        }
    };

    template < index_t dimension >
    SurfaceMeshDegeneration< dimension >::SurfaceMeshDegeneration(
        const SurfaceMesh< dimension >& mesh )
        : impl_( mesh, false )
    {
    }

    template < index_t dimension >
    SurfaceMeshDegeneration< dimension >::SurfaceMeshDegeneration(
        const SurfaceMesh< dimension >& mesh, bool verbose )
        : impl_( mesh, verbose )
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
    index_t SurfaceMeshDegeneration< dimension >::nb_degenerated_edges() const
    {
        return impl_->nb_degenerated_edges();
    }

    template < index_t dimension >
    index_t
        SurfaceMeshDegeneration< dimension >::nb_degenerated_polygons() const
    {
        return impl_->nb_degenerated_polygons();
    }

    template < index_t dimension >
    std::vector< index_t >
        SurfaceMeshDegeneration< dimension >::degenerated_edges() const
    {
        return impl_->degenerated_edges();
    }

    template < index_t dimension >
    std::vector< index_t >
        SurfaceMeshDegeneration< dimension >::degenerated_polygons() const
    {
        return impl_->degenerated_polygons();
    }

    template class opengeode_inspector_inspector_api
        SurfaceMeshDegeneration< 2 >;
    template class opengeode_inspector_inspector_api
        SurfaceMeshDegeneration< 3 >;
} // namespace geode
