/*
 * Copyright (c) 2019 - 2026 Geode-solutions
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

#include <geode/tests_config.hpp>

#include <geode/basic/logger.hpp>

#include <geode/model/representation/core/brep.hpp>
#include <geode/model/representation/io/brep_input.hpp>

#include <geode/inspector/validity/brep_validity.hpp>
#include <geode/inspector/validity/object_validity.hpp>

void check_model_a1()
{
    const auto model_brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "model_A1.og_brep" ) );
    const auto invalidities = geode::is_brep_valid( model_brep );

    geode::OpenGeodeInspectorValidityException::test(
        invalidities.nb_issues() == 7, "model_A1 has ",
        invalidities.nb_issues(), " issues instead of 7." );
}

void check_model_a1_valid()
{
    const auto model_brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "model_A1_valid.og_brep" ) );
    const auto invalidities = geode::is_brep_valid( model_brep );

    geode::OpenGeodeInspectorValidityException::test(
        invalidities.nb_issues() == 7, "model_A1_valid has ",
        invalidities.nb_issues(), " issues instead of 7." );
}

void check_model_mss()
{
    const auto model_brep =
        geode::load_brep( absl::StrCat( geode::DATA_PATH, "mss.og_brep" ) );
    const auto invalidities = geode::is_brep_valid( model_brep );

    geode::OpenGeodeInspectorValidityException::test(
        invalidities.nb_issues() == 5, "model_mss has ",
        invalidities.nb_issues(), " issues instead of 5." );
}

void check_model_D()
{
    const auto model_brep =
        geode::load_brep( absl::StrCat( geode::DATA_PATH, "model_D.og_brep" ) );
    const auto invalidities = geode::is_brep_valid( model_brep );

    geode::OpenGeodeInspectorValidityException::test(
        invalidities.nb_issues() == 0, "model_D has ", invalidities.nb_issues(),
        " issues instead of 0." );
}

void check_wrong_bsurfaces_model()
{
    const auto model_brep = geode::load_brep( absl::StrCat(
        geode::DATA_PATH, "wrong_boundary_surface_model.og_brep" ) );
    const auto invalidities = geode::is_brep_valid( model_brep );

    geode::OpenGeodeInspectorValidityException::test(
        invalidities.nb_issues() == 2, "wrong_boundary_surface_model has ",
        invalidities.nb_issues(), " issues instead of 2." );
}

void check_segmented_cube()
{
    const auto model_brep = geode::load_brep(
        absl::StrCat( geode::DATA_PATH, "cube_segmented.og_brep" ) );
    const auto invalidities = geode::is_brep_valid( model_brep );

    geode::OpenGeodeInspectorValidityException::test(
        invalidities.nb_issues() == 1, "cube_segmented has ",
        invalidities.nb_issues(), " issues instead of 1." );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorValidityLibrary::initialize();
        geode::Logger::set_level( geode::Logger::LEVEL::debug );
        check_model_a1();
        check_model_a1_valid();
        check_model_mss();
        check_model_D();
        check_wrong_bsurfaces_model();
        check_segmented_cube();
        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
