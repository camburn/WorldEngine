#include "engine.hpp"
#include "Engine/objects.hpp"


namespace engine {

Transform Object::transform(Type type) {
    Transform t;
    switch(type) {
        case CAMERA:
            if (_type & CAMERA) {
                
            }

            break;

        case MESH:

            break;

        case LIGHT:

            break;

        case COLLIDER:
            if (_type & COLLIDER) {
                // Colliders do not have 
                glm::vec3 pos = _collider->transform.get_translation();
                
                t.set_translation(
                    _transform->get_translation() + _collider->transform.get_translation()
                );
                t.set_rotation(
                    _transform->get_rotation() * _collider->transform.get_rotation()
                );
                return t;
            }
            break;
            
        default:
            GL_NOOP;
    };
    return t;
}


void Object::attach(std::shared_ptr<Entity> mesh) {
    if (_type & MESH) {
        ENGINE_WARN("Cannot attach mesh to object as it already has a mesh");
        return;
    }
    if (_type & LIGHT) {
        ENGINE_WARN("Cannot attach mesh to object as it already has an incompatable attachment (Light)");
        return;
    }
    _mesh = mesh;
    _type = _type | MESH;
}

void Object::attach(std::shared_ptr<Light> light) {
    if (_type & LIGHT) {
        ENGINE_WARN("Cannot attach light to object as it already has a light");
        return;
    }
    if (_type & MESH) {
        ENGINE_WARN("Cannot attach mesh to object as it already has an incompatable attachment (Mesh)");
        return;
    }
    _light = light;
    _type = _type | LIGHT;
}

void Object::attach(std::shared_ptr<Script> script) {
    if (_type & SCRIPT) {
        ENGINE_WARN("Cannot attach script to object as it already has one");
        return;
    }
    _script = script;
    _type = _type | SCRIPT;
}

void Object::attach(std::shared_ptr<Collider> collider) {
    if (_type & COLLIDER) {
        ENGINE_WARN("Cannot attach collider to object as it already has one");
        return;
    }
    _collider = collider;
    _collider->transform.set_parent(_transform);
    _type = _type | COLLIDER;
}

}