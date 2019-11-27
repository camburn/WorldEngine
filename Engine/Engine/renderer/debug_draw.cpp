#include "engine.hpp"
#include "debug_draw.hpp"

namespace engine_debug {


//std::deque<std::pair<glm::vec3, glm::vec3>> deferred_calls;
std::deque<std::shared_ptr<DebugPrimitive>> deferred_calls;

std::shared_ptr<engine::Entity> debug_cube;
std::shared_ptr<engine::Shader> debug_shader;
std::shared_ptr<engine::Shader> line_shader;

void init() {
    #ifdef OPENGL_COMPATIBILITY
    std::string vs_file_simple = "./shaders/opengl2_vertex_simple.glsl";
    std::string fs_file_simple = "./shaders/opengl2_fragment_simple.glsl";
    std::string vs_line_simple = "./shaders/opengl2_vertex_line.glsl";
    std::string fs_line_simple = "./shaders/opengl2_fragment_line.glsl";
    #else
    std::string vs_file_simple = "./shaders/vertex_simple.glsl";
    std::string fs_file_simple = "./shaders/fragment_simple.glsl";
    #endif
    debug_shader.reset(new engine::Shader{ vs_file_simple, fs_file_simple });
    debug_cube = engine::GltfEntity::load_from_file("./assets/gltf/Cube/Cube.gltf");

    line_shader.reset(new engine::Shader{ vs_line_simple, fs_line_simple });
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

void new_frame() {
    enginegl::new_frame();
}

void draw_simple_line(glm::vec4 a, glm::vec4 b, glm::vec4 color) {
    enginegl::draw_line(a, b, color);
}

void draw_simple_line(glm::vec3 a, glm::vec3 b, glm::vec4 color) {
    enginegl::draw_line(
        glm::vec4(a, 1.0f),
        glm::vec4(b, 1.0f),
        color
    );
}

void draw_buffers() {
    line_shader->bind();
    //line_shader->upload_u_vec4("u_color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    line_shader->upload_u_mat4("u_model", glm::mat4{1.0f});
    line_shader->upload_u_mat4("u_view_projection",
        engine::Renderer::scene_state->view_projection_matrix
    );
    enginegl::draw_buffers();
}

}
