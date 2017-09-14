#include <vector>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/matrix_transform.hpp>
using glm::vec2;
using glm::vec3;

#include "graphics/buffers.hpp"
#include "graphics/mesh.hpp"

Mesh InitPlanes();
void DrawPlanes(GLuint shader);
int UpdatePlaneBuffers(Mesh plane_mesh);
void CreatePlane(vec2 pos, float width, float height, 
    vector<Vertex>& vertex_data, vector<GLuint>& index_data);