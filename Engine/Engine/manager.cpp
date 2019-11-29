#include "engine.hpp"
#include "manager.hpp"

#include <json.hpp>
#include <fstream>
using json = nlohmann::json;

namespace engine {

std::map<std::string, std::shared_ptr<Object>> m_objects;
std::map<std::string, std::shared_ptr<Entity>> m_entities;
std::map<std::string, std::shared_ptr<Texture>> m_textures;
std::map<std::string, std::shared_ptr<PythonScript>> m_py_scripts;
//std::map<std::string, std::shared_ptr<Camera>> cameras;
//std::map<std::string, std::shared_ptr<Light>> lights;

void process_asset(std::string file_path, std::string name) {

}

void deserialise_assets () {
    // Load default assets
    m_entities["primitive_sphere"] = generate_sphere();
    m_entities["primitive_sphere"]->name = "Sphere Mesh";

    // Process configured assets
    std::ifstream assets_config("./config/assets.json");
    json assets_json;
    assets_config >> assets_json;

    for (auto &object: assets_json) {
        if (object["type"] == "mesh") {
            m_entities[object["name"]] = GltfEntity::load_from_file(object["path"]);
            m_entities[object["name"]]->name = object["name"];
        } else if (object["type"] == "texture") {
            m_textures[object["name"]] = Texture2D::create(object["path"]);
        }
    }
}

glm::vec3 deserialise_vec3(json vector){
    std::vector<float> data = vector.get<std::vector<float>>();
    return glm::vec3{data[0], data[1], data[2]};
}

glm::vec4 deserialise_vec4(json vector){
    std::vector<float> data = vector.get<std::vector<float>>();
    return glm::vec4{data[0], data[1], data[2], data[3]};
}

glm::quat deserialise_quat(json vector){
    std::vector<float> data = vector.get<std::vector<float>>();
    return glm::quat{data[0], data[1], data[2], data[3]};
}

Transform deserialise_transform(json transform_obj) {
    Transform t;
    if (transform_obj.contains("translation")) {
        t.set_translation(deserialise_vec3(transform_obj["translation"]));
    }
    if (transform_obj.contains("scale")) {
        t.set_scale(deserialise_vec3(transform_obj["scale"]));
    }
    if (transform_obj.contains("rotation")) {
        t.set_rotation(deserialise_quat(transform_obj["rotation"]));
    }
    return t;
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
            m_objects[json_id]->set_transform(deserialise_transform(json_obj["transform"]));
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

        if (json_obj.contains("primitive")) {
            //TODO: Primitive should be one of [sphere, ...]
            std::string mesh_name = "primitive_" + json_obj["primitive"].get<std::string>();
            m_objects[json_id]->attach(m_entities[mesh_name]);
        }

        if (json_obj.contains("textures")) {
            //TODO: Textures are only support on primitive objects (CustomEntity)
            for (auto &json_texture: json_obj["textures"]) {
                std::string tex_type = json_texture["type"].get<std::string>();
                std::string tex_id = json_texture["texture"].get<std::string>();
                std::static_pointer_cast<CustomEntity>(m_objects[json_id]->mesh())->add_texture(
                    tex_type, m_textures[json_texture["texture"]]
                );
            }
        }

        if (json_obj.contains("light")) {
            auto &light_obj = json_obj["light"];
            glm::vec3 transform {0.0f, 0.0f, 0.0f};
            glm::vec3 color {1.0f, 1.0f, 1.0f};
            bool directional_light = false;
            if (light_obj.contains("translation")) {
                transform = deserialise_vec3(light_obj["translation"]);
            }
            if (light_obj.contains("color")) {
                color = deserialise_vec3(light_obj["color"]);
            }
            if (light_obj.contains("type")) {
                if (light_obj["type"] == "point") {
                    directional_light = false;
                } else if (light_obj["type"] == "direction") {
                    directional_light = true;
                }
            }
            m_objects[json_id]->attach(std::shared_ptr<Light> {
                new Light{color, transform, directional_light}
            });
        }

        if (json_obj.contains("collider")) {
            m_objects[json_id]->attach(
                std::shared_ptr<BoxCollider>(
                    new BoxCollider{
                        deserialise_transform(json_obj["collider"]["transform"]).get_translation(),
                        deserialise_vec3(json_obj["collider"]["size"])
                    }
                )
            );
        }
    }
}

} // namespace