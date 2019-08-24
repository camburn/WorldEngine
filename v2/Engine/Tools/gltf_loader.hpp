#ifndef _GLTF_LOADER_HPP
#define _GLTF_LOADER_HPP
#include <string>
#include <memory>
#include <map>
#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/vertex_array.hpp"

#include "glad/glad.h"

#include "tiny_gltf.h"

struct ModelObject {
    std::vector<GLuint> texture_ids;
    std::shared_ptr<engine::VertexArray> vao;
};

std::shared_ptr<engine::VertexArray> mesh_loader(std::string filename, const std::shared_ptr<engine::Shader> &shader);
void gltf_to_opengl(ModelObject& m_obj, std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<engine::Shader> &shader);
void gltf_inspector(bool display, std::shared_ptr<tinygltf::Model> &model);

#endif