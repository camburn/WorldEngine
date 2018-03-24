#include <glm/glm.hpp>
#include "graphics/renderer.hpp"

class Object {
public:
    Object();
private:
    glm::vec3 position;

    DrawObject prim;
    ModelObject model;
    
}