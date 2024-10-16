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

#include <geode/inspector/criterion/degeneration/brep_meshes_degeneration.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/criterion/degeneration/solid_degeneration.hpp>
#include <geode/inspector/criterion/internal/component_meshes_degeneration.hpp>

namespace geode
{
    std::string BRepMeshesDegenerationInspectionResult::string() const
    {
        std::string message;
        if( degenerated_edges.nb_issues() != 0 )
        {
            absl::StrAppend( &message, degenerated_edges.string(), "\n" );
        }
        if( degenerated_polygons.nb_issues() != 0 )
        {
            absl::StrAppend( &message, degenerated_polygons.string(), "\n" );
        }
        if( degenerated_polyhedra.nb_issues() != 0 )
        {
            absl::StrAppend( &message, degenerated_polyhedra.string(), "\n" );
        }
        if( message.empty() )
        {
            return "No degeneration issues in model component meshes \n";
        }
        return message;
    }

    std::string BRepMeshesDegenerationInspectionResult::inspection_type() const
    {
        return "Degeneration inspection";
    }

    class BRepComponentMeshesDegeneration::Impl
        : public ComponentMeshesDegeneration< BRep >
    {
    public:
        Impl( const BRep& brep ) : ComponentMeshesDegeneration< BRep >( brep )
        {
        }

        void add_solid_degenerations(
            InspectionIssuesMap< index_t >& degenerated_edges_map,
            InspectionIssuesMap< index_t >& degenerated_polyhedra_map ) const
        {
            for( const auto& block : model().blocks() )
            {
                const geode::SolidMeshDegeneration3D inspector{ block.mesh() };
                auto degenerated_edges = inspector.degenerated_edges();
                degenerated_edges.set_description( absl::StrCat(
                    "Block ", block.id().string(), " degenerated edges" ) );
                degenerated_edges_map.add_issues_to_map(
                    block.id(), std::move( degenerated_edges ) );
                auto degenerated_polyhedra = inspector.degenerated_polyhedra();
                degenerated_polyhedra.set_description( absl::StrCat(
                    "Block ", block.id().string(), " degenerated polyhedra" ) );
                degenerated_polyhedra_map.add_issues_to_map(
                    block.id(), std::move( degenerated_polyhedra ) );
            }
        }
    };

    BRepComponentMeshesDegeneration::BRepComponentMeshesDegeneration(
        const BRep& model )
        : impl_{ model }
    {
    }

    BRepComponentMeshesDegeneration::~BRepComponentMeshesDegeneration() =
        default;

    BRepMeshesDegenerationInspectionResult
        BRepComponentMeshesDegeneration::inspect_elements_degeneration() const
    {
        BRepMeshesDegenerationInspectionResult result;
        impl_->add_degenerated_edges( result.degenerated_edges );
        impl_->add_degenerated_polygons( result.degenerated_polygons );
        impl_->add_solid_degenerations(
            result.degenerated_edges, result.degenerated_polyhedra );
        return result;
    }
} // namespace geode
