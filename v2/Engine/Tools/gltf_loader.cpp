#include "engine.hpp"

#include "gltf_loader.hpp"
#include "Engine/renderer/buffer.hpp"
//#include "Engine/entity.hpp"

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

void describe_material(std::shared_ptr<Model> &model, int material_index, std::string indent="") {
    Material material = model->materials[material_index];
    ENGINE_INFO(indent + "          + name: {0}", material.name);
    ENGINE_INFO(indent + "          - Base texture");
    ENGINE_INFO(indent + "            + Index: {0}", material.pbrMetallicRoughness.baseColorTexture.index);
    ENGINE_INFO(indent + "            + TexCoord: {0}", material.pbrMetallicRoughness.baseColorTexture.texCoord);
    if (material.pbrMetallicRoughness.baseColorTexture.index == -1) {
        return;
    }
    Texture texture = model->textures[material.pbrMetallicRoughness.baseColorTexture.index];
    Sampler sampler;
    if (texture.sampler > 0) {
        Sampler sampler = model->samplers[texture.sampler];
    }
    ENGINE_INFO(indent + "            - Sampler: {0}", TEXTURE_NAMES[texture.sampler]);
    ENGINE_INFO(indent + "              + minFilter: {0}", TEXTURE_NAMES[sampler.minFilter]);
    ENGINE_INFO(indent + "              + magFilter: {0}", TEXTURE_NAMES[sampler.magFilter]);
    ENGINE_INFO(indent + "              + wrapS: {0}", TEXTURE_NAMES[sampler.wrapS]);
    ENGINE_INFO(indent + "              + wrapT: {0}", TEXTURE_NAMES[sampler.wrapT]);
    ENGINE_INFO(indent + "              + wrapR: {0}", TEXTURE_NAMES[sampler.wrapR]);
    Image image = model->images[texture.source];
    ENGINE_INFO(indent + "            - Image: {0}", texture.source);
    ENGINE_INFO(indent + "              + File: {0}", image.uri);
    ENGINE_INFO(indent + "              + Pixel Type: {0}", ACCESSOR_COMPONENT_TYPE[image.pixel_type]);
    ENGINE_INFO(indent + "              + Size: {0}x{1}", image.width, image.height);
}

void describe_mesh(std::shared_ptr<Model> &model, Mesh &mesh, std::string indent="") {
    ENGINE_INFO(indent + "    - Mesh - {0}", mesh.name);
    for (auto primitive: mesh.primitives) {
        ENGINE_INFO(indent + "      - Primitive");
        ENGINE_INFO(indent + "        + Mode: {0}", PRIMITIVE_DRAW_MODE[primitive.mode]);
        if (primitive.indices != -1)
            ENGINE_INFO(indent + "        + Indices: {0}", primitive.indices);
        if (primitive.material != -1)
            ENGINE_INFO(indent + "        - Material", primitive.material);
            describe_material(model, primitive.material);
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
        describe_mesh(model, mesh, indent);
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

bool walk_nodes(Node &selected_node, Node &current_node, std::shared_ptr<Model> &model) {
    bool node_selected = false;
    if (ImGui::TreeNode("Nodes")) {
        for (int node_index: current_node.children) {
            Node node = model->nodes[node_index];
            if(ImGui::Selectable(node.name.c_str())) {
                selected_node = node;
                node_selected = true;
            }
            if (node.children.size() > 0) {
                walk_nodes(selected_node, node, model);
            }
        }
        ImGui::TreePop();
    }
    return node_selected;
}

void gltf_inspector(bool display, std::shared_ptr<Model> &model) {
    if (!display) return;
    ImGui::Begin("glTF Inspector");
    ImGui::Columns(2, "tree", true);
    // Scenes
    static Scene selected_scene;
    static Node selected_node;
    static Mesh selected_mesh;
    static Material selected_material;

    static bool node_selected = false;
    static bool scene_selected = false;
    static bool mesh_selected = false;
    static bool material_selected = false;

    auto select = [&](bool& selector) {
        scene_selected = false;
        node_selected = false;
        mesh_selected = false;
        material_selected = false;
        selector = true;
    };

    if (ImGui::TreeNode("Scenes")) {
        for (Scene scene: model->scenes) {
            if(ImGui::Selectable(scene.name.c_str())) {
                selected_scene = scene;
                select(scene_selected);
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Nodes")) {
        // Hmm how to deal with multiple scenes?
        
        for (int node_index: model->scenes[0].nodes) {
            Node node = model->nodes[node_index];
            if(ImGui::Selectable(node.name.c_str())) {
                selected_node = node;
                select(node_selected);
            }
            if(walk_nodes(selected_node, node, model)) {
                select(node_selected);
            }
        }
        /*
        for (Node node: model->nodes) {
            if(ImGui::Selectable(node.name.c_str())) {
                selected_node = node;
                select(node_selected);
            }
        }
        */
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Meshes")) {
        for (Mesh mesh: model->meshes) {
            if(ImGui::Selectable(mesh.name.c_str())) {
                selected_mesh = mesh;
                select(mesh_selected);
            }
        }
        ImGui::TreePop();
    }
    if (ImGui::TreeNode("Materials")) {
        for (Material material: model->materials) {
            if(ImGui::Selectable("a")) {
                selected_material = material;
                select(material_selected);
            }
        }
        ImGui::TreePop();
    }
    ImGui::NextColumn();
    ImGui::Text("Selected");
    if (scene_selected) {
        ImGui::Text("Node count: %zu", selected_scene.nodes.size());
        
    }
    if (node_selected) {
        ImGui::Text("Name: %s", selected_node.name.c_str());
        ImGui::Text("Children: %zu", selected_node.children.size());
        ImGui::Text("Camera: %i", selected_node.camera);
        ImGui::Text("Mesh: %i", selected_node.mesh);
    }

    if (mesh_selected) {
        ImGui::Text("Name: %s", selected_mesh.name.c_str());
        ImGui::Text("Mesh Primitives: %zu", selected_mesh.primitives.size());

        for (Primitive primitive: selected_mesh.primitives) {
            if (ImGui::TreeNode("Primitives")) {
                ImGui::Text("Mode: %s", PRIMITIVE_DRAW_MODE[primitive.mode].c_str());
                ImGui::Text("Material Indices: %i", primitive.indices);
                ImGui::Text("Material Index: %i", primitive.material);
                ImGui::Separator();
                for (auto attr: primitive.attributes) {
                    ImGui::Text("Attribute: %s, %i", attr.first.c_str(), attr.second);
                }
                ImGui::TreePop();
            }
        }
    }
    ImGui::End();
}

uint32_t process_material(std::shared_ptr<Model> &model, Material &material) {
    int texture_index = material.pbrMetallicRoughness.baseColorTexture.index;
    if (texture_index > -1) {
        Texture texture = model->textures[texture_index];
        Sampler sampler; //Use default sampler if one is not specified
        if (texture.sampler > -1) {
            sampler = model->samplers[texture.sampler];
        }
        Image image = model->images[texture.source];
        GLuint tex_id = enginegl::buffer_image(sampler, image);
        ENGINE_INFO("Loaded image - {0}", tex_id);
        return tex_id;
    }
    return -1;
}

std::shared_ptr<engine::IndexBuffer> process_indices(std::shared_ptr<Model> &model, BufferView &buffer_view, uint32_t count) {
    ENGINE_TRACE("Processed Indices; Count: {0}, Length: {1}", count, buffer_view.byteLength);
    return engine::IndexBuffer::create(
        &model->buffers[buffer_view.buffer].data.at(0) + buffer_view.byteOffset,
        count,
        buffer_view.byteLength
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

void process_mesh(
        ModelObjects& m_obj, std::shared_ptr<Model> &model, Mesh &mesh,
        const std::shared_ptr<engine::Shader> &shader) {
    for (Primitive &primitive: mesh.primitives) {
        ENGINE_TRACE("VAO create");
        m_obj.vaos.push_back(engine::VertexArray::create());
        auto &vao = m_obj.vaos.back();
        vao->bind();
        if (primitive.indices != -1) {
            Accessor accessor = model->accessors[primitive.indices];
            BufferView buffer_view = model->bufferViews[accessor.bufferView];
            if (common_index_buffers.count(accessor.bufferView) == 0) {
                auto index_buffer = process_indices(model, buffer_view, accessor.count);
                common_index_buffers[accessor.bufferView] = index_buffer;
            } else {
                common_index_buffers[accessor.bufferView]->bind();
            }
            vao->set_index_buffer(common_index_buffers[accessor.bufferView]);
        }
        if (primitive.material != -1) {
            m_obj.texture_ids.push_back(
                process_material(model, model->materials[primitive.material])
            );
        }
        // Process primitive attribute data
        ENGINE_TRACE("--------ATTRIBUTES----------");
        for (auto &[name, accessor_view_index]: primitive.attributes){
            ENGINE_TRACE("----- {0}", name);
            Accessor accessor = model->accessors[accessor_view_index];
            BufferView buffer_view = model->bufferViews[accessor.bufferView];
            // We choose not buffer unsupported data?
            // Alternatively we could buffer it and only recreate the vertexattribpointers
            if (shader->attribute_supported(name)) {
                if (common_buffers.count(accessor.bufferView) == 0) {
                    auto vertex_buffer = process_buffer_view(model, buffer_view);  // This creates the VBOs
                    common_buffers[accessor.bufferView] = vertex_buffer;
                } else {
                    common_buffers[accessor.bufferView]->bind();
                }
                vao->add_vertex_buffer(common_buffers[accessor.bufferView], false);
                // We also need to describe this VBO
                GLuint attr_loc = shader->attribute_location(name);
                ENGINE_TRACE("Describing index {0} as {1}", attr_loc, name);
                describe_buffer(accessor, attr_loc, buffer_view.byteStride);
            }
            ENGINE_TRACE("--------------------------");
        }
        vao->unbind();
        
        ENGINE_TRACE("VAO Setup finished");
    }
}

void process_node(
        ModelObjects& m_obj, std::shared_ptr<Model> &model, Node &node, 
        const std::shared_ptr<engine::Shader> &shader
    ) {
    if (node.mesh != -1) {
        //process_mesh_two(m_obj, model, model->meshes[node.mesh], shader);
    }
    for (int child_index: node.children) {
        process_node(m_obj, model, model->nodes[child_index], shader);
    }
}

/*
Primitive - VAO, + material (textures + other data)
Mesh - Collection of Primitives
Node - Mesh + relative_transform
*/

void gltf_to_opengl(ModelObjects& m_obj, std::shared_ptr<Model> &model, const std::shared_ptr<engine::Shader> &shader) {
    ENGINE_INFO("Processing GLtf");
    common_buffers.clear();
    common_index_buffers.clear();

    for (Mesh &mesh: model->meshes) {
        process_mesh(m_obj, model, mesh, shader);
    }

    for (Scene &scene: model->scenes) {
        for (int &node_index: scene.nodes) {
            process_node(m_obj, model, model->nodes[node_index], shader);
        }
    }
}
