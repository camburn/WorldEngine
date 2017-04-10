#ifndef SHADER_INCLUDED
#define SHADER_INCLUDED
#define GLEW_STATIC
#include <Python.h>
#include <GL/glew.h>

char* loadFile(const char* filepath);
GLuint LoadShader(const char* file_path, GLuint ShaderID);
GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path);

#endif