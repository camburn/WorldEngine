#ifndef _OBJECTS_HPP
#define _OBJECTS_HPP

#include "Engine/entity.hpp"
#include "Engine/transform.hpp"
//#include "Engine/scripts.hpp"
#include "Engine/renderer/camera.hpp"
#include "Engine/renderer/lights.hpp"

namespace engine {

class Script;

class Object {
    /* Base common object */
public:
    Object() {}
    ~Object() {}

    enum Type {
        CAMERA,
        MESH,
        LIGHT,
        EMPTY
    };

    Transform& transform() { return _transform; }
    std::shared_ptr<Entity>& mesh() { return _mesh; }
    std::shared_ptr<Script>& script() { return _script; }
    Light& light() { return *_light; }

    void attach(std::shared_ptr<Entity> mesh);
    void attach(std::shared_ptr<Light> light);
    void attach(std::shared_ptr<Script> script);

    virtual void update() {}; // Simulation step

    void draw_ui() {}; // Draw UI Elements for this object (immediate)

    void draw_deferred() {}; // What about shaders?
    // Only the renderer cares about shaders?

    const Type type() { return _type;}

    std::string name;
private:
    Transform _transform {};
    Type _type {EMPTY};
    std::shared_ptr<Entity> _mesh {nullptr};
    std::shared_ptr<Camera> camera {nullptr};
    std::shared_ptr<Light> _light {nullptr};
    std::shared_ptr<Script> _script {nullptr};
};


} // Namspace

#endif
/*
Mesh mesh;
Object obj;
obj.attach_mesh(mesh);

obj.transform().set_translation(glm::vec3(5, 0, 0));

obj.draw_deferred();
// Submit the components of object that can be drawn for drawing at a later
// point in time

// This will submit any meshes, editor ui components, or other components to be
// drawn in bulk. This will later allow drawing optimisations as required.
// (batch processing through glMultiDrawElemnts or glDrawElementsInstance)

// it will also allow threading of the object representation vs the rendererer drawing
// How do we send the transform data to the mesh instance?

*/