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

#include <geode/inspector/criterion/negative_elements/solid_negative_elements.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/basic_objects/tetrahedron.hpp>
#include <geode/geometry/information.hpp>
#include <geode/geometry/sign.hpp>

#include <geode/mesh/core/solid_mesh.hpp>

namespace geode
{
    template < index_t dimension >
    class SolidMeshNegativeElements< dimension >::Impl
    {
    public:
        Impl( const SolidMesh< dimension >& mesh ) : mesh_( mesh ) {}

        bool mesh_has_negative_elements() const
        {
            for( const auto polyhedron_id : Range{ mesh_.nb_polyhedra() } )
            {
                if( polyhedron_has_negative_volume( polyhedron_id ) )
                {
                    return true;
                }
            }
            return false;
        }

        InspectionIssues< index_t > negative_polyhedra() const
        {
            InspectionIssues< index_t > wrong_polyhedra{
                "Negative Polyhedra."
            };
            for( const auto polyhedron_id : Range{ mesh_.nb_polyhedra() } )
            {
                if( polyhedron_has_negative_volume( polyhedron_id ) )
                {
                    wrong_polyhedra.add_issue( polyhedron_id,
                        absl::StrCat( "Polyhedron ", polyhedron_id,
                            " of Solid ", mesh_.id().string(),
                            " has a negative volume." ) );
                }
            }
            return wrong_polyhedra;
        }

    private:
        bool polyhedron_has_negative_volume( index_t polyhedron_id ) const
        {
            if( mesh_.nb_polyhedron_vertices( polyhedron_id ) == 4 )
            {
                Tetrahedron tetrahedron{ mesh_.point( mesh_.polyhedron_vertex(
                                             { polyhedron_id, 0 } ) ),
                    mesh_.point(
                        mesh_.polyhedron_vertex( { polyhedron_id, 1 } ) ),
                    mesh_.point(
                        mesh_.polyhedron_vertex( { polyhedron_id, 2 } ) ),
                    mesh_.point(
                        mesh_.polyhedron_vertex( { polyhedron_id, 3 } ) ) };
                return tetrahedron_volume_sign( tetrahedron ) == Sign::negative;
            }
            return mesh_.polyhedron_volume( polyhedron_id ) < 0;
        }

    private:
        const SolidMesh< dimension >& mesh_;
    };

    template < index_t dimension >
    SolidMeshNegativeElements< dimension >::SolidMeshNegativeElements(
        const SolidMesh< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    SolidMeshNegativeElements< dimension >::~SolidMeshNegativeElements() =
        default;

    template < index_t dimension >
    bool SolidMeshNegativeElements< dimension >::mesh_has_negative_elements()
        const
    {
        return impl_->mesh_has_negative_elements();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        SolidMeshNegativeElements< dimension >::negative_polyhedra() const
    {
        return impl_->negative_polyhedra();
    }

    template class opengeode_inspector_inspector_api
        SolidMeshNegativeElements< 3 >;
} // namespace geode
