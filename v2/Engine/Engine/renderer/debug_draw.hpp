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
        : DebugPrimitive(color), point_a(point_a), point_b(point_b) {
    }

    glm::vec3 point_a {0.0f};
    glm::vec3 point_b {0.0f};
    virtual PrimitiveType type() override { return LINE; }
};

struct DebugCube: public DebugPrimitive {
public:
    DebugCube(engine::Transform t, glm::vec4 color)
        : DebugPrimitive(color), t(t) {
    }
    engine::Transform t;
    virtual PrimitiveType type() override { return CUBE; }
};

//std::deque<std::pair<glm::vec3, glm::vec3>> deferred_calls;
std::deque<std::shared_ptr<DebugPrimitive>> deferred_calls;

std::shared_ptr<engine::Entity> debug_cube;
std::shared_ptr<engine::Shader> debug_shader;

void init() {
    #ifdef OPENGL_COMPATIBILITY
    std::string vs_file_simple = "./shaders/opengl2_vertex_simple.glsl";
    std::string fs_file_simple = "./shaders/opengl2_fragment_simple.glsl";
    #else
    std::string vs_file_simple = "./shaders/vertex_simple.glsl";
    std::string fs_file_simple = "./shaders/fragment_simple.glsl";
    #endif
    debug_shader.reset(new engine::Shader{ vs_file_simple, fs_file_simple });
    debug_cube = engine::GltfEntity::load_from_file("./assets/gltf/Cube/Cube.gltf");
}

void draw_line(glm::vec3 a, glm::vec3 b, glm::vec4 color) {

    glm::vec3 dir = glm::vec3(b - a);
    float distance = glm::distance(a, b);
    dir /= distance;
    glm::quat angle = glm::quatLookAt(dir, glm::vec3(0, 1, 0));
    glm::vec3 scale = glm::vec3(0.05f, 0.05f, distance / 2);

    engine::Transform t = { glm::vec3(a) + (dir * (distance/2)), scale, angle };
    debug_cube->add_uniform_data("u_color", color);
    engine::Renderer::submit_entity(debug_shader, debug_cube, t);
}

void draw_cube(engine::Transform t, glm::vec4 color) {
    debug_cube->add_uniform_data("u_color", color);
    engine::Renderer::submit_entity(debug_shader, debug_cube, t);
}


void draw_line_deferred(glm::vec3 a, glm::vec3 b, glm::vec4 color) {
    deferred_calls.emplace_back(
        new DebugLine{a, b, color }
    );
}

void draw_cube_deferred(engine::Transform t, glm::vec4 color) {
    deferred_calls.emplace_back(
        new DebugCube{t, color }
    );
}

void draw_deferred() {
    while (!deferred_calls.empty()) {
        auto prim = deferred_calls.front();
        if (prim->type() == LINE) {
            DebugLine line = *std::static_pointer_cast<DebugLine>(prim).get();
            draw_line(line.point_a, line.point_b, line.color);
        } else if (prim->type() == CUBE) {
            DebugCube cube = *std::static_pointer_cast<DebugCube>(prim).get();
            draw_cube(cube.t, cube.color);
        }
        deferred_calls.pop_front();
    }

}

}

#endif