#ifndef _SHADER_HPP
#define _SHADER_HPP
#include <string>

#include <glm/glm.hpp>

namespace engine {

class Shader {
public:
    Shader(std::string &vertex_shader_file_path, std::string &fragment_shader_file_path);
    ~Shader();
    void bind();
    void unbind();

    void upload_u_mat4(const std::string& u_name, const glm::mat4& matrix);

    void inspect_uniforms();
    void inspect_attributes();
    void program_resources();

private:
    uint32_t shader_id;
};

}  // namespace

#endif