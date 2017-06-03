/*
This is the mesh class for creation and storage of meshes
*/
#pragma once

#define GLEW_STATIC
#include <vector>
#include <string>
#include <sstream>
using namespace std;
#include <stdio.h>
#include <assimp/scene.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
};

struct Texture {
    GLuint id;
    string type;
    aiString path;
};

class Mesh {
public:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    vector<Texture> textures;

    Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures);
    void Draw(GLuint shader);
private:
    GLuint VAO, VBO, EBO;
    void setupMesh();
};