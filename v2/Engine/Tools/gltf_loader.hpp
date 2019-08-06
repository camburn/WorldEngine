#ifndef _GLTF_LOADER_HPP
#define _GLTF_LOADER_HPP
#include <string>
#include <memory>
#include "Engine/renderer/shader.hpp"

unsigned int mesh_loader(std::string filename, std::shared_ptr<engine::Shader> shader);

#endif