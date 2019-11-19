#ifndef _MANAGER_HPP
#define _MANAGER_HPP

#include "Engine/renderer/camera.hpp"
#include "Engine/renderer/lights.hpp"
#include "Engine/entity.hpp"
#include "Engine/scripts.hpp"
#include "Engine/python_api.hpp"
#include "Engine/transform.hpp"

namespace engine {

template<typename T>
using Ref = std::shared_ptr<T>;

void process_asset(std::string file_path, std::string name);

void deserialise_object ();

} // namespace

#endif