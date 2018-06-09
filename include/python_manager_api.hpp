#ifndef PYTHON_MANAGER_API_INCLUDED
#define PYTHON_MANAGER_API_INCLUDED
#include <Python.h>
#include <functional>
#include <iostream>
#include <string>
#include "tracker.hpp"
#include "graphics/debug.hpp"
#include "graphics/planes.hpp"
#include "graphics/console.hpp"

#include "managers/state_manager.hpp"
#include "managers/instance_manager.hpp"

PyObject* PyInit_manager(void);
void assign_managers(State &state, InstanceManager &instances);

#endif
