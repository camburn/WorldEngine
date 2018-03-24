#include "graphics/renderer.hpp"



Shader::Shader(string s_name, string vs_file, string fs_file) {
    shader_id = opengl::BuildGlProgram(vs_file.c_str(), fs_file.c_str());
    name = s_name;
    get_uniforms();
}

Shader::Shader(string s_name, string vs_file, string fs_file, string gs_file) {
    shader_id = opengl::BuildGlProgram(vs_file.c_str(), fs_file.c_str(), gs_file.c_str());
    name = s_name;
    get_uniforms();
}

void Shader::get_uniforms() {
    GLint i;
    GLint count;

    GLint size; // size of the variable
    GLenum type; // type of the variable (float, vec3 or mat4, etc)

    const GLsizei bufSize = 32; // maximum name length
    GLchar uniform_name[bufSize]; // variable name in GLSL
    GLsizei length; // name length

    glGetProgramiv(shader_id, GL_ACTIVE_ATTRIBUTES, &count);
    std::cout << "SHADER:: " <<  name << " Active Attributes: " << count << std::endl;

    for (i=0; i < count; i++)
    {
        glGetActiveAttrib(shader_id, (GLuint)i, bufSize, &length, &size, &type, uniform_name);

        std::cout << "SHADER:: Attribute No: " << i << " Type: " << type << " Name: " << uniform_name << std::endl;
    }

    glGetProgramiv(shader_id, GL_ACTIVE_UNIFORMS, &count);
    std::cout << "SHADER:: " <<  name << " Active Uniforms: " << count << std::endl;

    for (i=0; i < count; i++) {
        glGetActiveUniform(
            shader_id, (GLuint)i, bufSize, &length, &size, &type, uniform_name
        );
        std::cout << "SHADER:: Uniform No: " << i << " Type: " << type << " Name: " << uniform_name << std::endl;
        GLuint uniform_loc = glGetUniformLocation(shader_id, uniform_name);
        uniform_locs.emplace(string(uniform_name), uniform_loc);
    }
}

void Shader::set_uniform(string name, glm::mat4 value) {
    if (uniform_locs.count(name) == 1) {
        glUniformMatrix4fv(uniform_locs[name], 1, GL_FALSE, &value[0][0]);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(string name, glm::mat3 value) {
    if (uniform_locs.count(name) == 1) {
        glUniformMatrix3fv(uniform_locs[name], 1, GL_FALSE, &value[0][0]);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(string name, glm::vec4 value) {
    if (uniform_locs.count(name) == 1) {
        glUniform4f(uniform_locs[name], value.x, value.y, value.z, value.w);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(string name, glm::vec3 value) {
    if (uniform_locs.count(name) == 1) {
        glUniform3f(uniform_locs[name], value.x, value.y, value.z);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(string name, float x, float y, float z) {
    if (uniform_locs.count(name) == 1) {
        glUniform3f(uniform_locs[name], x, y, z);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(string name, bool value) {
    if (uniform_locs.count(name) == 1) {
        glUniform1i(uniform_locs[name], value);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

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