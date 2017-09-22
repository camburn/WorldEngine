#include <vector>
#include <map>
#include <string>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/matrix_transform.hpp>
using glm::vec2;
using glm::vec3;

#include "graphics/buffers.hpp"
#include "graphics/mesh.hpp"

struct TexRect;

void InitPlanes();
void DrawPlanes(GLuint shader);
int UpdatePlaneBuffers(vec2 pos, float width, float height, string texture);
void SetTextures(string texture_name);
void SetTextureMap(string texture_name);
void CreatePlane(vec2 pos, float width, float height, 
    vector<Vertex>& vertex_data, vector<GLuint>& index_data, 
    int start_index, TexRect texture);