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
#include <string>

#include <geode/inspector/information.h>

namespace geode
{
    template < typename ProblemType >
    void do_define_information(
        pybind11::module& module, const std::string& typestr )
    {
        using InspectionIssues = geode::InspectionIssues< ProblemType >;
        const auto name = absl::StrCat( "InspectionIssues", typestr );
        pybind11::class_< InspectionIssues >( module, name.c_str() )
            .def_readwrite( "description", &InspectionIssues::description )
            .def_readwrite( "problems", &InspectionIssues::problems )
            .def_readwrite( "messages", &InspectionIssues::messages )
            .def( "number", &InspectionIssues::number )
            .def( "string", &InspectionIssues::string );
    }
    void define_information( pybind11::module& module )
    {
        do_define_information< index_t >( module, "Index" );
        do_define_information< std::vector< index_t > >(
            module, "VectorIndex" );
        do_define_information< PolyhedronFacet >( module, "PolyhedronFacet" );
        do_define_information< std::array< index_t, 2 > >(
            module, "ArrayIndex2" );
        do_define_information< PolyhedronFacetVertices >(
            module, "PolyhedronFacetVertices" );
        do_define_information< PolygonEdge >( module, "PolygonEdge" );
        do_define_information< std::pair< index_t, index_t > >(
            module, "PairIndex" );
        do_define_information< uuid >( module, "UUID" );
        do_define_information< std::pair< geode::ComponentMeshElement,
            geode::ComponentMeshElement > >( module, PairComponentMeshElement )
    }
} // namespace geode
