#include "engine.hpp"
#include "entity.hpp"

namespace engine {

void Entity::add_attribute_data(std::string name, std::vector<glm::vec4> &data) {
    if (attribute_size <= 0) {
        attribute_size = data.size();
    }
    ENGINE_ASSERT(data.size() == attribute_size, "Attributes require the same number of elements");
    attribute_data[name] = data;
}

void Entity::add_index_data(std::vector<uint32_t> &data) {
    index_data = data;
}

void Entity::add_uniform_data(std::string name, glm::vec4 data) {
    uniform_vec4_data[name] = data;
}

void Entity::add_uniform_data(std::string name, glm::mat4 data) {
    uniform_mat4_data[name] = data;
}

void Entity::interlace_data(BufferLayout &layout, std::vector<float> &data) {
    if (attribute_data.size() == 0) {
        ENGINE_ERROR("No attribute data for this entity");
        return;
    }
    ;
    data.reserve(attribute_size * attribute_data.size());
    bool fill_missing_data = false;
    // All uniforms lists should have the same number of elements

    for (auto& element: layout) {
        if (attribute_data.count(element.name) > 0) {
            // We have that data
        } else if (fill_missing_data){
            // We don't have that data
            ENGINE_ERROR("NOT IMPLEMENTED");
        } else {
            ENGINE_ERROR("Entity missing attribute data required by shader - {0}", element.name);
        }
    }
    //TODO: This only supports vec4 data
    for (int i = 0; i < attribute_size; i++) {  // For each element
        for (auto& element: layout) {
            glm::vec4 value = attribute_data[element.name][i];
            data.insert(data.end(), { value.x, value.y, value.z, value.w });
        }
    }
}

void Entity::update_buffers(const std::shared_ptr<Shader>& shader) {
    if (buffers_set) return;
    // TODO: Shader should cache entities that have buffered data already
    BufferLayout layout = shader->attribute_layout();

    // Need to generate data that matches this layout
    std::vector<float> data;
    interlace_data(layout, data);

    vao.reset(VertexArray::create());

    buffer.reset(VertexBuffer::create(&data[0], data.size() * sizeof(float)));
    buffer->set_layout(layout);
    index_buffer.reset(IndexBuffer::create(&index_data[0], index_data.size()));

    vao->add_vertex_buffer(buffer);
    vao->set_index_buffer(index_buffer);
    buffers_set = true;
}

void Entity::render(Shader &shader) {
    //Renderer::submit(shader, this);
}

} // namespace