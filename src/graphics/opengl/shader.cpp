#include "graphics/opengl/shader.hpp"

Shader::Shader(std::string s_name, std::string vs_file, std::string fs_file) {
    shader_id = opengl::BuildGlProgram(vs_file.c_str(), fs_file.c_str());
    name = s_name;
    get_uniforms();
}

Shader::Shader(std::string s_name, std::string vs_file, std::string fs_file, std::string gs_file) {
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
        uniform_locs.emplace(std::string(uniform_name), uniform_loc);
    }
}

void Shader::set_uniform(std::string name, glm::mat4 value) {
    if (uniform_locs.count(name) == 1 || true) {
        // FIXME: the name is light_cube_matrix[0] but can be sent with 
        // light_cube_matrix[x]
        //glUniformMatrix4fv(uniform_locs[name], 1, GL_FALSE, &value[0][0]);
        GLuint uniform_loc = glGetUniformLocation(shader_id, name.c_str());
        glUniformMatrix4fv(uniform_loc, 1, GL_FALSE, &value[0][0]);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(std::string name, glm::mat3 value) {
    if (uniform_locs.count(name) == 1) {
        glUniformMatrix3fv(uniform_locs[name], 1, GL_FALSE, &value[0][0]);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(std::string name, glm::vec4 value) {
    if (uniform_locs.count(name) == 1) {
        glUniform4f(uniform_locs[name], value.x, value.y, value.z, value.w);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(std::string name, glm::vec3 value) {
    if (uniform_locs.count(name) == 1) {
        glUniform3f(uniform_locs[name], value.x, value.y, value.z);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(std::string name, float x, float y, float z) {
    if (uniform_locs.count(name) == 1) {
        glUniform3f(uniform_locs[name], x, y, z);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(std::string name, bool value) {
    if (uniform_locs.count(name) == 1) {
        glUniform1i(uniform_locs[name], value);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(std::string name, void* value, UNIFORM_TYPE u_type) {
    if (uniform_locs.count(name) == 1) {
        switch(u_type) {
            case U_BOOL  : glUniform1i(uniform_locs[name], *(bool *)value); break;
            case U_FLOAT : glUniform1f(uniform_locs[name], *(float *)value); break;
            case U_INT   : glUniform1i(uniform_locs[name], *(int *)value); break;
            case U_UINT  : glUniform1ui(uniform_locs[name], *(GLuint *)value); break;
        }
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(std::string name, GLfloat value) {
    if (uniform_locs.count(name) == 1) {
        glUniform1f(uniform_locs[name], value);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(std::string name, GLuint value) {
    if (uniform_locs.count(name) == 1) {
        glUniform1ui(uniform_locs[name], value);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}

void Shader::set_uniform(std::string name, GLint value) {
    if (uniform_locs.count(name) == 1) {
        glUniform1i(uniform_locs[name], value);
    } else {
        std::cout << "RENDER:: Invalid uniform name: " << name << std::endl;
    }
}
