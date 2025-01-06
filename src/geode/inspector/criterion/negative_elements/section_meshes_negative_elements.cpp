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

#include <geode/inspector/criterion/negative_elements/section_meshes_negative_elements.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/mesh/core/surface_mesh.hpp>

#include <geode/model/mixin/core/surface.hpp>
#include <geode/model/representation/core/section.hpp>

#include <geode/inspector/criterion/negative_elements/surface_negative_elements.hpp>

namespace geode
{
    std::string SectionMeshesNegativeElementsInspectionResult::string() const
    {
        if( negative_polygons.nb_issues() != 0 )
        {
            return absl::StrCat( negative_polygons.string(), "\n" );
        }
        return "No negative elements issues in model component meshes \n";
    }

    std::string
        SectionMeshesNegativeElementsInspectionResult::inspection_type() const
    {
        return "Negative elements inspection";
    }

    class SectionComponentMeshesNegativeElements::Impl
    {
    public:
        Impl( const Section& section ) : section_( section ) {}

        SectionMeshesNegativeElementsInspectionResult
            surface_negative_elements() const
        {
            SectionMeshesNegativeElementsInspectionResult result;
            for( const auto& surface : section_.surfaces() )
            {
                const geode::SurfaceMeshNegativeElements2D inspector{
                    surface.mesh()
                };
                auto negative_elements = inspector.negative_polygons();
                negative_elements.set_description( absl::StrCat(
                    "Surface ", surface.id().string(), " negative polygons" ) );
                result.negative_polygons.add_issues_to_map(
                    surface.id(), std::move( negative_elements ) );
            }
            return result;
        }

    private:
        const Section& section_;
    };

    SectionComponentMeshesNegativeElements::
        SectionComponentMeshesNegativeElements( const Section& model )
        : impl_{ model }
    {
    }

    SectionComponentMeshesNegativeElements::
        ~SectionComponentMeshesNegativeElements() = default;

    SectionMeshesNegativeElementsInspectionResult
        SectionComponentMeshesNegativeElements::inspect_negative_elements()
            const
    {
        return impl_->surface_negative_elements();
    }
} // namespace geode
