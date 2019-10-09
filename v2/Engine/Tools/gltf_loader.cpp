#include "engine.hpp"

#include "gltf_loader.hpp"
#include "Engine/renderer/buffer.hpp"
//#include "Engine/entity.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtx/decomposition.hpp>

#include "Platform/OpenGL/gl_texture.hpp"

#include "imgui.h"

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
std::map<int, std::string> TEXTURE_NAMES {
    { 0x2600, "GL_NEAREST"},
    { 0x2601, "GL_LINEAR"},
    { 0x2700, "GL_NEAREST_MIPMAP_NEAREST"},
    { 0x2701, "GL_LINEAR_MIPMAP_NEAREST"},
    { 0x2702, "GL_NEAREST_MIPMAP_LINEAR"},
    { 0x2703, "GL_LINEAR_MIPMAP_LINEAR"},
    { 0x2800, "GL_TEXTURE_MAG_FILTER"},
    { 0x2801, "GL_TEXTURE_MIN_FILTER"},
    { 0x2802, "GL_TEXTURE_WRAP_S"},
    { 0x2803, "GL_TEXTURE_WRAP_T"},
    { 0x2901, "GL_REPEAT"},
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

NodeObject selected_node;

void draw_node(NodeObject &node, int& counter) {
    counter++;
    if (ImGui::Selectable(("Node: " + std::to_string(counter)).c_str())) {
        selected_node = node;
    }
    if (node.children.size() > 0) {

        if (ImGui::TreeNode(("Children: " + std::to_string(counter)).c_str())) {

            for (auto &node: node.children) {
            draw_node(node, counter);
            }
            ImGui::TreePop();
        }
    }
}

void draw_node_graph(NodeObject &root_node) {

    ImGui::Begin("glTF Inspector");
    ImGui::Columns(2, "tree", true);
    int counter = 0;
    draw_node(root_node, counter);

    ImGui::NextColumn();

    ImGui::Text("Node Selected");
    ImGui::Text("Children: %zu", selected_node.children.size());
    ImGui::Text("Mesh Primitives: %zu", selected_node.mesh.primitives.size());
    
    //glm::decompose(selected_node.transform_matrix, );
    ImGui::Text("Transform Matrix");
    auto& m = selected_node.get_matrix();
    float row_a[4] = {m[0][0], m[1][0], m[2][0], m[3][0]};
    float row_b[4] = {m[0][1], m[1][1], m[2][1], m[3][1]};
    float row_c[4] = {m[0][2], m[1][2], m[2][2], m[3][2]};
    float row_d[4] = {m[0][3], m[1][3], m[2][3], m[3][3]};
    ImGui::InputFloat4("", row_a);
    ImGui::InputFloat4("", row_b);
    ImGui::InputFloat4("", row_c);
    ImGui::InputFloat4("", row_d);

    ImGui::End();
}

TextureObject process_texture(std::shared_ptr<Model> &model, Texture& texture, std::string name, const std::shared_ptr<engine::Shader> &shader) {
    Sampler sampler; //Use default sampler if one is not specified
    if (texture.sampler > -1) {
        sampler = model->samplers[texture.sampler];
    }
    Image image = model->images[texture.source];
    GLuint tex_id = enginegl::buffer_image(sampler, image);
    ENGINE_INFO("Loaded image - {0}: {1}: {2}", name, tex_id, image.uri);
    //GLint texture_unit = shader->uniform_texture_unit(name);
    return TextureObject {tex_id, -1, name};
}

TextureObject process_default_texture(
    std::shared_ptr<Model> &model, std::string name, const std::shared_ptr<engine::Shader> &shader, unsigned char color = 0xff) {
    // No texture has been supplied - some require a default
    Sampler sampler; //Use default sampler if one is not specified
    Image image;
    image.name = "default_" + name;
    image.width = 2;
    image.height = 2;
    image.component = 3;
    image.pixel_type = 5121;
    image.image = {
        color, color, color,
        color, color, color,
        color, color, color,
        color, color, color,
    };
    image.uri = "default_" + name;

    GLuint tex_id = enginegl::buffer_image(sampler, image);
    ENGINE_INFO("Loaded image - {0}: {1}: {2}", name, tex_id, image.uri);
    //GLint texture_unit = shader->uniform_texture_unit(name);
    return TextureObject {tex_id, -1, name};
}

MaterialObject process_material(std::shared_ptr<Model> &model, Material &material,
        const std::shared_ptr<engine::Shader> &shader) {
    MaterialObject material_object;
    // The baseColorFactor has a specification guaranteed default of (1,1,1,1)
    material_object.color = glm::vec4(
        material.pbrMetallicRoughness.baseColorFactor[0],
        material.pbrMetallicRoughness.baseColorFactor[1],
        material.pbrMetallicRoughness.baseColorFactor[2],
        material.pbrMetallicRoughness.baseColorFactor[3]
    );
    // TODO: How to avoid hardcoding these names?
    int texture_index = material.pbrMetallicRoughness.baseColorTexture.index;
    if (texture_index > -1) {
        material_object.textures.push_back(
            process_texture(model, model->textures[texture_index], "albedo", shader)
        );
    }
    int normal_index = material.normalTexture.index;
    if (normal_index > -1) {
        material_object.textures.push_back(
            process_texture(model, model->textures[normal_index], "normal", shader)
        );
    }
    int mr_index = material.pbrMetallicRoughness.metallicRoughnessTexture.index;
    // TODO: Handle when roughness is seperate to metallic?
    if (mr_index > -1) {
        material_object.textures.push_back(
            process_texture(model, model->textures[mr_index], "roughness_metallic", shader)
        );
    }
    int ao_index = material.occlusionTexture.index;
    if (ao_index > -1) {
        material_object.textures.push_back(
            process_texture(model, model->textures[ao_index], "ambient", shader)
        );
    } else {
        material_object.textures.push_back(
            process_default_texture(model, "ambient", shader, 0xff)
        );
    }
    int emission_index = material.emissiveTexture.index;
    if (emission_index > -1) {
        material_object.textures.push_back(
            process_texture(model, model->textures[emission_index], "emission", shader)
        );
    } else if (shader->uniform_supported("emission")) {
        material_object.textures.push_back(
            process_default_texture(model, "emission", shader, 0x00)
        );
    }
    return material_object;
}

std::shared_ptr<engine::IndexBuffer> process_indices(std::shared_ptr<Model> &model, BufferView &buffer_view, Accessor &accessor) {
    ENGINE_TRACE("Processed Indices; Type: {0}, Count: {1}, Length: {2}, Offset: {3}",
        ACCESSOR_COMPONENT_TYPE[accessor.componentType],
        accessor.count, buffer_view.byteLength, accessor.byteOffset
    );
    int type = accessor.componentType; // TODO: Set the component type of the IndexBuffer
    return engine::IndexBuffer::create(
        &model->buffers[buffer_view.buffer].data.at(0) + buffer_view.byteOffset,
        accessor.count,
        buffer_view.byteLength,
        accessor.byteOffset
    );
}

std::shared_ptr<engine::VertexBuffer> process_buffer_view(std::shared_ptr<Model> &model, BufferView &buffer_view) {
    ENGINE_TRACE("Processed Vertex Buffer; Offset: {0}, Length: {1}", buffer_view.byteOffset, buffer_view.byteLength);
    return engine::VertexBuffer::create(
        &model->buffers[buffer_view.buffer].data.at(0) + buffer_view.byteOffset,
        buffer_view.byteLength
    );
}

void describe_buffer(Accessor &accessor, uint32_t location, uint32_t stride) {
    ENGINE_TRACE("glVertexAttribPointer({0}, {1}, {2}, {3}, {4}, {5})",
        location,
        accessor.type,
        ACCESSOR_COMPONENT_TYPE[accessor.componentType],
        accessor.normalized ? "GL_TRUE": "GL_FALSE",
        stride,
        accessor.byteOffset
    );
    glEnableVertexAttribArray(location);
    glVertexAttribPointer(
        location,
        accessor.type,
        accessor.componentType,
        accessor.normalized ? GL_TRUE : GL_FALSE,
        stride,
        (const void *)(accessor.byteOffset)
    );
}

std::map<int, std::shared_ptr<engine::IndexBuffer>> common_index_buffers;
std::map<int, std::shared_ptr<engine::VertexBuffer>> common_buffers;
std::map<int, MaterialObject> map_materials;
std::map<int, MeshObject> map_meshes;

MeshObject process_mesh(
        ModelObjects& m_obj, std::shared_ptr<Model> &model, Mesh &mesh,
        const std::shared_ptr<engine::Shader> &shader) {
    MeshObject mesh_object;
    for (Primitive &primitive: mesh.primitives) {
        PrimitiveObject primitive_object;
        primitive_object.vao = engine::VertexArray::create();
        m_obj.vaos.push_back(primitive_object.vao);
        auto &vao = m_obj.vaos.back();
        vao->bind();
        if (primitive.indices != -1) {
            Accessor accessor = model->accessors[primitive.indices];
            if (accessor.sparse.isSparse) {
                ENGINE_WARN("Warning, Sparse accessor detected, skipping");
                continue;
            }
            BufferView buffer_view = model->bufferViews[accessor.bufferView];
            // Buffer deduplication
            if (common_index_buffers.count(accessor.bufferView) == 0) {
                auto index_buffer = process_indices(model, buffer_view, accessor);
                common_index_buffers[accessor.bufferView] = index_buffer;
                vao->set_index_buffer(index_buffer);
            } else {
                auto base_buffer = common_index_buffers[accessor.bufferView];
                auto index_buffer = engine::IndexBuffer::create(
                    base_buffer, accessor.count, accessor.byteOffset);
                vao->set_index_buffer(index_buffer);
                ENGINE_TRACE("Processed Indices; Type: {0}, Count: {1}, Length: {2}, Offset: {3}",
                    ACCESSOR_COMPONENT_TYPE[accessor.componentType], 
                    accessor.count, buffer_view.byteLength, accessor.byteOffset
                );
            }
        }
        if (primitive.material != -1) {
            primitive_object.material = map_materials[primitive.material];
        }
        // Process primitive attribute data
        for (auto &[name, accessor_view_index]: primitive.attributes){
            Accessor accessor = model->accessors[accessor_view_index];
            BufferView buffer_view = model->bufferViews[accessor.bufferView];
            // We choose not buffer unsupported data?
            // Alternatively we could buffer it and only recreate the vertexattribpointers
            ENGINE_INFO("Primitive Supports {0}", name);
            if (shader->attribute_supported(name)) {
            //{
                if (common_buffers.count(accessor.bufferView) == 0) {
                    auto vertex_buffer = process_buffer_view(model, buffer_view);  // This creates the VBOs
                    common_buffers[accessor.bufferView] = vertex_buffer;
                } else {
                    common_buffers[accessor.bufferView]->bind();
                }
                vao->add_vertex_buffer(common_buffers[accessor.bufferView], false);
                // We also need to describe this VBO
                GLuint attr_loc = shader->attribute_location(name);
                describe_buffer(accessor, attr_loc, buffer_view.byteStride);
            }
        }
        vao->unbind();
        mesh_object.primitives.push_back(primitive_object);
    }
    return mesh_object;
}

NodeObject process_node(std::shared_ptr<Model> &model, Node &node) {
    NodeObject node_object {};
    if (node.matrix.size() == 16) {
        glm::mat4 matrix {
            node.matrix[0],  node.matrix[1],  node.matrix[2],  node.matrix[3],
            node.matrix[4],  node.matrix[5],  node.matrix[6],  node.matrix[7],
            node.matrix[8],  node.matrix[9],  node.matrix[10], node.matrix[11],
            node.matrix[12], node.matrix[13], node.matrix[14], node.matrix[15]
        };
        glm::vec3 scale;
        glm::quat rotation;
        glm::vec3 translation;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(matrix, scale, rotation, translation, skew, perspective);
        node_object.set_translation(translation);
        node_object.set_rotation(rotation);
        node_object.set_scale(scale);
    }
    if (node.translation.size() == 3) {
        node_object.set_translation(glm::make_vec3(node.translation.data()));
    }
    if (node.scale.size() == 3) {
        node_object.set_scale(glm::make_vec3(node.scale.data()));
    }
    if (node.rotation.size() == 4) {
        node_object.set_rotation(glm::make_quat( node.rotation.data()));
    }
    // Load our mesh
    if (node.mesh != -1) {
        node_object.mesh = map_meshes[node.mesh];

    }
    for (int child_index: node.children) {
        node_object.children.push_back(
            process_node(model, model->nodes[child_index])
        );
    }
    return node_object;
}

/*
Primitive - VAO, + material (textures + other data)
Mesh - Collection of Primitives
Node - Mesh + relative_transform
*/

NodeObject gltf_to_opengl(ModelObjects& m_obj, std::shared_ptr<Model> &model, const std::shared_ptr<engine::Shader> &shader) {
    common_buffers.clear();
    common_index_buffers.clear();
    map_materials.clear();
    map_meshes.clear();

    // TODO - Remove the shader dependency - this should just load all data
    // the shader would make the decision on if the data is useful

    int counter = 0;
    for (Material &material: model->materials) {
        map_materials[counter] = process_material(model, material, shader);
        counter++;
    }
    
    counter = 0;
    for (Mesh &mesh: model->meshes) {
        map_meshes[counter] = process_mesh(m_obj, model, mesh, shader);
        counter++;
    }

    NodeObject node_object;
    for (Scene &scene: model->scenes) {
        for (int &node_index: scene.nodes) {
            node_object.children.push_back(
                process_node(model, model->nodes[node_index])
            );
        }
    }
    return node_object;
}
