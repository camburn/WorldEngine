#ifndef _GLTF_LOADER_HPP
#define _GLTF_LOADER_HPP
#include <string>
#include <memory>
#include <map>
#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/vertex_array.hpp"

#include "glad/glad.h"

#include "tiny_gltf.h"

std::shared_ptr<engine::VertexArray> mesh_loader(std::string filename, const std::shared_ptr<engine::Shader> &shader);
std::shared_ptr<engine::VertexArray> gltf_to_opengl(std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<engine::Shader> &shader);

#endif