#ifndef _DEBUG_DRAW_HPP
#define _DEBUG_DRAW_HPP

#include <deque>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Engine/entity.hpp"
#include "Engine/renderer/renderer.hpp"
#include "Engine/renderer/shader.hpp"
#include "Platform/OpenGL/gl_draw_debug.hpp"

namespace engine_debug {

enum PrimitiveType {
    NONE,
    CUBE,
    LINE
};


struct DebugPrimitive {
public:
    DebugPrimitive(glm::vec4 color): color(color) {}
    glm::vec4 color {1.0f};
    virtual PrimitiveType type() = 0;
};

struct DebugLine: public DebugPrimitive {
public:
    DebugLine(glm::vec3 point_a, glm::vec3 point_b, glm::vec4 color)
        : DebugPrimitive(color), point_a(point_a), point_b(point_b) {}

    glm::vec3 point_a {0.0f};
    glm::vec3 point_b {0.0f};
    virtual PrimitiveType type() override { return LINE; }
};

struct DebugCube: public DebugPrimitive {
public:
    DebugCube(engine::Transform t, glm::vec4 color)
        : DebugPrimitive(color), t(t) {}
    engine::Transform t;
    virtual PrimitiveType type() override { return CUBE; }
};

void init();

void draw_line(glm::vec3 a, glm::vec3 b, glm::vec4 color);

void draw_cube(engine::Transform t, glm::vec4 color);

void draw_line_deferred(glm::vec3 a, glm::vec3 b, glm::vec4 color);

void draw_cube_deferred(engine::Transform t, glm::vec4 color);

void draw_deferred();

void new_frame();

void draw_simple_line(glm::vec4 a, glm::vec4 b, glm::vec4 color);
void draw_simple_line(glm::vec3 a, glm::vec3 b, glm::vec4 color);

void draw_buffers();

}

#endif