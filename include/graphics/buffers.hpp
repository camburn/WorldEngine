/*
This is for the management of buffers to OpenGL
It manages data into OpenGL buffers and prepares it for drawing.
*/
#ifndef BUFFERS_INCLUDED
#define BUFFERS_INCLUDED
#define GLEW_STATIC
#include <algorithm>
#include <string>
#include <vector>
#include <iostream>
using namespace std;
#include <stdio.h>
#include <SOIL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

/*
Takes packed data in the form of:
vertex_x, vertex_y, vertex_z, color_r, color_g, color_b, texcoord_u, texcoord_v
and buffers it to the GPU for render

Returns:
GLuint - VAO ID
*/

enum class INTERLEAVE_TYPE : unsigned short {
	V = 0,
	VN = 1,
	VNT = 2
};

class Shader;

class Attribute {
	// A GLSL Attribute
public:
	
	const GLint id;
	const string name;
	const GLenum type;
	Attribute(Shader *s, GLint i, std::string n, GLenum t);
private:
    Shader *shader;

};

class Shader {
	//An OpenGL Shader
public:
	const GLuint id;
	Shader::Shader(GLuint shader_id);
private:
    vector<Attribute> attributes;
};

GLuint BufferMeshDataVNT(GLfloat *mesh_data, int size);
GLuint BufferMeshDataVT(GLfloat *mesh_data, int size);
GLuint BufferMeshDataV(vector<glm::vec3> vertices);
GLuint BufferMeshData(vector<glm::vec3> vertices, vector<unsigned int> indices, GLuint &vertex_buffer, GLuint &element_buffer);
void UpdateBufferData(vector<glm::vec3> vertices, vector<unsigned int> indices, GLuint vertex_buffer, GLuint element_buffer);
GLuint BufferTextureDataFromFile(string path, string directory = "./assets/textures");

#endif
