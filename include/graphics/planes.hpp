#include <vector>
using namespace std;

#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/matrix_transform.hpp>
using glm::vec2;
using glm::vec3;

#include "graphics/buffers.hpp"

void DrawPlanes();
int UpdatePlaneBuffers();
void CreatePlane(vec2 pos, float width, float height);