#ifndef PYTHON_API_INCLUDED
#define PYTHON_API_INCLUDED
#include <Python.h>
#include <functional>
#include <iostream>
#include <string>
#include "tracker.hpp"
#include "graphics/debug.hpp"
#include "graphics/planes.hpp"
#include "graphics/console.hpp"

PyObject* PyInit_Debug(void);
PyObject* PyInit_Planes(void);
PyObject* PyInit_console_engine(void);

#endif
