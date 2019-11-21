#include "engine.hpp"
#include "manager.hpp"

#include <json.hpp>
#include <fstream>
using json = nlohmann::json;

namespace engine {

std::map<std::string, std::shared_ptr<Object>> m_objects;
std::map<std::string, std::shared_ptr<Entity>> m_entities;
std::map<std::string, std::shared_ptr<PythonScript>> m_py_scripts;
//std::map<std::string, std::shared_ptr<Camera>> cameras;
//std::map<std::string, std::shared_ptr<Light>> lights;

void process_asset(std::string file_path, std::string name) {

}

void deserialise_assets () {
    std::ifstream assets_config("./config/assets.json");
    json assets_json;
    assets_config >> assets_json;

    for (auto &object: assets_json) {
        m_entities[object["name"]] = GltfEntity::load_from_file(object["path"]);
        m_entities[object["name"]]->name = object["name"];
    }
}

void deserialise_object () {
    std::ifstream objects_config("./config/objects.json");
    json objects_json;
    objects_config >> objects_json;

    for (auto &json_obj: objects_json) {
        std::string json_id = json_obj["id"].get<std::string>();
        std::string json_name = json_obj["name"].get<std::string>();
        ENGINE_INFO("Object: {0}", json_name);
        m_objects[json_id].reset(new Object());
        m_objects[json_id]->name = json_name;

        // Process Tranforms
        if (json_obj.contains("transform")) {
            auto &transform_obj = json_obj["transform"];
            if (transform_obj.contains("translation")) {
                std::vector<float> transform = transform_obj["translation"].get<std::vector<float>>();
                m_objects[json_id]->transform().set_translation(
                    glm::vec3(transform[0], transform[1], transform[2]));
            }
            if (transform_obj.contains("scale")) {
                std::vector<float> scale = transform_obj["scale"].get<std::vector<float>>();
                m_objects[json_id]->transform().set_scale(
                    glm::vec3(scale[0], scale[1], scale[2]));
            }
            if (transform_obj.contains("rotation")) {
                std::vector<float> rot = transform_obj["rotation"].get<std::vector<float>>();
                m_objects[json_id]->transform().set_rotation(
                    glm::quat(rot[0], rot[1], rot[2], rot[3]));
            }
        }

        if (json_obj.contains("script")) {
            std::string json_script = json_obj["script"].get<std::string>();
            m_py_scripts[json_id].reset(new PythonScript(json_script, m_objects[json_id]));
            m_objects[json_id]->attach(m_py_scripts[json_id]);
        }

        if (json_obj.contains("mesh")) {
            std::string json_mesh = json_obj["mesh"].get<std::string>();
            m_objects[json_id]->attach(m_entities[json_mesh]);
        }

        if (json_obj.contains("light")) {
            auto &light_obj = json_obj["light"];
            std::vector<float> transform {0.0f};
            std::vector<float> color {1.0f};
            bool directional_light = false;
            if (light_obj.contains("translation")) {
                transform = light_obj["translation"].get<std::vector<float>>();
            }
            if (light_obj.contains("color")) {
                color = light_obj["color"].get<std::vector<float>>();
            }
            if (light_obj.contains("type")) {
                if (light_obj["type"] == "point") {
                    directional_light = false;
                } else if (light_obj["type"] == "direction") {
                    directional_light = true;
                }
            }
            m_objects[json_id]->attach(std::shared_ptr<Light> {
                new Light{
                    glm::vec3(color[0], color[1], color[2]),
                    glm::vec3(transform[0], transform[1], transform[2]),
                    directional_light
                }
            });
        }
    }
}

} // namespace