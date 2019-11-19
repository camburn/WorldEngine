#include "engine.hpp"
#include "manager.hpp"

#include <json.hpp>
using json = nlohmann::json;

namespace engine {



void process_asset(std::string file_path, std::string name) {

}

void deserialise_assets () {
    std::ifstream assets_config("./config/assets.json");
    json assets_json;
    assets_config >> assets_json;

    for (auto &object: assets_json) {
        entities[object["name"]] = GltfEntity::load_from_file(object["path"]);
        entities[object["name"]]->name = object["name"];
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
            py_scripts[json_id].reset(new PythonScript(json_script, m_objects[json_id]));
            m_objects[json_id]->attach(py_scripts[json_id]);
        }
        if (json_obj.contains("mesh")) {
            std::string json_mesh = json_obj["mesh"].get<std::string>();
            m_objects[json_id]->attach(entities[json_mesh]);
        }
    }
}

} // namespace