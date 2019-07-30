#ifndef _SHADER_HPP
#define _SHADER_HPP
#include <string>


namespace engine {

class Shader {
public:
    Shader(std::string &vertex_shader_file_path, std::string &fragment_shader_file_path);
    ~Shader();
    void bind();
    void unbind();

private:
    uint32_t shader_id;
};

}  // namespace

#endif