#include "graphics/buffers.hpp"

std::map<GLuint, Shader> shader_map;
//shader_attribute_name	// Local data format
std::map<std::string, Format> shader_attr_format;

//Format format(ATTRIBUTE_TYPE::VERTEX_DATA, GL_FLOAT_VEC3);
//shader_attr_format["position"] = format;
//shader_attr_format.insert(
//	std::make_pair<std::string, Format>("position", Format(ATTRIBUTE_TYPE::VERTEX_DATA, GL_FLOAT_VEC3))
//);

//shader_attr_format["position"] = Format{ATTRIBUTE_TYPE::VERTEX_DATA, GL_FLOAT_VEC3};
//shader_attr_format["normal"] = Format(ATTRIBUTE_TYPE::NORMAL_DATA, GL_FLOAT_VEC3);
//shader_attr_format["texCoord"] = Format(ATTRIBUTE_TYPE::TEXTURE_DATA, GL_FLOAT_VEC2);


GLuint BufferMeshDataVNT(GLfloat *mesh_data, int size) {
	// Create our VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Buffer data to VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, size, mesh_data, GL_STATIC_DRAW);
	// Specify the vertex position attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
	// Specify the vertex normal attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
	// Specify the vertex texture coordinate attributes
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(6 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0); 
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(2);
	return VertexArrayID;
}

GLuint BufferMeshDataVT(GLfloat *mesh_data, int size) {
	// Create our VAO
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Buffer data to VBO
	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, size, mesh_data, GL_STATIC_DRAW);
	// Specify the vertex position attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
	// Specify the vertex texture coordinate attributes
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	return VertexArrayID;
}

GLuint BufferMeshDataV(vector<glm::vec3> vertices) {
	// Create our VAO
	GLuint VertexArrayID;
	
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	// Buffer data to VBO
	GLuint vertexbuffer;
	//GLuint elementBuffer;
	//glGenBuffers(1, &elementBuffer);
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint),
    //    &indices[0], GL_STATIC_DRAW);
	// Specify the vertex position attributes
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	return VertexArrayID;
}

GLuint BufferMeshData(vector<glm::vec3> vertices, vector<unsigned int> indices, GLuint &vertex_buffer, GLuint &element_buffer) {
	// Create our VAO
	GLuint VertexArrayID;
	
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Buffer data to VBO
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);	

	//GLuint elementBuffer;
	glGenBuffers(1, &element_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDisableVertexAttribArray(0);
	return VertexArrayID;
}

void UpdateBufferData(vector<glm::vec3> vertices, vector<unsigned int> indices, GLuint vertex_buffer, GLuint element_buffer) {
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
}

GLuint BufferTextureDataFromFile(string file, string directory) {
    string filename = directory + file;
    replace(filename.begin(), filename.end(), '\\', '/');
    printf("INFO::IMAGES:: Loading texture: %s\n:", filename.c_str());
    // Load Textures
    int tex_w, tex_h;
    unsigned char* image = SOIL_load_image(filename.c_str(), &tex_w, &tex_h, 0, SOIL_LOAD_RGB);

    if (!image) {
        printf("ERROR::IMAGES:: Failed to load texture %s\n", filename.c_str());
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_w, tex_h, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    SOIL_free_image_data(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    return textureID;
}

GLuint ShadowMapBuffer() {
    GLuint depthMapFBO;

    glGenFramebuffers(1, &depthMapFBO);

    const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return depthMap;
}

/*
V = Vertex
N = Normal
C = Color
T = u,v Coordinates
I = Indices
*/

Shader::Shader() : id(-1) {}

Shader::Shader(GLuint id) : id(id) {
	GLint i;
	GLint count;

	GLint size; // size of the variable
	GLenum type; // type of the variable (float, vec3 or mat4, etc)

	const GLsizei bufSize = 16; // maximum name length
	GLchar name[bufSize]; // variable name in GLSL
	GLsizei length; // name length

	std::cout << "Checking Attributes" << std::endl;
	glGetProgramiv(id, GL_ACTIVE_ATTRIBUTES, &count);
	std::cout << "Active Attributes: " << count << std::endl;
	attributes.reserve(3);

	for (i = 0; i < count; i++)
	{
		glGetActiveAttrib(id, (GLuint)i, bufSize, &length, &size, &type, name);
		Attribute attribute(this, i, name, type);
		attributes.push_back(attribute);
			
	}
	/*
	std::cout << "GL_FLOAT: " << GL_FLOAT << std::endl;
	std::cout << "GL_FLOAT_VEC3: " << GL_FLOAT_VEC3 << std::endl;
	std::cout << "GL_FLOAT_VEC2: " << GL_FLOAT_VEC2 << std::endl;
	std::cout << "GL_FLOAT_VEC4: " << GL_FLOAT_VEC4 << std::endl;*/
}


DataFormat::DataFormat(bool v3f=false, bool n3f=false, bool t2f=false, bool interleaved=true, bool indices=true)
	: V3f(v3f), N3f(n3f), T2f(t2f), Interleaved(interleaved), Indices(indices) {

}

Format::Format() {}

Format::Format(ATTRIBUTE_TYPE type, GLenum data_type) : type(type), data_type(data_type) {

}


Attribute::Attribute(Shader *s, GLint i, string n, GLenum t) : shader(s), id(i), name(n), type(t) {
	std::cout << "Attribute # " << id << " Type: " << type << " Name: " <<  name << std::endl;
}


Buffer::Buffer(vector<float> buffer_data, DataFormat data_format) {
	// Steps ... 
	// Set up VAO
	// Create VBO
	// How many do I need?
	if (data_format.Interleaved) {
		GLuint temp;
		glGenBuffers(1, &temp);
		vbo_id = temp;
	} else {
		// 1 VBO per data
		// TODO
		std::cout << "ERROR:: BUFFERS:: Data not interlaved not implemented" << std::endl;
	}

	if (data_format.Indices) {
		glGenBuffers(1, &ebo_id);
	}

	// Set VBO Attributes
}

void Buffer::InitialiseBuffer(){
	// When buffer is initialiased it will set the data types it supports
	// i.e. VNT, VN, ...
	// You cannot change data types later, need new buffer


	


	//glBindVertexArray(VertexArrayID);0
	//glGenBuffers();
}

void Buffer::AddShader(GLuint shader_id) {
	assert(shader_map.count(shader_id) == 0);
	Shader s = shader_map[shader_id];

	glGenVertexArrays(1, &vao_id);
	shader_attributes[shader_id] = vao_id;

	glBindVertexArray(vao_id);
	SetAttributes(s);
	glBindVertexArray(0);
}


void Buffer::SetAttributes(Shader s) {
	// What do I need?
	// For each data type supported by this buffer and shader combo
	// Create a VAO and set the attributes, bind the right buffers.

	// I need to match up shader attributes to our buffers data format

	// VAO per shader
	for (Attribute attribute : s.attributes) {
		
		assert(shader_attr_format.count(attribute.name) > 0 && "No mapping exists for shader attribute and data format");
		// What is this type?
		// Do we have a mapping?
		Format format = shader_attr_format[attribute.name];

		// Does our Buffer have data for this format?


	}

}
