#pragma once
#include <vector>
#include <map>
#include <string>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "imgui.h"
using glm::vec2;
using glm::vec3;
using glm::mat3;
using glm::mat4;

#include "graphics/buffers.hpp"
#include "graphics/sprite_mesh.hpp"

struct TexRect;

void DisplayPlaneData(bool* p_open);
void InitPlanes(GLuint sprite_program);
void DrawPlanes(GLuint shader);
void UpdateMatrixBuffer();
glm::mat4 GetPlaneMatrix(string name);
void TranslatePlaneMatrix(string name, float x, float y, float z);
int UpdatePlaneMatrix(string name, mat4 model_matrix);
int UpdatePlaneBuffers(float width, float height, string texture, vec2 pos=vec2(0, 0),  int model_loc=0);
void CreateDynamicPlane(vec2 pos, float width, float height, string texture, string name);
void SetTextures(string texture_name);
void SetTextureMap(string texture_name);
void CreatePlane(vec2 pos, float width, float height, 
    vector<SpriteVertex>& vertex_data, vector<GLuint>& index_data, 
    int start_index, TexRect texture, int model_loc);