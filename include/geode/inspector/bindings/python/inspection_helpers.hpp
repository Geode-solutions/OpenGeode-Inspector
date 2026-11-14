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

#pragma once

#include <string>

#include <absl/container/linked_hash_map.h>
#include <absl/strings/str_cat.h>
#include <absl/strings/str_join.h>

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <geode/bindings/python/common.hpp>
#include <geode/bindings/python/model/model_vertex_check.hpp>

namespace pybind11
{
    namespace detail
    {
        template < typename Key, typename Value >
        struct type_caster< absl::linked_hash_map< Key, Value > >
            : map_caster< absl::linked_hash_map< Key, Value >, Key, Value >
        {
        };
    } // namespace detail
} // namespace pybind11

namespace geode
{
    /*!
     * The inspections only read the model or the mesh: the GIL is released
     * while they run.
     */
    using ReleaseGil = pybind11::call_guard< pybind11::gil_scoped_release >;

    /*!
     * Wrap a const method of a model inspector taking a unique vertex index
     * first: the index is checked against the model remembered by the
     * constructor (remember_model, IndexError when out of range), then the
     * method runs without the GIL. The Python signature is typed with
     * Inspector.
     */
    template < typename Inspector,
        typename Class,
        typename Return,
        typename... Args >
    auto checked_unique_vertex( std::string_view name,
        Return ( Class::*method )( index_t, Args... ) const )
    {
        return
            [name = std::string{ name }, method]( TypedObject< Inspector > self,
                index_t unique_vertex_index, Args... args ) -> Return {
                if( pybind11::hasattr( self, "_model" ) )
                {
                    check_model_unique_vertex(
                        name, self.attr( "_model" ), unique_vertex_index );
                }
                const auto& inspector = self.template cast< const Class& >();
                const pybind11::gil_scoped_release release;
                return ( inspector.*method )( unique_vertex_index, args... );
            };
    }

    // Call form of the inspection results: the total number of issues, and
    // the number of issues of each member having some, e.g.
    // SurfaceInspectionResult(nb_issues=8, degenerated_edges=3,
    // isolated_vertices=5)
    inline std::string result_repr( const pybind11::object& self )
    {
        const auto property_type =
            pybind11::module_::import( "builtins" ).attr( "property" );
        std::vector< std::string > arguments{ absl::StrCat(
            "nb_issues=", self.attr( "nb_issues" )().cast< index_t >() ) };
        for( const auto item :
            pybind11::type::of( self ).attr( "__dict__" ).attr( "items" )() )
        {
            const auto member = item.cast< pybind11::tuple >();
            if( !pybind11::isinstance( member[1], property_type ) )
            {
                continue;
            }
            const auto value = self.attr( member[0] );
            if( !pybind11::hasattr( value, "nb_issues" ) )
            {
                continue;
            }
            const auto nb_issues =
                value.attr( "nb_issues" )().cast< index_t >();
            if( nb_issues != 0 )
            {
                arguments.push_back( absl::StrCat(
                    member[0].cast< std::string >(), "=", nb_issues ) );
            }
        }
        return absl::StrCat(
            pybind11::type::of( self ).attr( "__name__" ).cast< std::string >(),
            "(", absl::StrJoin( arguments, ", " ), ")" );
    }

    /*!
     * Define the members shared by every inspection result: the constructor
     * building a result without issue, nb_issues() and inspection_type().
     * The members of the result are then defined with
     * INSPECTION_RESULT_MEMBER.
     */
    template < typename Result >
    pybind11::class_< Result > define_inspection_result(
        pybind11::module& module, const char* name, const char* doc )
    {
        auto python_class = define_string_representation(
            pybind11::class_< Result >( module, name, doc ), &result_repr );
        python_class
            .def( pybind11::init<>(),
                "Create a result without any issue (every inspection is "
                "described as not tested)" )
            .def( "nb_issues", &Result::nb_issues,
                "Return the total number of issues of the result: the "
                "sum of the issues of its members" )
            .def( "inspection_type", &Result::inspection_type,
                "Return the title of the kind of inspection of this "
                "result" );
        return python_class;
    }
} // namespace geode

#define INSPECTION_RESULT_MEMBER( python_class, Result, member, doc )          \
    geode::def_readwrite_named( python_class, #member, &Result::member, doc )
