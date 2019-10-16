#include "engine.hpp"
#include "Engine/objects.hpp"


namespace engine {

void Object::attach_mesh(std::shared_ptr<Entity> mesh) {
    if (type != EMPTY || type != MESH) {
        ENGINE_WARN("Cannot attach mesh to object as it is the wrong type");
        return;
    }
    _mesh = mesh;
    type = MESH;
}

}