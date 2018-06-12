
# PyEngine

Welcome to the world of the PyEngine! A C++ OpenGL engine written with an embedded Python
interpreter.

The goals of this is to be able to dynamically manage your engine with powerful Python commands.
It also enables game logic to the written in Python.

## Building

PyEngine uses the SCons build tool. To build a python environment should be configued with scons installed.

The virtualenv requires an up to date version of pip, setuptools, pyyaml, and wheel before scons can be isntalled.

### Running Build Environment

Before building the following environment variables should be set:

``` bash
EXTERNAL_LIBS  # Directory containing the external dependencies libs and headers

# Windows Only
PYTHON_PATH    # Directory containing the Python 3.6 libs and headers
WIN_KIT_DIR    # Directory containing the Windows 10 development kit
```

#### Linux

``` bash
python3 -m venv scons_env
source scons_env/bin/activate
pip install -U pip setuptools wheel pyyaml
pip install scons
cd /path/to/project/dir

scons
```

#### Windows

``` cmd
py -3 -m venv scons_env
./scons_env/Scripts/activate.bat
python -m pip install --upgrade pip setuptools wheel pyyaml
python -m pip install scons
cd /path/to/project/dir

scons.py
```

### Dependencies

PyEngine contains the following dependencies:

- assimp
- freetype
- glew
- glfw
- glm
- shapelib
- soil

Most of these libraries are compiled and linked against statically. A few are
header only (glm). A bundled zip can be downloaded that contains all the libraries see the
[External Libraries](https://github.com/camburn/WorldEngine/releases/tag/external_libs)
release. On linux (Ubuntu) most dependencies are available as apt packages.

In addition the following Packages are required:

``` bash
# All
Python 3.6 with built libs and headers (recommend debug libs as well)

# Windows Only:
Windows 10 Development Kit (10.0.14393.0)

# Linux Only:
G++
```