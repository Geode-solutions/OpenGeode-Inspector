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

#include <geode/mesh/core/solid_mesh.hpp>
#include <geode/mesh/core/surface_mesh.hpp>
#include <geode/model/mixin/core/component_type.hpp>

#include <geode/inspector/inspection/information.hpp>

#include "../inspection_helpers.hpp"

namespace geode
{
    template < typename IssueType >
    inline void do_define_information( pybind11::module& module,
        const std::string& typestr,
        const std::string& issue_description )
    {
        using InspectionIssues = geode::InspectionIssues< IssueType >;
        const auto name = absl::StrCat( "InspectionIssues", typestr );
        define_string_representation(
            pybind11::class_< InspectionIssues >( module, name.c_str(),
                absl::StrCat(
                    "Issues found by one inspection of a mesh or a "
                    "model: a description of what is inspected, and a "
                    "list of issues, each one being ",
                    issue_description,
                    ". The message of each issue is only available in "
                    "string()" )
                    .c_str() )
                .def( pybind11::init<>(),
                    "Create a list of issues without issue, with a default "
                    "description" )
                .def( pybind11::init< std::string_view >(),
                    pybind11::arg( "issue_description" ),
                    "Create a list of issues without issue, describing what is "
                    "inspected" )
                .def( "set_description", &InspectionIssues::set_description,
                    pybind11::arg( "issue_description" ),
                    "Set the description of what is inspected" )
                .def( "description", &InspectionIssues::description,
                    "Return the description of what is inspected" )
                .def( "nb_issues", &InspectionIssues::nb_issues,
                    "Return the number of issues" )
                .def( "add_issue", &InspectionIssues::add_issue,
                    pybind11::arg( "issue" ), pybind11::arg( "message" ),
                    "Add an issue at the end of the list, with the message "
                    "describing it in string()" )
                .def( "issues", &InspectionIssues::issues,
                    "Return the list of the issues, in the order they were "
                    "found" ),
            []( const InspectionIssues& issues ) {
                return absl::StrCat( python_type_name( issues ),
                    "(description=",
                    python_repr( std::string{ issues.description() } ),
                    ", nb_issues=", issues.nb_issues(), ")" );
            } );

        using InspectionIssuesMap = geode::InspectionIssuesMap< IssueType >;
        const auto map_name = absl::StrCat( "InspectionIssuesMap", typestr );
        define_string_representation(
            pybind11::class_< InspectionIssuesMap >( module, map_name.c_str(),
                absl::StrCat(
                    "Issues found by one inspection of the components "
                    "of a model: a description of what is inspected, "
                    "and the issues of each component having some "
                    "(InspectionIssues, whose issues are each ",
                    issue_description,
                    "), indexed by the uuid of the component" )
                    .c_str() )
                .def( pybind11::init<>(),
                    "Create a map without issue, with a default description" )
                .def( pybind11::init< std::string_view >(),
                    pybind11::arg( "issue_description" ),
                    "Create a map without issue, describing what is inspected" )
                .def( "set_description", &InspectionIssuesMap::set_description,
                    pybind11::arg( "issue_description" ),
                    "Set the description of what is inspected" )
                .def( "description", &InspectionIssuesMap::description,
                    "Return the description of what is inspected" )
                .def( "nb_issues", &InspectionIssuesMap::nb_issues,
                    "Return the total number of issues, summed over every "
                    "component of the map" )
                .def( "nb_components_with_issues",
                    &InspectionIssuesMap::nb_components_with_issues,
                    "Return the number of components having at least one "
                    "issue" )
                .def( "add_issues_to_map",
                    &InspectionIssuesMap::add_issues_to_map,
                    pybind11::arg( "id_to_issues" ), pybind11::arg( "issues" ),
                    "Add the issues of the component with this uuid; nothing "
                    "is "
                    "added when there is no issue, and nothing is replaced "
                    "when "
                    "the component already has issues in the map" )
                .def( "issues_map", &InspectionIssuesMap::issues_map,
                    "Return a dict giving the issues (InspectionIssues) of "
                    "each "
                    "component with issues, indexed by its uuid, in the order "
                    "the "
                    "components were added. It is a copy: modifying it does "
                    "not "
                    "modify the map" ),
            []( const InspectionIssuesMap& issues ) {
                return absl::StrCat( python_type_name( issues ),
                    "(description=",
                    python_repr( std::string{ issues.description() } ),
                    ", nb_issues=", issues.nb_issues(),
                    ", nb_components_with_issues=",
                    issues.nb_components_with_issues(), ")" );
            } );
    }

    inline void define_information( pybind11::module& module )
    {
        do_define_information< index_t >( module, "Index",
            "an index (of a vertex, an edge, a polygon, a polyhedron or a "
            "unique vertex, depending on the inspection)" );
        do_define_information< std::vector< index_t > >( module, "VectorIndex",
            "a list of indices (a group of colocated vertices)" );
        do_define_information< PolyhedronFacet >( module, "PolyhedronFacet",
            "a PolyhedronFacet (a facet of a polyhedron)" );
        do_define_information< std::array< index_t, 2 > >( module,
            "ArrayIndex2",
            "a list of two indices (the two vertices of an edge)" );
        do_define_information< PolyhedronFacetVertices >( module,
            "PolyhedronFacetVertices",
            "a list of vertex indices (the vertices of a polyhedron facet)" );
        do_define_information< PolygonEdge >(
            module, "PolygonEdge", "a PolygonEdge (an edge of a polygon)" );
        do_define_information< std::pair< index_t, index_t > >( module,
            "PairIndex", "a pair of indices (two intersecting elements)" );
        do_define_information< uuid >(
            module, "UUID", "a uuid (the identifier of a component)" );
        do_define_information< ComponentID >( module, "ComponentID",
            "a ComponentID (the type and the uuid of a component)" );
        do_define_information< std::pair< geode::ComponentMeshElement,
            geode::ComponentMeshElement > >( module, "PairComponentMeshElement",
            "a pair of ComponentMeshElement (two intersecting elements, each "
            "one in the mesh of a component)" );
    }
} // namespace geode
