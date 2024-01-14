#ifndef _OBJECTS_HPP
#define _OBJECTS_HPP

#include "Engine/entity.hpp"
#include "Engine/transform.hpp"
//#include "Engine/scripts.hpp"
#include "Engine/renderer/camera.hpp"
#include "Engine/renderer/lights.hpp"
#include "Engine/physics/collision.hpp"

namespace engine {

class Script;

class Object {
    /* Base common object */
public:
    Object() {}
    ~Object() {}

    enum Type : char {
        EMPTY = 1 << 0,
        CAMERA = 1 << 1,
        MESH = 1 << 2,
        LIGHT = 1 << 3,
        SCRIPT = 1 << 5,
        COLLIDER = 1 << 6
    };

    Transform& transform() { return *_transform; }
    std::shared_ptr<Transform>& transform_reference() { return _transform; }
    Transform transform(Type type);
    std::shared_ptr<Entity>& mesh() { return _mesh; }
    std::shared_ptr<Script>& script() { return _script; }
    Light& light() { return *_light; }
    std::shared_ptr<Light>& light_reference() { return _light; }
    std::shared_ptr<Collider>& collider() { return _collider; }

    void attach(std::shared_ptr<Entity> mesh);
    void attach(std::shared_ptr<Light> light);
    void attach(std::shared_ptr<Script> script);
    void attach(std::shared_ptr<Collider> collider);


    virtual void update() {}; // Simulation step

    void draw_ui() {}; // Draw UI Elements for this object (immediate)

    void draw_deferred() {}; // What about shaders?
    // Only the renderer cares about shaders?

    const bool attached(Type attachment_type) { return attachment_type & _type; }

    void set_transform(Transform t) { *_transform = t; }

    std::string name;
private:
    
    Type _type {EMPTY};
    std::shared_ptr<Transform> _transform {new Transform{} };
    std::shared_ptr<Entity> _mesh {nullptr};
    std::shared_ptr<Camera> camera {nullptr};
    std::shared_ptr<Light> _light {nullptr};
    std::shared_ptr<Script> _script {nullptr};
    std::shared_ptr<Collider> _collider {nullptr};
};

inline Object::Type operator|(Object::Type a, Object::Type b) {
    return static_cast<Object::Type>(static_cast<char>(a) | static_cast<char>(b));
}
inline Object::Type operator&(Object::Type a, Object::Type b) {
    return static_cast<Object::Type>(static_cast<char>(a) & static_cast<char>(b));
}


} // Namspace

#endif
