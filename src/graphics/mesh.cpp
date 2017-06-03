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
    this->setupMesh();
}

void Mesh::setupMesh() {
    glGenVertexArrays(1, &this->VAO);
    glGenBuffers(1, &this->VBO);
    glGenBuffers(1, &this->EBO);

    glBindVertexArray(this->VAO);
    glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
    glBufferData(GL_ARRAY_BUFFER, this->vertices.size() * sizeof(Vertex),
        &this->vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->indices.size() * sizeof(GLuint),
        &this->indices[0], GL_STATIC_DRAW);

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

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
