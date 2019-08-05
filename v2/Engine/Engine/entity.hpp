#ifndef _ENTITY_HPP
#define _ENTITY_HPP

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"

namespace engine {

//                  vec4      vec3     vec2
// Attribute data - Vertices, normals, UV coordinates,

// Uniform data - Model matrix, Material Properties

class Entity {
public:
    Entity(){}
    void add_attribute_data(std::string name, std::vector<glm::vec4> &data);
    void add_index_data(std::vector<uint32_t> &data);
    void add_uniform_data(std::string name, glm::vec4 data);
    void add_uniform_data(std::string name, glm::mat4 data);

    void update_buffers(const std::shared_ptr<Shader>& shader);

    void render(Shader &shader);

    std::map<std::string, std::vector<glm::vec4>> attribute_data;
    std::map<std::string, glm::vec4> uniform_vec4_data;
    std::map<std::string, glm::mat4> uniform_mat4_data;
    std::vector<uint32_t> index_data;

    std::shared_ptr<VertexArray> get_vao(){return vao;}

private:
    bool buffers_set = false;
    std::shared_ptr<VertexArray> vao;
    std::shared_ptr<VertexBuffer> buffer;
    std::shared_ptr<IndexBuffer> index_buffer;
    int attribute_size = 0;

    void interlace_data(BufferLayout &layout, std::vector<float> &data);
};

} // Namespace

#endif