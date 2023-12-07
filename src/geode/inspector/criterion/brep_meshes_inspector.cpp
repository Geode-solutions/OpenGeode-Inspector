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

#include <geode/inspector/criterion/brep_meshes_inspector.h>

namespace geode
{

    BRepMeshesInspector::BRepMeshesInspector( const BRep& brep )
        : BRepUniqueVerticesColocation( brep ),
          BRepComponentMeshesAdjacency( brep ),
          BRepComponentMeshesColocation( brep ),
          BRepComponentMeshesDegeneration( brep ),
          BRepComponentMeshesManifold( brep ),
          BRepMeshesIntersections( brep )
    {
    }

    BRepMeshesInspectionResult BRepMeshesInspector::inspect_brep_meshes() const
    {
        BRepMeshesInspectionResult result;
        result.colocated_unique_vertices_groups =
            colocated_unique_vertices_groups();
        result.unique_vertices_linked_to_different_points =
            unique_vertices_linked_to_different_points();

        result.surfaces_edges_with_wrong_adjacencies =
            surfaces_edges_with_wrong_adjacencies();
        result.blocks_facets_with_wrong_adjacencies =
            blocks_facets_with_wrong_adjacencies();
        return result;
    }
} // namespace geode
