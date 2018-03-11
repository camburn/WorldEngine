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

/*
Takes packed data in the form of:
vertex_x, vertex_y, vertex_z, color_r, color_g, color_b, texcoord_u, texcoord_v
and buffers it to the GPU for render

Returns:
GLuint - VAO ID
*/

enum class ATTRIBUTE_TYPE : unsigned short{
	VERTEX_DATA,
	NORMAL_DATA,
	TEXTURE_DATA
};


class Format {
	// This describes a Shaders attribute data 
public:
	ATTRIBUTE_TYPE type;
	GLenum data_type;
	Format();
	Format(ATTRIBUTE_TYPE format_type, GLenum format_data_type);
private: 

};


class DataFormat {
	// This describes the data accepted by the Buffer
public:
	const bool V3f;
	const bool N3f;
	const bool T2f;
	const bool Interleaved;
	const bool Indices;
	DataFormat(bool v3f, bool n3f, bool t2f, bool interleaved, bool indices);
private:

};

enum class INTERLEAVE_TYPE : unsigned short {
	V3 = 0x01,		//Vertex
	V3N3 = 0x02,		//VertexNormal
	V3N3T2 = 0x03		//VertexNormalTexture
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
	Shader();
	Shader(GLuint id);
	vector<Attribute> attributes;
private:

};


class Buffer {
	/*
	A generic data buffer capable of configuring any required data layouts.

	Acronyms:
	V - Vector data
	N - Normal data
	T - u,v texture data information
	I - Index data
	*/
public:
	Buffer(vector<float> buffer_data, DataFormat data_format);
private:
	bool has_vector_data;
	bool has_normal_data;
	bool has_texture_data;
	bool has_index_data;
	
	GLuint vao_id;
	GLuint vbo_id;
	GLuint ebo_id;
	void InitialiseBuffer();
	void SetAttributes(Shader s);
	void LoadVNT();
	void AddShader(GLuint shader_id);

	std::map<GLuint, GLuint> shader_attributes;
};

GLuint BufferMeshDataVNT(GLfloat *mesh_data, int size);
GLuint BufferMeshDataVT(GLfloat *mesh_data, int size);
GLuint BufferMeshDataV(vector<glm::vec3> vertices);
GLuint BufferMeshData(vector<glm::vec3> vertices, vector<unsigned int> indices, GLuint &vertex_buffer, GLuint &element_buffer);
void UpdateBufferData(vector<glm::vec3> vertices, vector<unsigned int> indices, GLuint vertex_buffer, GLuint element_buffer);
GLuint BufferTextureDataFromFile(string path, string directory = "./assets/textures");

#endif
