#include "graphics/mesh.hpp"

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

Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    this->SetupMesh();
    this->BufferData();
    this->fixed = true;
}

Mesh::Mesh(int max_vertices, bool use_model_buffer, string name) {
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
    this->name = name.c_str();
}

void Mesh::Init() {
    this->SetupMesh();
    this->SetBuffer();
}

int Mesh::IndexCount() {
    return this->i_size;
}

int Mesh::VertexCount() {
    return this->v_size;
}

void Mesh::SetupMesh() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);

    //Define our data for OpenGL
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
        (GLvoid*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
        (GLvoid*)offsetof(Vertex, Normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), 
        (GLvoid*)offsetof(Vertex, TexCoord));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(GLuint), 
    (GLvoid*)offsetof(Vertex, TexCoord));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::BufferData() {
    // Buffer a fixed data source to the GPU
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
        &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
        &this->indices[0], GL_STATIC_DRAW);
        
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Mesh::SetBuffer() {
    // Set a buffer amount for update at a later point in time
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->max_vertices * sizeof(Vertex),
        NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->max_vertices * sizeof(GLuint),
        NULL, GL_STATIC_DRAW);

    if (this->use_model_buffer) {
        glBindBuffer(GL_ARRAY_BUFFER, this->MBO);
        glBufferData(GL_ARRAY_BUFFER, this->max_vertices * sizeof(GLuint),
            NULL, GL_DYNAMIC_DRAW);
    }
        
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

int Mesh::CreateUniformBlock(GLuint shader_id) {
    GLuint bindingPoint = 1;  // This is coded into the shader
    GLuint blockIndex = glGetUniformBlockIndex(shader_id, "ModelMatrices");
    glUniformBlockBinding(shader_id, blockIndex, bindingPoint);

    GLuint MAX_MATRICES = 16;
    glGenBuffers(1, &this->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, this->UBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * sizeof(GLfloat) * MAX_MATRICES,
        NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, this->UBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    return 0;
}

int Mesh::UpdateModelMatrix(int start_bytes, glm::mat4 model_matrix) {
    glBindBuffer(GL_ARRAY_BUFFER, this->UBO);
    glBufferSubData(GL_ARRAY_BUFFER, start_bytes, sizeof(model_matrix), 
        &model_matrix[0]);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return 0;
} 

int Mesh::AppendData(vector<Vertex> vertices, vector<GLuint> indices) {
    // Append data to this meshes buffer
    if (this->fixed) {
        return 1;
    }
    int data_size = vertices.size() * sizeof(Vertex);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferSubData(GL_ARRAY_BUFFER, this->v_offset, data_size, &vertices[0]);
    this->v_offset += data_size;

    data_size = indices.size() * sizeof(GLuint);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, this->i_offset, data_size, &indices[0]);
    this->i_offset += data_size;
    this->i_size += indices.size();
    this->v_size += vertices.size();

    if (this->use_model_buffer) {
        data_size = sizeof(glm::mat4);
        glBindBuffer(GL_ARRAY_BUFFER, this->MBO);
        glBufferSubData(GL_ARRAY_BUFFER, this->m_offset, data_size, &glm::mat4(1.0f)[0]);
        this->m_offset += data_size;
        this->m_size++;
    }

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return 0;
}

void Mesh::Draw(GLuint shader) {
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
    if (this->use_model_buffer) {
        GLuint n = glGetUniformLocation(shader, "use_model_buffer");
        glUniform1i(n, true);
    } else {
        glUniform1i(glGetUniformLocation(shader, "use_model_buffer"), false);
    }
    glBindVertexArray(this->VAO);
    if (this->fixed) {
        glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    } else {
        glDrawElements(GL_TRIANGLES, this->i_size, GL_UNSIGNED_INT, 0);
    }
    glUniform1i(glGetUniformLocation(shader, "use_model_buffer"), false);

    glBindVertexArray(0);
}