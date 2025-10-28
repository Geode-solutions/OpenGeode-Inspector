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

#include <geode/inspector/criterion/degeneration/solid_degeneration.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/mensuration.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

#include <geode/inspector/criterion/internal/degeneration_impl.hpp>

namespace geode
{
    template < index_t dimension >
    class SolidMeshDegeneration< dimension >::Impl
        : public internal::DegenerationImpl< SolidMesh< dimension > >
    {
    public:
        Impl( const SolidMesh< dimension >& mesh )
            : internal::DegenerationImpl< SolidMesh< dimension > >{ mesh }
        {
        }

        bool is_mesh_degenerated() const final
        {
            if( this->internal::DegenerationImpl<
                    SolidMesh< dimension > >::is_mesh_degenerated() )
            {
                return true;
            }
            for( const auto polyhedron_id :
                Range{ this->mesh().nb_polyhedra() } )
            {
                if( this->mesh().is_polyhedron_degenerated( polyhedron_id ) )
                {
                    return true;
                }
            }
            return false;
        }

        InspectionIssues< index_t > small_height_polyhedra(
            double threshold ) const
        {
            InspectionIssues< index_t > wrong_polyhedra{
                "Degenerated Polyhedra."
            };
            for( const auto polyhedron_id :
                Range{ this->mesh().nb_polyhedra() } )
            {
                if( this->mesh().polyhedron_minimum_height( polyhedron_id )
                    <= threshold )
                {
                    wrong_polyhedra.add_issue( polyhedron_id,
                        absl::StrCat( "Polyhedron ", polyhedron_id,
                            " is degenerated." ) );
                }
            }
            return wrong_polyhedra;
        }

        InspectionIssues< index_t > degenerated_polyhedra() const
        {
            return small_height_polyhedra( GLOBAL_EPSILON );
        }
    };

    template < index_t dimension >
    SolidMeshDegeneration< dimension >::SolidMeshDegeneration(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshDegeneration< dimension >::~SolidMeshDegeneration() = default;

    template < index_t dimension >
    bool SolidMeshDegeneration< dimension >::is_mesh_degenerated() const
    {
        return impl_->is_mesh_degenerated();
    }

    template < index_t dimension >
    InspectionIssues< index_t > SolidMeshDegeneration< dimension >::small_edges(
        double threshold ) const
    {
        return impl_->small_edges( threshold );
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SolidMeshDegeneration< dimension >::degenerated_edges() const
    {
        return impl_->degenerated_edges();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SolidMeshDegeneration< dimension >::small_height_polyhedra(
            double threshold ) const
    {
        return impl_->small_height_polyhedra( threshold );
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SolidMeshDegeneration< dimension >::degenerated_polyhedra() const
    {
        return impl_->degenerated_polyhedra();
    }

    template class opengeode_inspector_inspector_api SolidMeshDegeneration< 3 >;
} // namespace geode
