#include <iostream>
#include <string>
#include <unordered_map>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/opengl/gl_renderer.hpp"

class Shader {
public:
    Shader() {};
    Shader(std::string s_name, std::string vs_file, std::string fs_file);
    Shader(std::string s_name, std::string vs_file, std::string fs_file, std::string gs_file);
    GLuint get_shader_id() {
        return shader_id;
    }
    void set_uniform(std::string name, glm::mat4 value);
    void set_uniform(std::string name, glm::mat3 value);
    void set_uniform(std::string name, glm::vec4 value);
    void set_uniform(std::string name, glm::vec3 value);
    void set_uniform(std::string name, float x, float y, float z);
    void set_uniform(std::string name, bool value);
    void activate() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glUseProgram(shader_id);
    }
private:
    std::unordered_map<std::string, GLuint> uniform_locs;
    void get_uniforms();
    GLuint shader_id;
    std::string name;

};
