#ifndef _MANAGER_HPP
#define _MANAGER_HPP

#include "Engine/renderer/camera.hpp"
#include "Engine/renderer/lights.hpp"
#include "Engine/entity.hpp"
#include "Engine/scripts.hpp"
#include "Engine/python_api.hpp"
#include "Engine/transform.hpp"
#include "Tools/generate_sphere.hpp"

namespace engine {

extern std::map<std::string, std::shared_ptr<Object>> m_objects;
extern std::map<std::string, std::shared_ptr<Entity>> m_entities;
extern std::map<std::string, std::shared_ptr<PythonScript>> m_py_scripts;

void process_asset(std::string file_path, std::string name);

void deserialise_object ();
void deserialise_assets ();

} // namespace

#endif