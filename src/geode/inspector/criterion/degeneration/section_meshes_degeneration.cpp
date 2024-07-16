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

#include <geode/inspector/criterion/degeneration/section_meshes_degeneration.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/degeneration/solid_degeneration.hpp>
#include <geode/inspector/criterion/internal/component_meshes_degeneration.hpp>

namespace geode
{
    std::string SectionMeshesDegenerationInspectionResult::string() const
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
        if( !message.empty() )
        {
            return message;
        }
        return "No degeneration issues in model component meshes \n";
    }

    std::string
        SectionMeshesDegenerationInspectionResult::inspection_type() const
    {
        return "Degeneration inspection";
    }

    class SectionComponentMeshesDegeneration::Impl
        : public ComponentMeshesDegeneration< Section >
    {
    public:
        Impl( const Section& section )
            : ComponentMeshesDegeneration< Section >( section )
        {
        }
    };

    SectionComponentMeshesDegeneration::SectionComponentMeshesDegeneration(
        const Section& model )
        : impl_{ model }
    {
    }

    SectionComponentMeshesDegeneration::~SectionComponentMeshesDegeneration() {}

    SectionMeshesDegenerationInspectionResult
        SectionComponentMeshesDegeneration::inspect_elements_degeneration()
            const
    {
        SectionMeshesDegenerationInspectionResult result;
        impl_->add_degenerated_edges( result.degenerated_edges );
        impl_->add_degenerated_polygons( result.degenerated_polygons );
        return result;
    }
} // namespace geode
