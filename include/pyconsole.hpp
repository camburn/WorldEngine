#ifndef PYCONSOLE_INCLUDED
#define PYCONSOLE_INCLUDED
#include <functional>
#include <iostream>
#include <string>
#include <vector>
#include <Python.h>

#include "python_api.hpp"

void ConsoleInit();
const char *SendCommand(const char *command);
std::vector<const char*> CompleteCommand(const char *partial_command);
const char *GetSP();

#endif