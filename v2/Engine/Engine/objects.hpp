#ifndef _OBJECTS_HPP
#define _OBJECTS_HPP

#include "Engine/entity.hpp"
#include "Engine/transform.hpp"
#include "Engine/renderer/camera.hpp"

namespace engine {

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
    const std::shared_ptr<Entity>& mesh() { return _mesh; }

    void attach_mesh(std::shared_ptr<Entity> mesh);

    virtual void update() {}; // Simulation step

    void draw_ui() {}; // Draw UI Elements for this object (immediate)

    void draw_deferred() {}; // What about shaders?
    // Only the renderer cares about shaders?

    std::string name;
private:
    Transform _transform;
    Type type {EMPTY};
    std::shared_ptr<Entity> _mesh {nullptr};
    std::shared_ptr<Camera> camera {nullptr};
    //std::shared_ptr<Light> entity nullptr;
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