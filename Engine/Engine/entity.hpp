#ifndef _ENTITY_HPP
#define _ENTITY_HPP

#include <map>
#include <vector>
#include <unordered_set>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/common.hpp>
#include "tiny_gltf.h"

#include "Engine/transform.hpp"
#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"

#include "Tools/gltf_loader.hpp"

namespace engine {

//  TODO Rename to mesh:
class Entity {
public:
    virtual void add_uniform_data(std::string name, glm::vec4 data);
    virtual void add_uniform_data(std::string name, glm::mat4 data);

    virtual void add_vertex_array(std::shared_ptr<VertexArray> vao);

    virtual void update_buffers(const std::shared_ptr<Shader>& shader) = 0;

    virtual void render() = 0;

    virtual void on_ui_render(bool display) = 0;

    bool animated();

    std::vector<std::shared_ptr<VertexArray>>::iterator begin() { return vaos.begin(); }
    std::vector<std::shared_ptr<VertexArray>>::iterator end() { return vaos.end(); }
    std::vector<std::shared_ptr<VertexArray>>::const_iterator begin() const { return vaos.begin(); }
    std::vector<std::shared_ptr<VertexArray>>::const_iterator end() const { return vaos.end(); }

    std::shared_ptr<VertexArray> get_vao(uint32_t index);
    std::map<std::string, glm::vec4> uniform_vec4_data;
    std::map<std::string, glm::mat4> uniform_mat4_data { {"u_model", glm::mat4(1.0f) }};
    std::vector<GLuint> texture_ids;
    std::shared_ptr<Shader> allocated_shader;
    bool draw = true;
    std::string name;

    glm::vec3 min_extents {0.0f};
    glm::vec3 max_extents {0.0f};

    Transform transform;

protected:
    std::vector<std::shared_ptr<VertexArray>> vaos;
    std::vector<std::shared_ptr<ShaderStorageBuffer>> ssbos;
    bool has_animations = false;
};

class CustomEntity: public Entity {
public:
    CustomEntity(){}
    CustomEntity(engine::DrawMode mode): draw_mode(mode) {}
    ~CustomEntity() { ENGINE_INFO("Custom Entity {0} garbage collected", name); }
    void add_attribute_data(std::string name, std::vector<glm::vec4> &data);
    void add_attribute_data(std::string name, std::vector<glm::vec3> &data);
    void add_attribute_data(std::string name, std::vector<glm::vec2> &data);
    void add_index_data(std::vector<uint32_t> &data);

    void update_buffers(const std::shared_ptr<Shader>& shader) override;

    void render() override;

    virtual void on_ui_render(bool display) override;

    std::map<std::string, std::vector<glm::vec4>> attribute_data_vec4;
    std::map<std::string, std::vector<glm::vec3>> attribute_data_vec3;
    std::map<std::string, std::vector<glm::vec2>> attribute_data_vec2;
    std::vector<uint32_t> index_data;

    DrawMode get_drawmode() { return draw_mode; }
    bool elements_set() { return index_buffer != nullptr; }

    void add_texture(std::string sampler_name, std::shared_ptr<Texture> texture) { 
        textures.emplace(sampler_name, texture); 
    }
    std::shared_ptr<VertexArray> get_shader_vaos(uint32_t shader_id) {
        if (shader_vaos.count(shader_id) > 0)
            return shader_vaos.at(shader_id);
        return nullptr;
    }

    std::map<std::string, std::shared_ptr<Texture>> textures;

private:
    DrawMode draw_mode = DrawMode::TRIANGLES;
    glm::vec3 translation {0.0f, 0.0f, 0.0f};
    glm::vec3 scale {1.0f, 1.0f, 1.0f};
    glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
    bool buffers_set = false;
    std::shared_ptr<VertexBuffer> buffer;
    std::shared_ptr<IndexBuffer> index_buffer;
    int attribute_size = 0;
    std::unordered_set<uint32_t> handled_shaders;
    std::map<uint32_t, std::shared_ptr<VertexArray>> shader_vaos;
    void interlace_data(BufferLayout &layout, std::vector<float> &data);
};


class GltfEntity: public Entity {
public:
    GltfEntity(std::shared_ptr<tinygltf::Model> model_data);
    ~GltfEntity() { ENGINE_INFO("GLTF Entity {0} garbage collected", name); }

    void update_buffers(const std::shared_ptr<Shader>& shader) override;
    void clear_gltf_data();

    void render() override;

    static std::shared_ptr<GltfEntity> load_from_file(std::string file_name);

    virtual void on_ui_render(bool display) override;
    NodeObject &get_node() { return node_object; }

    std::string file_name = "";

private:
    std::vector<std::shared_ptr<VertexBuffer>> buffers;
    std::shared_ptr<IndexBuffer> index_buffer;
    std::shared_ptr<tinygltf::Model> model;
    std::unordered_set<uint32_t> handled_shaders;
    NodeObject node_object;
    bool buffered = false;

};

} // Namespace

#endif