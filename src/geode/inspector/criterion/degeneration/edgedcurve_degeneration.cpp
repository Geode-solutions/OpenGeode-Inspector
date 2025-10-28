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

#include <geode/inspector/criterion/degeneration/edgedcurve_degeneration.hpp>
#include <geode/inspector/criterion/internal/degeneration_impl.hpp>

#include <geode/basic/pimpl_impl.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/geometry/point.hpp>

#include <geode/mesh/core/edged_curve.hpp>

namespace geode
{
    template < index_t dimension >
    class EdgedCurveDegeneration< dimension >::Impl
    {
    public:
        Impl( const EdgedCurve< dimension >& mesh ) : mesh_( mesh ) {}

        bool is_mesh_degenerated() const
        {
            for( const auto edge_id : Range{ mesh_.nb_edges() } )
            {
                if( mesh_.is_edge_degenerated( edge_id ) )
                {
                    return true;
                }
            }
            return false;
        }

        InspectionIssues< index_t > small_edges( double threshold ) const
        {
            InspectionIssues< index_t > degenerated_edges_index{
                "Degenerated Edges."
            };
            for( const auto edge_id : Range{ mesh_.nb_edges() } )
            {
                if( mesh_.edge_length( edge_id ) <= threshold )
                {
                    degenerated_edges_index.add_issue( edge_id,
                        absl::StrCat( "Edge ", edge_id, ", at position [",
                            mesh_.edge_barycenter( edge_id ).string(), "]" ) );
                }
            }
            return degenerated_edges_index;
        }

        InspectionIssues< index_t > degenerated_edges() const
        {
            return small_edges( GLOBAL_EPSILON );
        }

    private:
        const EdgedCurve< dimension >& mesh_;
    };

    template < index_t dimension >
    EdgedCurveDegeneration< dimension >::EdgedCurveDegeneration(
        const EdgedCurve< dimension >& mesh )
        : impl_( mesh )
    {
    }

    template < index_t dimension >
    EdgedCurveDegeneration< dimension >::~EdgedCurveDegeneration() = default;

    template < index_t dimension >
    bool EdgedCurveDegeneration< dimension >::is_mesh_degenerated() const
    {
        return impl_->is_mesh_degenerated();
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        EdgedCurveDegeneration< dimension >::small_edges(
            double threshold ) const
    {
        return impl_->small_edges( threshold );
    }

    template < index_t dimension >
    InspectionIssues< index_t >
        EdgedCurveDegeneration< dimension >::degenerated_edges() const
    {
        return impl_->degenerated_edges();
    }

    template class opengeode_inspector_inspector_api
        EdgedCurveDegeneration< 2 >;
    template class opengeode_inspector_inspector_api
        EdgedCurveDegeneration< 3 >;
} // namespace geode
