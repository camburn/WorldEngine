#ifndef _ENTITY_HPP
#define _ENTITY_HPP

#include <map>
#include <vector>
#include <unordered_set>

#include <glm/glm.hpp>
#include "tiny_gltf.h"

#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"

#include "Tools/gltf_loader.hpp"

namespace engine {

//                  vec4      vec3     vec2
// Attribute data - Vertices, normals, UV coordinates,

// Uniform data - Model matrix, Material Properties

class Entity {
public:
    virtual void add_uniform_data(std::string name, glm::vec4 data);
    virtual void add_uniform_data(std::string name, glm::mat4 data);

    virtual void add_vertex_array(std::shared_ptr<VertexArray> vao);

    virtual void update_buffers(const std::shared_ptr<Shader>& shader) = 0;

    virtual void render() = 0;

    virtual void on_ui_render(bool display) = 0;

    virtual glm::vec3 get_translation() = 0;
    virtual glm::vec3 get_scale() = 0;
    virtual glm::quat get_rotation() = 0;

    virtual void set_translation(glm::vec3 value) = 0;
    virtual void set_scale(glm::vec3 value) = 0;
    virtual void set_rotation(glm::quat value) = 0;

    std::vector<std::shared_ptr<VertexArray>>::iterator begin() { return vaos.begin(); }
    std::vector<std::shared_ptr<VertexArray>>::iterator end() { return vaos.end(); }
    std::vector<std::shared_ptr<VertexArray>>::const_iterator begin() const { return vaos.begin(); }
    std::vector<std::shared_ptr<VertexArray>>::const_iterator end() const { return vaos.end(); }

    std::shared_ptr<VertexArray> get_vao(uint32_t index);
    std::map<std::string, glm::vec4> uniform_vec4_data;
    std::map<std::string, glm::mat4> uniform_mat4_data;
    std::vector<GLuint> texture_ids;
    std::shared_ptr<Shader> allocated_shader;
    bool draw = true;
protected:
    std::vector<std::shared_ptr<VertexArray>> vaos;
};

class CustomEntity: public Entity {
public:
    CustomEntity(){}
    void add_attribute_data(std::string name, std::vector<glm::vec4> &data);
    void add_attribute_data(std::string name, std::vector<glm::vec2> &data);
    void add_index_data(std::vector<uint32_t> &data);

    void update_buffers(const std::shared_ptr<Shader>& shader) override;

    void render() override;

    virtual void on_ui_render(bool display) override;

    std::map<std::string, std::vector<glm::vec4>> attribute_data_vec4;
    std::map<std::string, std::vector<glm::vec3>> attribute_data_vec3;
    std::map<std::string, std::vector<glm::vec2>> attribute_data_vec2;
    std::vector<uint32_t> index_data;

    glm::vec3 get_translation() override { return translation; }
    glm::vec3 get_scale() override { return scale; }
    glm::quat get_rotation() override { return rotation; }

    void set_translation(glm::vec3 value) override { translation = value; }
    void set_scale(glm::vec3 value) override { scale = value; }
    void set_rotation(glm::quat value) override { rotation = value; }

private:
    glm::vec3 translation;
    glm::vec3 scale;
    glm::quat rotation;
    bool buffers_set = false;
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

    void render() override;

    static std::shared_ptr<GltfEntity> load_from_file(std::string file_name);

    virtual void on_ui_render(bool display) override;
    NodeObject &get_node() { return node_object; }

    glm::vec3 get_translation() override { return node_object.get_translation(); }
    glm::vec3 get_scale() override { return node_object.get_scale(); }
    glm::quat get_rotation() override { return node_object.get_rotation(); }

    void set_translation(glm::vec3 value) override { node_object.set_translation(value); }
    void set_scale(glm::vec3 value) override { node_object.set_scale(value); }
    void set_rotation(glm::quat value) override { node_object.set_rotation(value); }

private:
    std::vector<std::shared_ptr<VertexBuffer>> buffers;
    std::shared_ptr<IndexBuffer> index_buffer;
    std::shared_ptr<tinygltf::Model> model;
    std::unordered_set<uint32_t> handled_shaders;
    NodeObject node_object;
};

} // Namespace

#endif