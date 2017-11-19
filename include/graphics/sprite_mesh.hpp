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


struct SpriteVertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoord;
    GLuint ModelLoc;
};

struct SpriteTexture {
    GLuint id;
    string type;
    aiString path;
};

class SpriteMesh {
public:
    vector<SpriteVertex> vertices;
    vector<GLuint> indices;
    vector<SpriteTexture> textures;

    SpriteMesh(int max_vertices, bool use_model_buffer=false, char* name="Default");
    void Init(GLuint sprite_program);
    int UpdateModelMatrix(int start_bytes, glm::mat4 model_matrix);
    int IndexCount();
    int VertexCount();
    int CreateUniformBlock(GLuint shader_id);
    int AppendData(vector<SpriteVertex> vertices, vector<GLuint> indices);
    void Draw(GLuint shader);
private:
    GLuint VAO, VBO, EBO, MBO, UBO;
    bool fixed = false;
    bool use_model_buffer = false;
    int max_vertices;
    int v_offset;
    int i_offset;
    int m_offset;
    int i_size;
    int v_size;
    int m_size;
    char* name;
    void SetupMesh();
    void BufferData();
    void SetBuffer();
};