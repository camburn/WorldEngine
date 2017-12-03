#include "graphics/sprite_mesh.hpp"

// FIXME: This has gone full c++ we might want to convert this to C

/*
A mesh should have the following

vertex data
normal data
texture coordinate data
index data

Associated with a mesh are:

textures: one to many
 - diffuse map
 - bump map
 - specular map
 - et cetera 

A mesh also has a material (per mesh) that has the following attributes:
 - diffuse (a flat color)
 - specular
 - shininess
*/

SpriteMesh::SpriteMesh(int max_vertices, bool use_model_buffer, char* name) {
    vertices.reserve(max_vertices);
    indices.reserve(max_vertices);
    textures.reserve(max_vertices);
    this->max_vertices = max_vertices;
    this->v_offset = 0;
    this->i_offset = 0;
    this->m_offset = 0;
    this->i_size = 0;
    this->v_size = 0;
    this->m_size = 0;
    this->use_model_buffer = use_model_buffer;
    this->name = name;
}

void SpriteMesh::Init(GLuint sprite_program) {
    this->SetupMesh();
    this->SetBuffer();
    this->CreateUniformBlock(sprite_program);
}

int SpriteMesh::IndexCount() {
    return this->i_size;
}

int SpriteMesh::VertexCount() {
    return this->v_size;
}

void SpriteMesh::SetupMesh() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

    //Define our data for OpenGL
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), 
        (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), 
        (GLvoid*)offsetof(SpriteVertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(SpriteVertex), 
        (GLvoid*)offsetof(SpriteVertex, TexCoord));

    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, sizeof(SpriteVertex), 
        (GLvoid*)offsetof(SpriteVertex, ModelLoc));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SpriteMesh::BufferData() {
    // Buffer a fixed data source to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(SpriteVertex),
        &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
        &this->indices[0], GL_STATIC_DRAW);
        
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void SpriteMesh::SetBuffer() {
    // Set a buffer amount for update at a later point in time
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->max_vertices * sizeof(SpriteVertex),
        NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->max_vertices * sizeof(GLuint),
        NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int SpriteMesh::CreateUniformBlock(GLuint shader_id) {
    glGenBuffers(1, &this->UBO);
    GLuint bindingPoint = 1;  // This is coded into the shader
    GLuint blockIndex = glGetUniformBlockIndex(shader_id, "ModelMatrices");
    glUniformBlockBinding(shader_id, blockIndex, bindingPoint);

    GLuint MAX_MATRICES = 256;
    
    glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, this->UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * sizeof(GLfloat) * MAX_MATRICES,
        NULL, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return 0;
}

int SpriteMesh::UpdateModelMatrix(int start_bytes, glm::mat4 model_matrix) {
    glBindBuffer(GL_ARRAY_BUFFER, this->UBO);
    int s = sizeof(model_matrix);
    glBufferSubData(GL_ARRAY_BUFFER, start_bytes, sizeof(model_matrix), 
        &model_matrix[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return 0;
} 

int SpriteMesh::AppendData(vector<SpriteVertex> vertices, vector<GLuint> indices) {
    // Append data to this meshes buffer
    if (this->fixed) {
        return 1;
    }
    int data_size = vertices.size() * sizeof(SpriteVertex);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, this->v_offset, data_size, &vertices[0]);
    this->v_offset += data_size;

    data_size = indices.size() * sizeof(GLuint);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, this->i_offset, data_size, &indices[0]);
    int start_bytes = this->i_offset;
    this->i_offset += data_size;
    this->i_size += indices.size();
    this->v_size += vertices.size();

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return i_offset;
}

void SpriteMesh::Draw(GLuint shader) {
    GLuint diffuseNr = 1;
    GLuint specularNr = 1;
    for (GLuint i = 0; i < this->textures.size(); i++) {
        // Each bound texture requires its own texture unit
        glActiveTexture(GL_TEXTURE0 + i); // Activate the correct texture unit
        
        stringstream ss;
        string number;
        string name = this->textures[i].type;
        if (name == "texture_diffuse") {
            ss << diffuseNr++;
        }
        else if (name == "texture_specular") {
            ss << specularNr++;
        }
        number = ss.str();
        GLint tex_loc = glGetUniformLocation(shader, (name + number).c_str());
        if (tex_loc != -1) {
            glUniform1i(tex_loc, i);
            glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
        }
    }
    glActiveTexture(GL_TEXTURE0);
    //Draw the mesh
    glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
    glBindVertexArray(this->VAO);
    if (this->fixed) {
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawElements(GL_TRIANGLES, this->i_size, GL_UNSIGNED_INT, 0);
    }
    glUniform1i(glGetUniformLocation(shader, "use_model_buffer"), false);

    glBindVertexArray(0);
}