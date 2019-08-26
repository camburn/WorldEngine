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
    GLuint texture_id;
    glm::vec4 color;
};

struct PrimitiveObject {
    MaterialObject material;
    std::shared_ptr<engine::VertexArray> vao; // Contains VBOs, IBO
};

struct MeshObject {
    std::vector<PrimitiveObject> primitives;
};

struct NodeObject {
    std::shared_ptr<MeshObject> mesh;
    glm::mat4 transform;
    glm::vec4 rotation;
    glm::vec3 transform;
    glm::vec3 scale;
};

struct DrawObject {

};

std::shared_ptr<engine::VertexArray> mesh_loader(std::string filename, const std::shared_ptr<engine::Shader> &shader);
void gltf_to_opengl(MeshObject& m_obj, std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<engine::Shader> &shader);
void gltf_inspector(bool display, std::shared_ptr<tinygltf::Model> &model);

#endif