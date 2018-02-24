#ifndef SHADER_INCLUDED
#define SHADER_INCLUDED
#define GLEW_STATIC
#include <stdexcept>
#include <Python.h>
#include <GL/glew.h>

void unload_shaders();
char* loadFile(const char* filepath);
GLuint LoadShader(const char* file_path, GLuint ShaderID);
GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path);
GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path);

#endif