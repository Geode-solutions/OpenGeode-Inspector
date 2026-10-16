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

#include <geode/basic/assert.hpp>
#include <geode/basic/logger.hpp>
#include <geode/basic/range.hpp>
#include <geode/basic/uuid.hpp>

#include <geode/inspector/inspection/information.hpp>

namespace
{
    geode::InspectionIssues< geode::index_t > make_issues(
        geode::index_t nb_issues )
    {
        geode::InspectionIssues< geode::index_t > issues{ "component issues" };
        for( const auto issue : geode::Range{ nb_issues } )
        {
            issues.add_issue( issue, absl::StrCat( "issue ", issue ) );
        }
        return issues;
    }
} // namespace

void check_issues()
{
    geode::InspectionIssues< geode::index_t > empty{ "nothing" };
    geode::OpenGeodeInspectorInspectionException::test(
        empty.string() == "no issues with nothing\n",
        "[Test] Wrong empty InspectionIssues string: ", empty.string() );

    const auto issues = make_issues( 2 );
    geode::OpenGeodeInspectorInspectionException::test(
        issues.nb_issues() == 2, "[Test] Wrong InspectionIssues count" );
    const std::string expected{
        "2 issues with component issues\n\tissue 0\n\tissue 1\n"
    };
    geode::OpenGeodeInspectorInspectionException::test(
        issues.string() == expected,
        "[Test] Wrong InspectionIssues string: ", issues.string() );
}

void check_issues_map()
{
    geode::InspectionIssuesMap< geode::index_t > map{ "map issues" };
    geode::OpenGeodeInspectorInspectionException::test(
        map.nb_issues() == 0 && map.nb_components_with_issues() == 0,
        "[Test] Empty InspectionIssuesMap should have no issues" );
    geode::OpenGeodeInspectorInspectionException::test(
        map.string() == "no issues with map issues\n",
        "[Test] Wrong empty InspectionIssuesMap string: ", map.string() );

    const geode::uuid first;
    const geode::uuid second;
    const geode::uuid third;
    map.add_issues_to_map( first, make_issues( 3 ) );
    map.add_issues_to_map( second, make_issues( 0 ) );
    map.add_issues_to_map( third, make_issues( 1 ) );
    geode::OpenGeodeInspectorInspectionException::test(
        map.nb_components_with_issues() == 2,
        "[Test] InspectionIssuesMap should have 2 components with issues, "
        "not ",
        map.nb_components_with_issues() );
    geode::OpenGeodeInspectorInspectionException::test( map.nb_issues() == 4,
        "[Test] InspectionIssuesMap should have 4 issues, not ",
        map.nb_issues() );
    geode::index_t nb_issues_from_map{ 0 };
    for( const auto& issues : map.issues_map() )
    {
        nb_issues_from_map += issues.second.nb_issues();
    }
    geode::OpenGeodeInspectorInspectionException::test(
        map.nb_issues() == nb_issues_from_map,
        "[Test] InspectionIssuesMap count differs from its content" );

    const std::string expected{ "4 issues with map issues"
                                "\n\t3 issues with component issues"
                                "\n\tissue 0\n\tissue 1\n\tissue 2\n"
                                "\n\t1 issues with component issues"
                                "\n\tissue 0\n"
                                "\n" };
    geode::OpenGeodeInspectorInspectionException::test(
        map.string() == expected,
        "[Test] Wrong InspectionIssuesMap string: ", map.string() );
}

int main()
{
    try
    {
        geode::OpenGeodeInspectorInspectionLibrary::initialize();
        check_issues();
        check_issues_map();

        geode::Logger::info( "TEST SUCCESS" );
        return 0;
    }
    catch( ... )
    {
        return geode::geode_lippincott();
    }
}
