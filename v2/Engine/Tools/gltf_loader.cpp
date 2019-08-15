#include "engine.hpp"

#include "gltf_loader.hpp"
#include "Engine/renderer/buffer.hpp"
//#include "Engine/entity.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"

using namespace tinygltf;

std::map<int, std::string> PRIMITIVE_DRAW_MODE {
    {0, "POINTS"},
    {1, "LINES"},
    {2, "LINE_LOOP"},
    {3, "LINE_STRIP"},
    {4, "TRIANGLES"},
    {5, "TRIANGLE_STRIP"},
    {6, "TRIANGLE_FAN"},
};
std::map<int, std::string> ACCESSOR_COMPONENT_TYPE {
    {5120, "BYTE"},
    {5121, "UNSIGNED_BYTE"},
    {5122, "SHORT"},
    {5123, "UNSIGNED_SHORT"},
    {5125, "UNSIGNED_INT"},
    {5126, "FLOAT"}
};
std::map<int, std::string> ACCESSOR_TYPE {
    {TINYGLTF_TYPE_SCALAR, "SCALAR"},
    {TINYGLTF_TYPE_VEC2, "VEC2"},
    {TINYGLTF_TYPE_VEC3, "VEC3"},
    {TINYGLTF_TYPE_VEC4, "VEC4"},
    {TINYGLTF_TYPE_MAT2, "MAT2"},
    {TINYGLTF_TYPE_MAT3, "MAT3"},
    {TINYGLTF_TYPE_MAT4, "MAT4"},
};
static std::map<int, engine::ShaderDataType> ACCESSOR_TO_SHADER_TYPE {
    {TINYGLTF_TYPE_VEC2, engine::ShaderDataType::Float2},
    {TINYGLTF_TYPE_VEC3 , engine::ShaderDataType::Float3},
    {TINYGLTF_TYPE_VEC4 , engine::ShaderDataType::Float4},
    {TINYGLTF_TYPE_SCALAR , engine::ShaderDataType::Int},
    {TINYGLTF_TYPE_MAT3 , engine::ShaderDataType::Mat3},
    {TINYGLTF_TYPE_MAT4 , engine::ShaderDataType::Mat4},
};
std::map<int, std::string> BUFFER_VIEW_TARGET {
    {34962, "ARRAY_BUFFER"},
    {34963, "ELEMENT_ARRAY_BUFFER"},
};

void describe_mesh(Mesh &mesh, std::string indent="") {
    ENGINE_INFO(indent + "    - Mesh - {0}", mesh.name);
    for (auto primitive: mesh.primitives) {
        ENGINE_INFO(indent + "      - Primitive");
        ENGINE_INFO(indent + "        + Mode: {0}", PRIMITIVE_DRAW_MODE[primitive.mode]);
        if (primitive.indices != -1)
            ENGINE_INFO(indent + "        + Indices: {0}", primitive.indices);
        if (primitive.material != -1)
            ENGINE_INFO(indent + "        + material: {0}", primitive.material);
        ENGINE_INFO(indent + "        - Attributes", primitive.material);
        for (auto &[name, attr_index]: primitive.attributes){
            ENGINE_INFO(indent + "          + {0}: Accessor index: {1}", name, attr_index);
        }
    }
}

void describe_node(std::shared_ptr<Model> &model, Node &node, int node_index, std::string indent) {
    ENGINE_INFO(indent + "  - Node {0} - {1}", node_index, node.name);
    // A node can link to *a* mesh
    if (node.mesh != -1) {
        auto mesh = model->meshes[node.mesh];
        describe_mesh(mesh, indent);
    // Or *a* camera
    } else if (node.camera != -1) {
        ENGINE_INFO(indent + "    + Camera: {0}", node.camera);
    }
    if (node.matrix.size() > 0)
        ENGINE_INFO(indent + "    + Matrix");
    if (node.translation.size() > 0)
        ENGINE_INFO(indent + "    + Translation");
    if (node.rotation.size() > 0)
        ENGINE_INFO(indent + "    + Rotation");
    if (node.scale.size() > 0)
        ENGINE_INFO(indent + "    + Scale");

    for (auto child_index: node.children) {
        describe_node(model, model->nodes[child_index], child_index, indent + "  ");
    }
}

void inspect_gltf(std::shared_ptr<Model> model) {
    //std::shared_ptr<Model> model = std::make_shared<Model>();
    TinyGLTF loader;
    std::string err;
    std::string warn;

    // Model is one VAO (Render call)
    int scene_count = model->scenes.size();
    
    ENGINE_WARN("Model Hierarchy");
    for (auto scene: model->scenes) {
        ENGINE_INFO("Scene - {0}", scene.name);
        for (auto node_index: scene.nodes) {
            auto node = model->nodes[node_index];
            describe_node(model, node, node_index, "");
        }
    }

    ENGINE_WARN("Buffer Layout");

    int buffer_index = 0;
    for (auto buffer: model->buffers) {
        ENGINE_INFO("Buffer - {0} bytes", buffer.data.size());
        int buffer_view_index = 0;
        for (auto buffer_view: model->bufferViews) {
            if (buffer_view.buffer == buffer_index){  // Check if this view belongs to this buffer
                ENGINE_INFO("  - View {0} - {1}", buffer_view_index, buffer_view.name);
                ENGINE_INFO("    + Target: {0}", BUFFER_VIEW_TARGET[buffer_view.target]);
                ENGINE_INFO("    + Length: {0} bytes", buffer_view.byteLength);
                ENGINE_INFO("    + Offset: {0} bytes", buffer_view.byteOffset);
                ENGINE_INFO("    + Stride: {0} bytes", buffer_view.byteStride);
                int accessor_index = 0;
                for (auto accessor: model->accessors) {
                    if (accessor.bufferView == buffer_view_index) {  // Check if accessor belongs to this view
                        ENGINE_INFO("    - Accessor {0} - {1}", accessor_index, accessor.name);
                        ENGINE_INFO("      + Offset - {0} bytes", accessor.byteOffset);
                        ENGINE_INFO("      + Component type - {0}", ACCESSOR_COMPONENT_TYPE[accessor.componentType]);
                        ENGINE_INFO("      + Component Count - {0}", accessor.count);
                        ENGINE_INFO("      + type - {0}", ACCESSOR_TYPE[accessor.type]);
                    }
                    
                    accessor_index++;
                }
            }
            buffer_view_index++;
        }
        buffer_index++;
    }
    ENGINE_INFO("External model processed");
    //return model;
}

class ModelObject {

    std::vector<GLuint> vbos;
    GLuint vao;
};

void process_mesh(std::shared_ptr<Model> &model, Mesh &mesh, std::shared_ptr<engine::VertexArray> &vao, const std::shared_ptr<engine::Shader> &shader) {
    // A mesh has X primitives
    std::map<std::string, int> required_accessors;

    // gather supported accessors
    for (Primitive primitive: mesh.primitives) {
        if (primitive.indices != -1) {
            // This has indices
            required_accessors.emplace("INDICES", primitive.indices);
        }
        // Primitive describe the mode, its indices, and attributes
        for (auto &attr: primitive.attributes){
            if (shader->attribute_supported(attr.first)) {
                required_accessors.emplace(attr);
            }
        }
    }

    std::vector<int> required_buffer_views;
    // Gather supported buffer views
    for (auto &[name, index]: required_accessors) {
        Accessor accessor = model->accessors[index];

        if (accessor.bufferView != -1){
            BufferView buffer_view = model->bufferViews[accessor.bufferView];
            /*
            GLuint vbo;
            glGenBuffers(1, &vbo);
            glBindBuffer(buffer_view.target, vbo);
            glBufferData(
                buffer_view.target,
                buffer_view.byteLength,
                &model->buffers[buffer_view.buffer].data.at(0) + buffer_view.byteOffset,
                GL_STATIC_DRAW
            );
             */

            if(buffer_view.target == GL_ELEMENT_ARRAY_BUFFER) {
                std::shared_ptr<engine::IndexBuffer> index_buffer (
                    engine::IndexBuffer::create(
                        &model->buffers[buffer_view.buffer].data.at(0) + buffer_view.byteOffset,
                        accessor.count,
                        buffer_view.byteLength
                    )
                );
                vao->set_index_buffer(index_buffer);
            } else {
                std::shared_ptr<engine::VertexBuffer> index_buffer (
                    engine::VertexBuffer::create(
                        &model->buffers[buffer_view.buffer].data.at(0) + buffer_view.byteOffset,
                        buffer_view.byteLength
                    )
                );
                vao->add_vertex_buffer(index_buffer, false);
            }

        } else {
            ENGINE_WARN("Accessor with no buffer view, should be filled with zeros");
        }
        if (name != "INDICES") {
            int vertex_attribute_location = shader->attribute_location(name);
            glEnableVertexAttribArray(vertex_attribute_location);
            glVertexAttribPointer(
                vertex_attribute_location,
                accessor.type,
                accessor.componentType,
                accessor.normalized ? GL_TRUE : GL_FALSE,
                model->bufferViews[accessor.bufferView].byteStride,
                (const void *)(accessor.byteOffset)
            );
        }
    }
}


void process_node(std::shared_ptr<Model> &model, Node &node, std::shared_ptr<engine::VertexArray> &vao, const std::shared_ptr<engine::Shader> &shader) {
    if (node.mesh != -1) {
        process_mesh(model, model->meshes[node.mesh], vao, shader);
    }
    for (int child_index: node.children) {
        process_node(model, model->nodes[child_index], vao, shader);
    }
}

std::shared_ptr<engine::VertexArray> gltf_to_opengl(std::shared_ptr<Model> &model, const std::shared_ptr<engine::Shader> &shader) {
    inspect_gltf(model);
    //GLuint vao;
    //glGenVertexArrays(1, &vao);
    std::shared_ptr<engine::VertexArray> vao (engine::VertexArray::create());
    vao->bind();
    //glBindVertexArray(vao);

    for (Scene &scene: model->scenes) {
        for (int &node_index: scene.nodes) {
            process_node(model, model->nodes[node_index], vao, shader);
        }
    }
    glBindVertexArray(0);
    vao->unbind();
    return vao;
}
