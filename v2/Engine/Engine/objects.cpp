#include "engine.hpp"
#include "Engine/objects.hpp"


namespace engine {

void Object::attach(std::shared_ptr<Entity> mesh) {
    if (_type != EMPTY && _type != MESH) {
        ENGINE_WARN("Cannot attach mesh to object as it is the wrong type");
        return;
    }
    _mesh = mesh;
    _type = MESH;
}

void Object::attach(std::shared_ptr<Light> light) {
    if (_type != EMPTY && _type != LIGHT) {
        ENGINE_WARN("Cannot attach mesh to object as it is the wrong type");
        return;
    }
    _light = light;
    _type = LIGHT;
}

}