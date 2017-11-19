#include <vector>
#include <map>
#include <string>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/matrix_transform.hpp>
using glm::vec2;
using glm::vec3;
using glm::mat3;
using glm::mat4;

#include "graphics/buffers.hpp"
#include "graphics/sprite_mesh.hpp"

struct TexRect;

void InitPlanes(GLuint sprite_program);
void DrawPlanes(GLuint shader);
void UpdateMatrixBuffer();
glm::mat4 GetPlaneMatrix(string name);
int UpdatePlaneMatrix(string name, mat4 model_matrix);
int UpdatePlaneBuffers(vec2 pos, float width, float height, string texture, int model_loc);
void CreateDynamicPlane(vec2 pos, float width, float height, string texture, string name);
void SetTextures(string texture_name);
void SetTextureMap(string texture_name);
void CreatePlane(vec2 pos, float width, float height, 
    vector<SpriteVertex>& vertex_data, vector<GLuint>& index_data, 
    int start_index, TexRect texture, int model_loc);