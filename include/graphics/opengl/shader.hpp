#include <iostream>
#include <string>
#include <unordered_map>
#define GLEW_STATIC
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/opengl/gl_renderer.hpp"

enum UNIFORM_TYPE {
    U_FLOAT,
    U_BOOL,
    U_INT,
    U_UINT
};

class Shader {
public:
    Shader() {};
    Shader(std::string s_name, std::string vs_file, std::string fs_file);
    Shader(std::string s_name, std::string vs_file, std::string fs_file, std::string gs_file);
    GLuint get_shader_id() {
        return shader_id;
    }
    std::string get_shader_name() {
        return name;
    }
    void set_uniform(std::string name, glm::mat4 value);
    void set_uniform(std::string name, glm::mat3 value);
    void set_uniform(std::string name, glm::vec4 value);
    void set_uniform(std::string name, glm::vec3 value);
    void set_uniform(std::string name, float x, float y, float z);
    void set_uniform(std::string name, bool value);
    void set_uniform(std::string name, void* value, UNIFORM_TYPE u_type);
    void set_uniform(std::string name, GLfloat value);
    void set_uniform(std::string name, GLuint value);
    void set_uniform(std::string name, GLint value);
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
