#ifndef _COLLISION_HPP
#define _COLLISION_HPP

#include <glm/glm.hpp>
#include "Engine/physics/raycast.hpp"
#include "Engine/transform.hpp"
#include "Engine/renderer/debug_draw.hpp"

namespace engine {

class Collider {
public:
    Collider(){}
    ~Collider(){}

    virtual std::vector<glm::vec4> points() = 0;
    virtual RayHit intersect(Ray ray) = 0;

    glm::vec4 debug_color {0.3f, 1.0f, 0.3f, 1.0f};
    bool debug_draw_enabled = false;
    Transform transform;
private:

};

class AABBCollider: public Collider {
public:
    // This box collider is an AABB (axis aligned bounding box)
    AABBCollider();

    AABBCollider(glm::vec3 position, glm::vec3 size);

    RayHit intersect(Ray ray) override;
    std::vector<glm::vec4> points() override;

    glm::vec3 position;
    glm::vec3 size;
    glm::vec3 min_corner;
    glm::vec3 max_corner;
private:
    void calculate_minimals();
};

class BoxCollider: public Collider {
public:
    // This box collider is an OBB (oriented bounding box)
    // It does not support scaling on its transformation matrix
    // TODO: Fix scaling being set on transform
    // It should only inherit a position and rotation from parent
    BoxCollider();

    BoxCollider(glm::vec3 position, glm::vec3 size);

    RayHit intersect(Ray ray) override;
    std::vector<glm::vec4> points() override;

    glm::vec3 size;
    glm::vec3 min_corner;
    glm::vec3 max_corner;
private:
    void calculate_minimals();
};

}

namespace engine_debug {

void draw_box_collider(engine::Collider& box);

}

#endif