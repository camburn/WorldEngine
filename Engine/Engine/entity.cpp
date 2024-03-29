#include "engine.hpp"
#include "entity.hpp"

#include "Tools/gltf_loader.hpp"

namespace engine {

void Entity::add_uniform_data(std::string name, glm::vec4 data) {
    uniform_vec4_data[name] = data;
}

void Entity::add_uniform_data(std::string name, glm::mat4 data) {
    uniform_mat4_data[name] = data;
}

std::shared_ptr<VertexArray> Entity::get_vao(uint32_t index) { 
    if (vaos.size() < index) {
        return vaos[index];
    }
    return nullptr;
}

void Entity::add_vertex_array(std::shared_ptr<VertexArray> vao) {
    vaos.push_back(std::move(vao));
}

bool Entity::animated() {
    return has_animations;
}

void CustomEntity::add_attribute_data(std::string name, std::vector<glm::vec4> &data) {
    if (attribute_size <= 0) {
        attribute_size = data.size();
    }
    ENGINE_ASSERT(data.size() == attribute_size, "Attributes require the same number of elements");
    attribute_data_vec4[name] = data;
}

void CustomEntity::add_attribute_data(std::string name, std::vector<glm::vec3> &data) {
    if (attribute_size <= 0) {
        attribute_size = data.size();
    }
    ENGINE_ASSERT(data.size() == attribute_size, "Attributes require the same number of elements");
    attribute_data_vec3[name] = data;
}

void CustomEntity::add_attribute_data(std::string name, std::vector<glm::vec2> &data) {
    if (attribute_size <= 0) {
        attribute_size = data.size();
    }
    ENGINE_ASSERT(data.size() == attribute_size, "Attributes require the same number of elements");
    attribute_data_vec2[name] = data;
}

void CustomEntity::add_index_data(std::vector<uint32_t> &data) {
    index_data = data;
}

void CustomEntity::interlace_data(BufferLayout &layout, std::vector<float> &data) {
    if (attribute_data_vec4.size() == 0) {
        ENGINE_ASSERT(false, "No attribute data for this entity");
        return;
    }
    ;
    data.reserve(attribute_size * attribute_data_vec4.size());
    bool fill_missing_data = false;
    // All uniforms lists should have the same number of elements

    for (auto& element: layout) {
        if (attribute_data_vec4.count(element.name) > 0 ||
            attribute_data_vec3.count(element.name) > 0 ||
            attribute_data_vec2.count(element.name) > 0) {
            // We have that data
        } else if (fill_missing_data){
            // We don't have that data
            ENGINE_ERROR("NOT IMPLEMENTED");
        } else {
            ENGINE_ERROR("Entity missing attribute data required by shader - {0}", element.name);
            return;
            ENGINE_ASSERT(0, "Data required, cannot continue");
        }
    }
    //TODO: This only supports vec4 data
    for (int i = 0; i < attribute_size; i++) {  // For each element
        for (auto& element: layout) {
            if (element.size == 8) {
                ENGINE_ASSERT(attribute_data_vec2.count(element.name) > 0, "Mismatched shader type and element data - {0}", element.name);
                glm::vec2 value = attribute_data_vec2[element.name][i];
                data.insert(data.end(), { value.x, value.y });
            }
            else if (element.size == 12) {
                ENGINE_ASSERT(attribute_data_vec3.count(element.name) > 0, "Mismatched shader type and element data - {0}", element.name);
                glm::vec3 value = attribute_data_vec3[element.name][i];
                data.insert(data.end(), { value.x, value.y, value.z });
            }
            else if (element.size == 16) {
                ENGINE_ASSERT(attribute_data_vec4.count(element.name) > 0, "Mismatched shader type and element data - {0}", element.name);
                glm::vec4 value = attribute_data_vec4[element.name][i];
                data.insert(data.end(), { value.x, value.y, value.z, value.w });
            }
            else {
                ENGINE_ASSERT(0, "Element {0} with a size of {1} is not supported", element.name, element.size);
            }
        }
    }
}

void CustomEntity::update_buffers(const std::shared_ptr<Shader>& shader) {
    if (vaos.size() > 0) return;

    ENGINE_DEBUG("Generating vertex array and buffer for CustomEntity {0}", name);
    // TODO: Shader should cache entities that have buffered data already
    BufferLayout layout = shader->attribute_layout();

    // Need to generate data that matches this layout
    std::vector<float> data;
    interlace_data(layout, data);

    auto vao = VertexArray::create(draw_mode);
    buffer = VertexBuffer::create(&data[0], data.size() * sizeof(float));
    buffer->set_layout(layout);
    if (index_data.size() > 0) {
        index_buffer = IndexBuffer::create(&index_data[0], index_data.size());
        vao->set_index_buffer(index_buffer);
    } else {
        vao->set_array_count(attribute_size);
    }

    vao->add_vertex_buffer(buffer);

    buffers_set = true;
    shader->register_vertex_array(vao);
    handled_shaders.emplace(shader->get_id());

    shader_vaos.emplace(shader->get_id(), vao);
    vaos.push_back(vao);
}

void CustomEntity::render() {
    //Renderer::submit(shader, this);
}

void CustomEntity::on_ui_render(bool display) {
    
}

std::shared_ptr<GltfEntity> GltfEntity::load_from_file(std::string file_name) {
    std::shared_ptr<tinygltf::Model> model = std::make_shared<tinygltf::Model>();
    tinygltf::TinyGLTF loader;
    std::string err;
    std::string warn;
    ENGINE_INFO("Loading GLTF - {0}", file_name);
    bool ret = loader.LoadASCIIFromFile(model.get(), &err, &warn, file_name);
    //bool ret = loader.LoadBinaryFromFile(&model, &err, &warn, filename); // for binary glTF(.glb)

    if (!warn.empty()) ENGINE_WARN("{0}", warn);
    if (!err.empty()) ENGINE_ERROR("{0}", err);
    if (!ret) ENGINE_ASSERT(false, "Failed to parse glTF");

    std::shared_ptr<GltfEntity> entity = std::make_shared<GltfEntity>(model);

    entity->file_name = file_name;

    return entity; 
}

GltfEntity::GltfEntity(std::shared_ptr<tinygltf::Model> model_data) {
    model = model_data;
    if (model->animations.size() > 0) {
        // This entity has animations - mark it as so
        has_animations = true;
    }
}

void GltfEntity::update_buffers(const std::shared_ptr<Shader>& shader) {
    if (animated()) {
        for (std::shared_ptr<ShaderStorageBuffer> ssbo : ssbos) {
            //shader-> Check if shader supports animations
            ssbo->bind(0);
        }
    }
    if (buffered) return;
    if (handled_shaders.count(shader->get_id()) > 0 ) return;
    ModelObjects m_obj;
    ENGINE_INFO("Processing GLtf - {0}", file_name);
    node_object = gltf_to_opengl(m_obj, model, shader);
    texture_ids = m_obj.texture_ids;
    vaos = m_obj.vaos;
    ssbos = m_obj.ssbos;
    handled_shaders.emplace(shader->get_id());
    buffered = true;
    min_extents = m_obj.min_extents;
    max_extents = m_obj.max_extents;
}

void GltfEntity::clear_gltf_data() {
    if (model.unique()) {
        ENGINE_INFO("Deleting model state");
    }
    model.reset();
}

void GltfEntity::render() {

}

void GltfEntity::on_ui_render(bool display) {
    draw_node_graph(node_object);
}

} // namespace