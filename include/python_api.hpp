#ifndef PYTHON_API_INCLUDED
#define PYTHON_API_INCLUDED
#include <Python.h>
#include <functional>
#include <iostream>
#include <string>

PyObject* PyInit_emb(void);
typedef std::function<void(std::string)> stdout_write_type;

#endif