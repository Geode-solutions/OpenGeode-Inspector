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

#include <geode/inspector/criterion/negative_elements/brep_meshes_negative_elements.hpp>

#include <geode/basic/logger.hpp>
#include <geode/basic/pimpl_impl.hpp>

#include <geode/model/mixin/core/block.hpp>
#include <geode/model/representation/core/brep.hpp>

#include <geode/inspector/criterion/negative_elements/solid_negative_elements.hpp>

namespace geode
{
    std::string BRepMeshesNegativeElementsInspectionResult::string() const
    {
        if( negative_polyhedra.nb_issues() != 0 )
        {
            return absl::StrCat( negative_polyhedra.string(), "\n" );
        }
        return "No negative elements issues in model component meshes \n";
    }

    std::string
        BRepMeshesNegativeElementsInspectionResult::inspection_type() const
    {
        return "Negative elements inspection";
    }

    class BRepComponentMeshesNegativeElements::Impl
    {
    public:
        Impl( const BRep& brep ) : brep_( brep ) {}

        BRepMeshesNegativeElementsInspectionResult
            block_negative_elements() const
        {
            BRepMeshesNegativeElementsInspectionResult result;
            for( const auto& block : brep_.blocks() )
            {
                const geode::SolidMeshNegativeElements3D inspector{
                    block.mesh()
                };
                auto negative_elements = inspector.negative_polyhedra();
                negative_elements.set_description( absl::StrCat(
                    "Block ", block.id().string(), " negative polyhedra" ) );
                result.negative_polyhedra.add_issues_to_map(
                    block.id(), std::move( negative_elements ) );
            }
            return result;
        }

    private:
        const BRep& brep_;
    };

    BRepComponentMeshesNegativeElements::BRepComponentMeshesNegativeElements(
        const BRep& model )
        : impl_{ model }
    {
    }

    BRepComponentMeshesNegativeElements::~BRepComponentMeshesNegativeElements()
    {
    }

    BRepMeshesNegativeElementsInspectionResult
        BRepComponentMeshesNegativeElements::inspect_negative_elements() const
    {
        return impl_->block_negative_elements();
    }
} // namespace geode
