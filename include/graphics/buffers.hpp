/*
This is for the management of buffers to OpenGL
It manages data into OpenGL buffers and prepares it for drawing.
*/
#ifndef BUFFERS_INCLUDED
#define BUFFERS_INCLUDED
#define GLEW_STATIC
#include <string>
using namespace std;
#include <stdio.h>
#include <SOIL.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*
Takes packed data in the form of:
vertex_x, vertex_y, vertex_z, color_r, color_g, color_b, texcoord_u, texcoord_v
and buffers it to the GPU for render

Returns:
GLuint - VAO ID
*/
GLuint BufferMeshDataVNT(GLfloat *mesh_data, int size);
GLuint BufferMeshDataVT(GLfloat *mesh_data, int size);
GLuint BufferTextureDataFromFile(string path, string directory = "./assets/textures");

#endif