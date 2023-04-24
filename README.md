
# PyEngine

Welcome to the world of the PyEngine! A C++ OpenGL engine written with an
embedded Python interpreter.

The goals of this is to be able to dynamically manage your engine with powerful
Python commands. It also enables game logic to the written in Python.

## Building

PyEngine uses the premake build tool to generate appropriate make, msbuild
projects.

Requirements:

- Premake5
- Git LFS

### Running Build Environment

Before building the following environment variables should be set:

``` bash
# Windows Only
PYTHON_PATH    # Directory containing the Python 3.7 libs and headers
```

To setup build environment

``` bash
# Clone main project
git clone https://github.com/camburn/WorldEngine.git
# Clone third party projects
git submodule update --init --recursive
```

#### Linux

``` bash
premake5 gmake 2 --opengl2
make
```

#### Windows

``` cmd
premake5 gmake 2 --opengl3
msbuild WorldEngine.sln
```
