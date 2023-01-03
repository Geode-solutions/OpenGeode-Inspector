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

#include <geode/inspector/criterion/degeneration/edgedcurve_degeneration.h>
#include <geode/inspector/criterion/private/degeneration_impl.h>

#include <geode/basic/logger.h>
#include <geode/basic/pimpl_impl.h>

#include <geode/geometry/point.h>

#include <geode/mesh/core/edged_curve.h>

namespace geode
{
    template < index_t dimension >
    class EdgedCurveDegeneration< dimension >::Impl
    {
    public:
        Impl( const EdgedCurve< dimension >& mesh, bool verbose )
            : mesh_( mesh ), verbose_( verbose )
        {
        }

        bool is_mesh_degenerated() const
        {
            for( const auto edge_index : Range{ mesh_.nb_edges() } )
            {
                if( mesh_.edge_length( edge_index ) < global_epsilon )
                {
                    return true;
                }
            }
            return false;
        }

        index_t nb_degenerated_edges() const
        {
            index_t nb_degeneration{ 0 };
            for( const auto edge_index : Range{ mesh_.nb_edges() } )
            {
                if( mesh_.edge_length( edge_index ) < global_epsilon )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Edge with index ", edge_index,
                            ", at position [",
                            mesh_.edge_barycenter( edge_index ).string(),
                            "], is degenerated." );
                    }
                    nb_degeneration++;
                }
            }
            return nb_degeneration;
        }

        std::vector< index_t > degenerated_edges() const
        {
            std::vector< index_t > degenerated_edges_index;
            for( const auto edge_index : Range{ mesh_.nb_edges() } )
            {
                if( mesh_.edge_length( edge_index ) < global_epsilon )
                {
                    if( verbose_ )
                    {
                        Logger::info( "Edge with index ", edge_index,
                            ", at position [",
                            mesh_.edge_barycenter( edge_index ).string(),
                            "], is degenerated." );
                    }
                    degenerated_edges_index.push_back( edge_index );
                }
            }
            return degenerated_edges_index;
        }

    private:
        const EdgedCurve< dimension >& mesh_;
        DEBUG_CONST bool verbose_;
    };

    template < index_t dimension >
    EdgedCurveDegeneration< dimension >::EdgedCurveDegeneration(
        const EdgedCurve< dimension >& mesh )
        : impl_( mesh, false )
    {
    }

    template < index_t dimension >
    EdgedCurveDegeneration< dimension >::EdgedCurveDegeneration(
        const EdgedCurve< dimension >& mesh, bool verbose )
        : impl_( mesh, verbose )
    {
    }

    template < index_t dimension >
    EdgedCurveDegeneration< dimension >::~EdgedCurveDegeneration()
    {
    }

    template < index_t dimension >
    bool EdgedCurveDegeneration< dimension >::is_mesh_degenerated() const
    {
        return impl_->is_mesh_degenerated();
    }

    template < index_t dimension >
    index_t EdgedCurveDegeneration< dimension >::nb_degenerated_edges() const
    {
        return impl_->nb_degenerated_edges();
    }

    template < index_t dimension >
    std::vector< index_t >
        EdgedCurveDegeneration< dimension >::degenerated_edges() const
    {
        return impl_->degenerated_edges();
    }

    template class opengeode_inspector_inspector_api
        EdgedCurveDegeneration< 2 >;
    template class opengeode_inspector_inspector_api
        EdgedCurveDegeneration< 3 >;
} // namespace geode
