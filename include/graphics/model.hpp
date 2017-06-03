#pragma once

#define GLEW_STATIC
#include <vector>
#include <string>
#include <sstream>
using namespace std;
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "graphics/mesh.hpp"
#include "graphics/buffers.hpp"


//vector<Texture> textures_loaded;

class Model {
public:
    Model(GLchar* path, GLchar* filename);
    void Draw(GLuint shader);
private:
    vector<Mesh> meshes;
    string directory;
    vector<Texture> textures_loaded;

    int loadModel(string directory, string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, string typeName);
};