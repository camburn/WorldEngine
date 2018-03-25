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
#include "graphics/opengl/shader.hpp"
#include "graphics/opengl/gl_renderer.hpp"


struct Sprite {
    glm::mat4 model_matrix;
    std::string name;
    std::string texture_name;
};

struct Primitive {
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