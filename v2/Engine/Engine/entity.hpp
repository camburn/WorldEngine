#ifndef _ENTITY_HPP
#define _ENTITY_HPP

#include <map>
#include <vector>

#include <glm/glm.hpp>
#include "tiny_gltf.h"

#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"

namespace engine {

//                  vec4      vec3     vec2
// Attribute data - Vertices, normals, UV coordinates,

// Uniform data - Model matrix, Material Properties

class Entity {
public:
    virtual void add_uniform_data(std::string name, glm::vec4 data);
    virtual void add_uniform_data(std::string name, glm::mat4 data);

    virtual void update_buffers(const std::shared_ptr<Shader>& shader) = 0;

    virtual void render(Shader &shader) = 0;

    std::shared_ptr<VertexArray> get_vao(){ return vao; }
    std::map<std::string, glm::vec4> uniform_vec4_data;
    std::map<std::string, glm::mat4> uniform_mat4_data;
    std::vector<GLuint> texture_ids;

protected:
    std::shared_ptr<VertexArray> vao;
};

class CustomEntity: public Entity {
public:
    CustomEntity(){}
    void add_attribute_data(std::string name, std::vector<glm::vec4> &data);
    void add_attribute_data(std::string name, std::vector<glm::vec2> &data);
    void add_index_data(std::vector<uint32_t> &data);
    //void add_uniform_data(std::string name, glm::vec4 data);
    //void add_uniform_data(std::string name, glm::mat4 data);

    void update_buffers(const std::shared_ptr<Shader>& shader) override;

    void render(Shader &shader) override;

    std::map<std::string, std::vector<glm::vec4>> attribute_data_vec4;
    std::map<std::string, std::vector<glm::vec3>> attribute_data_vec3;
    std::map<std::string, std::vector<glm::vec2>> attribute_data_vec2;
    //std::map<std::string, glm::vec4> uniform_vec4_data;
    //std::map<std::string, glm::mat4> uniform_mat4_data;
    std::vector<uint32_t> index_data;

    //std::shared_ptr<VertexArray> get_vao(){return vao;}

private:
    bool buffers_set = false;
    //std::shared_ptr<VertexArray> vao;
    std::shared_ptr<VertexBuffer> buffer;
    std::shared_ptr<IndexBuffer> index_buffer;
    int attribute_size = 0;

    void interlace_data(BufferLayout &layout, std::vector<float> &data);
};


class GltfEntity: public Entity {
public:
    GltfEntity(std::shared_ptr<tinygltf::Model> model_data);
    ~GltfEntity() {}

    void update_buffers(const std::shared_ptr<Shader>& shader) override;

    void render(Shader &shader) override;

    static std::shared_ptr<GltfEntity> load_from_file(std::string file_name);

private:
    //std::shared_ptr<VertexArray> vao;
    std::vector<std::shared_ptr<VertexBuffer>> buffers;
    std::shared_ptr<IndexBuffer> index_buffer;
    std::shared_ptr<tinygltf::Model> model;
};

} // Namespace

#endif