#include "graphics/buffers.hpp"

#define SSBO_BINDING_UNIFORMS 3
#define SSBO_DEFAULT_SIZE 4096*1024  //4MB

GLuint BufferMeshDataVNT(const GLfloat *mesh_data, int size) {
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

GLuint BufferMeshDataVT(const GLfloat *mesh_data, int size) {
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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);

    //GLuint elementBuffer;
    glGenBuffers(1, &element_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(0);
    return VertexArrayID;
}

void UpdateBufferData(vector<glm::vec3> vertices, vector<unsigned int> indices, GLuint vertex_buffer, GLuint element_buffer) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), &vertices[0], GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_DYNAMIC_DRAW);
}

GLuint BufferTextureDataFromFile(string file, string directory) {
    string filename = directory + file;
    replace(filename.begin(), filename.end(), '\\', '/');
    printf(":INFO::IMAGES:: Loading texture: %s\n:", filename.c_str());
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

TextureBuffer DepthMapBuffer(GLuint width=1024, GLuint height=1024) {
    GLuint depthMapFBO;

    glGenFramebuffers(1, &depthMapFBO);

    GLuint SHADOW_WIDTH = width, SHADOW_HEIGHT = height;
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
        SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D,
        depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    TextureBuffer texture_buffer {depthMapFBO, depthMap};
    return texture_buffer;
}

TextureBuffer DepthCubeMapBuffer(GLuint width=1024, GLuint height=1024) {
    GLuint depthMapFBO;

    glGenFramebuffers(1, &depthMapFBO);

    GLuint SHADOW_WIDTH = width, SHADOW_HEIGHT = height;
    GLuint depthMap;
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthMap);
    for (unsigned int i = 0; i < 6; ++i) { 
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT,
            SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    TextureBuffer texture_buffer {depthMapFBO, depthMap};
    return texture_buffer;
}

static GLuint ssbo_id = -1;

void activate_ssbo() {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_BINDING_UNIFORMS, ssbo_id);
}

GLuint _create_ssbo(int size) {
    glGenBuffers(1, &ssbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
    glBufferData(GL_SHADER_STORAGE_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SSBO_BINDING_UNIFORMS, ssbo_id);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    return ssbo_id;
};

GLuint create_ssbo() {
    return _create_ssbo(SSBO_DEFAULT_SIZE);
}

GLuint create_ssbo(int size) {
    return _create_ssbo(size);
}

void update_ssbo(float uniform_data[], int uniform_size) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
    GLvoid *ptr_ssbo_map = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    memcpy(ptr_ssbo_map, &uniform_data, uniform_size);
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void update_ssbo(SharedState state) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
    //GLvoid *ptr_ssbo_map = glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(state), &state, GL_STATIC_DRAW);
    //memcpy(ptr_ssbo_map, &state, sizeof(state));
    //glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void read_ssbo(SharedState *read_state) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_id);
    SharedState *ptr_ssbo_map = (SharedState *)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_ONLY);
    memcpy(read_state, ptr_ssbo_map, sizeof(SharedState));
    glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    //read_state = &data;
}
