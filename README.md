<h1 align="center">OpenGeode-Inspector<sup><i>by Geode-solutions</i></sup></h1>
<h3 align="center">OpenGeode module for inspecting meshes and models</h3>

<p align="center">
  <img src="https://github.com/Geode-solutions/OpenGeode-Inspector/actions/workflows/schedule.yml/badge.svg" alt="Build Status">
  <img src="https://github.com/Geode-solutions/OpenGeode-Inspector/actions/workflows/deploy.yml/badge.svg" alt="Deploy Status">
  <img src="https://img.shields.io/github/release/Geode-solutions/OpenGeode-Inspector.svg" alt="Version">
  <img src="https://img.shields.io/pypi/v/opengeode-inspector" alt="PyPI" >
</p>

<p align="center">
  <img src="https://img.shields.io/static/v1?label=Windows&logo=windows&logoColor=white&message=support&color=success" alt="Windows support">
  <img src="https://img.shields.io/static/v1?label=Ubuntu&logo=Ubuntu&logoColor=white&message=support&color=success" alt="Ubuntu support">
  <img src="https://img.shields.io/static/v1?label=Red%20Hat&logo=Red-Hat&logoColor=white&message=support&color=success" alt="Red Hat support">
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C%2B%2B-17-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/license-MIT-blue.svg" alt="License">
  <img src="https://img.shields.io/badge/%20%20%F0%9F%93%A6%F0%9F%9A%80-semantic--release-e10079.svg" alt="Semantic-release">
</p>

<p align="center">
  <a href="https://geode-solutions.com/#slack">
    <img src="https://img.icons8.com/?size=20&id=kikR2jIn6485&format=png&color=000000" alt="Slack invite">
  </a>
  <a href="https://www.linkedin.com/company/geode-solutions">
    <img src="https://img.icons8.com/?size=25&id=13930&format=png&color=000000" alt="Linkedin">
  </a>
  <a href="mailto:contact@geode-solutions.com">
    <img src="https://img.icons8.com/?size=24&id=YRRhCXfA0Vd0&format=png&color=999999" alt="Mail Us">
  </a>
</p>

---

## Introduction

OpenGeode-Inspector is a module of [OpenGeode] providing ways of inspecting your meshes and models and verifying their validity.

[OpenGeode]: https://github.com/Geode-solutions/OpenGeode

## Documentation

Go check out the online documentation at [docs.geode-solutions.com].

[docs.geode-solutions.com] https://docs.geode-solutions.com

Installing OpenGeode-Inspector is done:

- either by compiling the C++ source.
- or by installing the python library using the pip command `pip install opengeode-inspector`.

## Usage

To use OpenGeode-Inspector, several options are available:

- If you installed and compiled the C++ source code, you can use the executable binaries to apply an inspection of your meshes/models and toggle on/off the various checks directly.
- or use the API functions (check the tests to see how it is done) if you want to go further or use the resulting errors to repair your meshes/models.
- If you installed the python library, you can add `import opengeode_inspector` in your Python script to use the available API functions. Check [this documentation page](https://docs.geode-solutions.com/guides/use-opengeode-binding.html) for more details. Examples are also procured in the `examples` folder.
- You can inspect your models without any installation, by using the API of the [Geode-solutions free tools](https://geode-solutions.com/tools).

The available checks for each mesh type are:

- PointSet:
  - Colocation of vertices
- EdgedCurve:
  - Colocation of vertices
  - Degeneration of edges
- SurfaceMesh:
  - Adjacency of polygons
  - Colocation of vertices
  - Degeneration of edges
  - Degeneration of polygons
  - Intersection of triangles (for triangulated surfaces)
  - Manifold of vertices
  - Manifold of edges
- SolidMesh:
  - Adjacency of polyhedra
  - Colocation of vertices
  - Degeneration of edges
  - Degeneration of polyhedra
  - Manifold of vertices
  - Manifold of edges
  - Manifold of facets
    The available checks for each model type are:
- Section:
  - Validity of the topology
  - Checks on each component mesh: all the previous mesh checks depending on the component mesh type
  - Checks on the validity of the unique vertices (linking to vertices, colocation of unique vertices points, un-colocation of points with same unique vertices)
  - Intersection of the component mesh surfaces between each other
- BRep:
  - Validity of the topology
  - Checks on each component mesh: all the previous mesh checks depending on the component mesh type
  - Checks on the validity of the unique vertices (linking to vertices, colocation of unique vertices points, un-colocation of points with same unique vertices)
  - Intersection of the component mesh surfaces between each other

## Questions

For questions and support please use the official [slack](https://opengeode-slack-invite.herokuapp.com) and go to the channel #inspector. The issue list of this repo is exclusively for bug reports and feature requests.

## License

[MIT](https://opensource.org/licenses/MIT)

Copyright (c) 2019 - 2026, Geode-solutions
