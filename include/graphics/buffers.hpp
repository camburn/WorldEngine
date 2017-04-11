/*
This is for the management of meshes
*/
#ifndef BUFFERS_INCLUDED
#define BUFFERS_INCLUDED
#define GLEW_STATIC
#include <stdio.h>
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

#endif