/*
This is for the management of buffers to OpenGL
It manages data into OpenGL buffers and prepares it for drawing.
*/
#ifndef BUFFERS_INCLUDED
#define BUFFERS_INCLUDED
#define GLEW_STATIC
#include <assert.h>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "managers/data_models.hpp"

class TextureBuffer{
public:
	GLuint framebuffer;
	GLuint texture_id;
};

GLuint BufferMeshDataVNT(const GLfloat *mesh_data, int size);
GLuint BufferMeshDataVT(const GLfloat *mesh_data, int size);
GLuint BufferMeshDataV(vector<glm::vec3> vertices);
GLuint BufferMeshData(vector<glm::vec3> vertices, vector<unsigned int> indices, GLuint &vertex_buffer, GLuint &element_buffer);
void UpdateBufferData(vector<glm::vec3> vertices, vector<unsigned int> indices, GLuint vertex_buffer, GLuint element_buffer);
GLuint BufferTextureDataFromFile(string path, string directory = "./assets/textures");
TextureBuffer DepthMapBuffer(GLuint width, GLuint height);
TextureBuffer DepthCubeMapBuffer(GLuint width, GLuint height);
void activate_ssbo(const GLuint &id, const GLuint bind_number);
GLuint create_ssbo(GLuint &id, const GLuint &bind_number);
GLuint TextureBuffer3D(signed char * data, int size, int depth);
void read_ssbo(const GLuint &id, SharedState *read_state);
void update_ssbo(const GLuint &id, LightState &state);
void update_ssbo(const GLuint &id, SharedState &state);
#endif
