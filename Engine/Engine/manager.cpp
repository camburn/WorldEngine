#include "engine.hpp"
#include "manager.hpp"

namespace engine {

std::map<std::string, std::shared_ptr<Object>> objects;
std::map<std::string, std::shared_ptr<Entity>> entities;
std::map<std::string, std::shared_ptr<PythonScript>> py_scripts;
std::map<std::string, std::shared_ptr<Camera>> cameras;
std::map<std::string, std::shared_ptr<Light>> lights;

void process_asset(std::string file_path, std::string name) {
    entities[name] = GltfEntity::load_from_file(file_path);
    entities[name]->name = name;
}

struct SerialObject {
    std::string name;
    std::string mesh_name;
    std::string script_name;
    Transform transform;
};

void deserialise_object () {
    objects["flight_helmet"].reset(new Object());
    objects["flight_helmet"]->attach(entities["flight_helmet"]);
    objects["flight_helmet"]->transform().set_translation(glm::vec3(3, 0, 0));
    objects["flight_helmet"]->transform().set_scale(glm::vec3(4, 4, 4));
    objects["flight_helmet"]->name = "Flight Helmet";
    py_scripts["flight_helmet"].reset(
        new PythonScript("flight_helmet", objects["flight_helmet"] )
    );
    objects["flight_helmet"]->attach(py_scripts["flight_helmet"]);
}

} // namespace