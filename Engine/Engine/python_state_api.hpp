#ifndef _PYTHON_STATE_API_HPP
#define _PYTHON_STATE_API_HPP

#include <Python.h>
#include <structmember.h>
#include <string>

namespace engine {

PyMODINIT_FUNC PyInit_input(void);

} // End namespace

#endif