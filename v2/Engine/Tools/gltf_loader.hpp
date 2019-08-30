#ifndef _GLTF_LOADER_HPP
#define _GLTF_LOADER_HPP
#include <string>
#include <memory>
#include <map>
#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/vertex_array.hpp"

#include "glad/glad.h"

#include "tiny_gltf.h"

struct ModelObjects {
    std::vector<GLuint> texture_ids;
    std::vector<std::shared_ptr<engine::VertexArray>> vaos;
};

struct MaterialObject {
    bool texture_set = false;
    GLuint texture_id;
    glm::vec4 color;
};

struct PrimitiveObject {
    MaterialObject material;
    std::shared_ptr<engine::VertexArray> vao; // Contains VBOs, IBO
    // Who stores the IBO offset?
};

struct MeshObject {
    std::vector<PrimitiveObject> primitives;
};

struct NodeObject {
    NodeObject(): 
        transform_matrix(glm::mat4(1.0f)), rotation(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)),
        transform(glm::vec3(0.0f)), scale(glm::vec3(1.0f)) { }

    std::vector<NodeObject> children;
    MeshObject mesh;
    glm::mat4 transform_matrix;
    glm::vec4 rotation;
    glm::vec3 transform;
    glm::vec3 scale;
};

struct DrawObject {

};

std::shared_ptr<engine::VertexArray> mesh_loader(std::string filename, const std::shared_ptr<engine::Shader> &shader);
NodeObject gltf_to_opengl(ModelObjects& m_obj, std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<engine::Shader> &shader);
void draw_node_graph(NodeObject &root_node);

#endif