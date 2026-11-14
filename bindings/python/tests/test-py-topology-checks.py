# Copyright (c) 2019 - 2026 Geode-solutions
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

"""The checks of the topology inspectors on one unique vertex: the unique vertex
index is checked (IndexError), the component indices are checked, and the
results agree with the inspection of the whole model."""

import os
import re

import geode_test_env

import opengeode
import opengeode_inspector_py_inspection as inspection


def check(condition, message):
    if not condition:
        raise ValueError(f"[Test] {message}")


def data_dir():
    return os.path.abspath(
        os.path.join(os.path.dirname(__file__), "../../../tests/data")
    )


def load_brep():
    return opengeode.load(
        opengeode.BRep, os.path.join(data_dir(), "model_A1.og_brep")
    )


def load_section():
    return opengeode.load(
        opengeode.Section, os.path.join(data_dir(), "vertical_lines.og_sctn")
    )


UNIQUE_VERTEX = re.compile(r"\(self: [\w.]+, unique_vertex_index: ")

# Class -> number of checks of one unique vertex
CLASSES = {
    "BRepCornersTopology": 5,
    "BRepLinesTopology": 5,
    "BRepSurfacesTopology": 4,
    "BRepBlocksTopology": 6,
    "SectionCornersTopology": 5,
    "SectionLinesTopology": 5,
    "SectionSurfacesTopology": 3,
}


def unique_vertex_methods(cls):
    """Names of the methods of the class (not inherited) taking a unique vertex"""
    return [
        name
        for name, member in vars(cls).items()
        if callable(member)
        and not name.startswith("_")
        and UNIQUE_VERTEX.search(member.__doc__ or "")
    ]


def call_with_defaults(inspector, name, unique_vertex):
    method = getattr(inspector, name)
    arguments = [unique_vertex]
    if "not_boundary_surfaces" in method.__doc__:
        arguments.append([])
    if "dangling_surfaces" in method.__doc__:
        arguments.append([])
    return method(*arguments)


def test_methods_are_bound():
    for class_name, count in CLASSES.items():
        methods = unique_vertex_methods(getattr(inspection, class_name))
        check(len(methods) == count, f"{class_name}: {methods}")


def test_unique_vertex_index_is_checked():
    for model, inspectors in [
        (load_brep(), ["BRepTopologyInspector", "BRepInspector"]),
        (load_section(), ["SectionTopologyInspector", "SectionInspector"]),
    ]:
        for inspector_name in inspectors:
            inspector = getattr(inspection, inspector_name)(model)
            check(inspector._model is model, f"{inspector_name} remembers its model")
            prefix = "BRep" if "BRep" in inspector_name else "Section"
            for suffix in ["Corners", "Lines", "Surfaces", "Blocks"]:
                class_name = f"{prefix}{suffix}Topology"
                if not hasattr(inspection, class_name):
                    continue
                for name in unique_vertex_methods(getattr(inspection, class_name)):
                    for index in [model.nb_unique_vertices(), 2**32 - 1]:
                        try:
                            call_with_defaults(inspector, name, index)
                        except IndexError as error:
                            check(
                                "unique vertex" in str(error) and name in str(error),
                                f"{name}: {error}",
                            )
                        else:
                            raise ValueError(
                                f"[Test] {name}({index}) should raise IndexError"
                            )
                    call_with_defaults(inspector, name, model.nb_unique_vertices() - 1)
            try:
                inspector.corner_is_part_of_line_but_not_boundary(-1)
            except TypeError:
                pass
            else:
                raise ValueError("[Test] a negative index is rejected")
    # The component inspectors alone check too
    brep = load_brep()
    corners = inspection.BRepCornersTopology(brep)
    try:
        corners.brep_corner_topology_is_valid(brep.nb_unique_vertices())
    except IndexError:
        pass
    else:
        raise ValueError("[Test] IndexError expected")


def test_results_agree_with_the_inspection():
    brep = load_brep()
    inspector = inspection.BRepTopologyInspector(brep)
    result = inspector.inspect_brep_topology()
    indices = range(brep.nb_unique_vertices())
    multiple_corners = [
        uv for uv in indices if inspector.unique_vertex_has_multiple_corners(uv)
    ]
    check(
        len(multiple_corners)
        == result.corners.unique_vertices_linked_to_multiple_corners.nb_issues(),
        "unique vertices with several Corners",
    )
    not_corner = [uv for uv in indices if inspector.vertex_has_lines_but_is_not_a_corner(uv)]
    check(
        len(not_corner)
        == result.lines.unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner.nb_issues(),
        "unique vertices with Lines but no Corner",
    )
    two_blocks = [
        uv
        for uv in indices
        if inspector.unique_vertex_is_part_of_two_blocks_and_no_boundary_surface(uv)
    ]
    check(
        len(two_blocks)
        == result.blocks.unique_vertices_part_of_two_blocks_and_no_boundary_surface.nb_issues(),
        "unique vertices of two Blocks",
    )
    messages = [
        inspector.corner_is_not_internal_nor_boundary(uv) for uv in indices
    ]
    check(
        all(message is None or isinstance(message, str) for message in messages),
        "a message or None",
    )
    valid = [inspector.brep_corner_topology_is_valid(uv) for uv in indices]
    check(
        valid.count(False) >= len(multiple_corners),
        "a vertex with several Corners is not a valid Corner vertex",
    )
    for uv in multiple_corners[:1]:
        check(
            f"unique vertex {uv}" in inspector.unique_vertex_has_multiple_corners(uv),
            "the message names the unique vertex",
        )
        check(not inspector.brep_corner_topology_is_valid(uv), "invalid Corner vertex")


def test_component_checks():
    brep = load_brep()
    inspector = inspection.BRepTopologyInspector(brep)
    corner = next(iter(brep.corners()))
    line = next(iter(brep.lines()))
    surface = next(iter(brep.surfaces()))
    block = next(iter(brep.blocks()))
    check(inspector.corner_is_meshed(corner), "a Corner is meshed")
    check(inspector.line_is_meshed(line), "a Line is meshed")
    check(inspector.surface_is_meshed(surface), "a Surface is meshed")
    check(not inspector.block_is_meshed(block), "the Blocks of model_A1 have no mesh")
    check(
        inspector.corner_vertices_are_associated_to_unique_vertices(corner),
        "Corner vertices are linked",
    )
    check(
        inspector.line_vertices_are_associated_to_unique_vertices(line),
        "Line vertices are linked",
    )
    nb_edges = line.mesh().nb_edges()
    check(
        inspector.line_edge_has_wrong_component_edges_around(line, 0) is None
        or isinstance(inspector.line_edge_has_wrong_component_edges_around(line, 0), str),
        "message or None",
    )
    for function, component, size in [
        (inspector.line_edge_has_wrong_component_edges_around, line, nb_edges),
        (
            inspector.surface_facet_has_wrong_component_facets_around,
            surface,
            surface.mesh().nb_polygons(),
        ),
    ]:
        for index in [size, size + 10]:
            try:
                function(component, index)
            except IndexError as error:
                check(function.__name__ in str(error), str(error))
            else:
                raise ValueError(f"[Test] {function.__name__}({index}) IndexError")
    surface_message = inspector.surface_facet_has_wrong_component_facets_around(surface, 0)
    check(surface_message is None or isinstance(surface_message, str), "message or None")
    # Lists of uuids
    uuids = [surface.id()]
    for uv in range(0, brep.nb_unique_vertices(), 97):
        inspector.vertex_is_part_of_invalid_single_surface(uv, uuids)
        inspector.vertex_is_part_of_surface_with_wrong_relationships_to_block(
            uv, uuids, []
        )
        inspector.vertex_is_part_of_surface_with_wrong_relationships_to_block(
            uv, [], uuids
        )
    section = load_section()
    section_inspector = inspection.SectionTopologyInspector(section)
    check(
        section_inspector.corner_is_meshed(next(iter(section.corners()))),
        "a Section Corner is meshed",
    )
    check(
        section_inspector.surface_is_meshed(next(iter(section.surfaces()))),
        "a Section Surface is meshed",
    )
    section_result = section_inspector.inspect_section_topology()
    not_corner = [
        uv
        for uv in range(section.nb_unique_vertices())
        if section_inspector.vertex_has_lines_but_is_not_a_corner(uv)
    ]
    check(
        len(not_corner)
        == section_result.lines.unique_vertices_linked_to_several_lines_but_not_linked_to_a_corner.nb_issues(),
        "Section unique vertices with Lines but no Corner",
    )


if __name__ == "__main__":
    test_methods_are_bound()
    test_unique_vertex_index_is_checked()
    test_results_agree_with_the_inspection()
    test_component_checks()
