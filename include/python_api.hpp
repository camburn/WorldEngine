#ifndef PYTHON_API_INCLUDED
#define PYTHON_API_INCLUDED
#include <Python.h>
#include <functional>
#include <iostream>
#include <string>
#include "graphics/debug.hpp"
#include "graphics/planes.hpp"

PyObject* PyInit_Debug(void);
PyObject* PyInit_Planes(void);

#endif
