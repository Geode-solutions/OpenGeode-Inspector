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

#include <geode/inspector/validity/object_validity.hpp>

#include "../inspection_helpers.hpp"

namespace geode
{
    inline void define_object_validity( pybind11::module& module )
    {
        auto validity = define_string_representation(
            pybind11::class_< ObjectValidity >( module, "ObjectValidity",
                "Validity of a mesh or of a model, as returned by the "
                "is_*_valid functions: its list of invalidities is empty when "
                "the object is valid, and bool(validity) is then True" ),
            []( const ObjectValidity& object_validity ) {
                return absl::StrCat( "ObjectValidity(nb_issues=",
                    object_validity.nb_issues(), ")" );
            } );
        validity
            .def( pybind11::init<>(), "Create the validity of a valid object" )
            .def( "nb_issues", &ObjectValidity::nb_issues,
                "Return the number of invalidities" )
            .def(
                "__bool__",
                []( const ObjectValidity& object_validity ) {
                    return static_cast< bool >( object_validity );
                },
                "Return True if the object is valid: there is no invalidity" );
        def_readwrite_named( validity, "invalidities",
            &ObjectValidity::invalidities,
            "The list of invalidities: one message per inspection that found "
            "issues, describing them" );
    }
} // namespace geode
