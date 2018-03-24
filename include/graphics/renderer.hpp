#ifndef RENDERER_H
#define RENDERER_H
/* Base Renderer Class
 *
 */
#include <iostream>
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/cube.hpp"
#include "graphics/model.hpp"
#include "graphics/buffers.hpp"
#include "graphics/opengl/gl_renderer.hpp"

class Shader {
public:
    Shader() {};
    Shader(string s_name, string vs_file, string fs_file);
    Shader(string s_name, string vs_file, string fs_file, string gs_file);
    GLuint get_shader_id() {
        return shader_id;
    }
    void Shader::set_uniform(string name, glm::mat4 value);
    void Shader::set_uniform(string name, glm::mat3 value);
    void Shader::set_uniform(string name, glm::vec4 value);
    void Shader::set_uniform(string name, glm::vec3 value);
    void Shader::set_uniform(string name, float x, float y, float z);
    void Shader::set_uniform(string name, bool value);
    void activate() {
        glUseProgram(shader_id);
    }
private:
    std::unordered_map<string, GLuint> uniform_locs;
    void get_uniforms();
    GLuint shader_id;
    string name;

};

struct DrawObject {
    GLuint mesh_id;
    GLuint tex_id;
    glm::vec3 pos;
    glm::vec3 rot;
    glm::vec3 scale;
    GLuint program;
    const char *name;
};

struct ModelObject {
    glm::vec3 translation;
    GLfloat rotationX;
    GLfloat rotationY;
    GLfloat rotationZ;
    glm::vec3 scale;
    Model model;
};

GLuint get_shader_id(string name);
Shader get_shader(string name);



class Renderer {
public:
    Renderer();
    void LoadShaders(
        GLuint *programID, GLuint *sprite_program,
        GLuint *simple_program, GLuint *line_program
    );
    GLFWwindow* get_window();
    Shader& active();
    void activate(string name);
    void activate(GLuint id);
    GLuint get_shader_id(string name);
    Shader& get_shader(string name);
    
private:
    Shader *ptr_active_shader;
    std::unordered_map<string, Shader> shader_map;
    std::unordered_map<GLuint, Shader*> shader_id_map;
    
};

GLuint load_texture(string file, string path) ;

namespace primitives {
    GLuint cube_mesh();
    GLuint light_mesh();
}

#endif