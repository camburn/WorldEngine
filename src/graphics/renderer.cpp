#include "graphics/renderer.hpp"


Renderer::Renderer() {
    // Prepare this renderer for rendering!
    opengl::init();
    opengl::create_window();
    opengl::enable_debug();
    opengl::set_base_state();
}

Shader& Renderer::get_shader(string name) {
    return shader_map[name];
}

GLuint Renderer::get_shader_id(string name) {
    return shader_map[name].get_shader_id();
}

Shader& Renderer::active() {
    return *ptr_active_shader;
}

void Renderer::activate(GLuint id) {
    if (shader_id_map.count(id) == 1) {
        shader_id_map[id]->activate();
        ptr_active_shader = shader_id_map[id];
    } else {
        std::cout << "RENDER:: Error activating render mode: " << id << std::endl;
    }
}

void Renderer::activate(string name) {
    if (shader_map.count(name) == 1) {
        shader_map[name].activate();
        ptr_active_shader = &shader_map[name];
    } else {
        std::cout << "RENDER:: Error activating render mode: " << name << std::endl;
    }
}

void Renderer::LoadShaders(
        GLuint *programID, 
        GLuint *sprite_program,
        GLuint *simple_program,
        GLuint *line_program
    ) {

    shader_map.emplace(
        "default", 
        Shader(
            "default",
            "./src/shaders/vertex_shader.glsl", 
            "./src/shaders/fragment_shader.glsl"
        )
    );
    *programID = shader_map["default"].get_shader_id();
    shader_id_map.emplace(*programID, &shader_map["default"]);

    shader_map.emplace(
        "sprite", 
        Shader(
            "sprite",
            "./src/shaders/sprite_vertex_shader.glsl", 
            "./src/shaders/sprite_fragment_shader.glsl"
        )
    );
    *sprite_program = shader_map["sprite"].get_shader_id();
    shader_id_map.emplace(*sprite_program, &shader_map["sprite"]);

    shader_map.emplace(
        "simple", 
        Shader(
            "simple",
            "./src/shaders/simple_vertex_shader.glsl",
            "./src/shaders/simple_fragment_shader.glsl"
        )
    );
    *simple_program = shader_map["simple"].get_shader_id();
    shader_id_map.emplace(*simple_program, &shader_map["simple"]);

    shader_map.emplace(
        "line", 
        Shader(
            "line",
            "./src/shaders/line_v_shader.glsl",
            "./src/shaders/line_f_shader.glsl",
            "./src/shaders/line_g_shader.glsl"
        )
    );
    *line_program = shader_map["line"].get_shader_id();
    shader_id_map.emplace(*line_program, &shader_map["line"]);
};

GLFWwindow* Renderer::get_window() {
    return opengl::get_window();
}

GLuint load_texture(string file, string path) {
    return BufferTextureDataFromFile("container.jpg", "./assets/textures/");
}

namespace primitives {
    GLuint cube_mesh_id = 0;
    GLuint light_mesh_id = 0;

    GLuint cube_mesh() {
        if (cube_mesh_id == 0) {
            cube_mesh_id = BufferMeshDataVNT(
                cube_data_normal, sizeof(cube_data_normal)
            );
        }
        return cube_mesh_id;
    }

    GLuint light_mesh() {
        if (light_mesh_id == 0) {
            light_mesh_id = BufferMeshDataVT(cube_data, sizeof(cube_data));
        }
        return light_mesh_id;
    }
}