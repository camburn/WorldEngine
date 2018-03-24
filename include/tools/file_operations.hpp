#ifndef SHADER_INCLUDED
#define SHADER_INCLUDED
#define GLEW_STATIC
#include <stdexcept>
#include <Python.h>

void unload_shaders();
char* loadFile(const char* filepath);

#endif