#include "managers/mesh_manager.hpp"

MeshInstance::MeshInstance(
    glm::vec3 position,
    glm::vec3 rotation,
    glm::vec3 scale,
    std::string filename,
    std::string path 
    ) : position(position), rotation(rotation), scale(scale) {
    model = Model(path, filename);
}

void MeshInstance::set_position(glm::vec3 new_position) {
    invalid_model_matrix = true;
    position = new_position;
}

void MeshInstance::set_rotation(glm::vec3 new_rotation) {
    invalid_model_matrix = true;
    rotation = new_rotation;
}

void MeshInstance::set_scale(glm::vec3 new_scale) {
    invalid_model_matrix = true;
    scale = new_scale;
}

glm::vec3 MeshInstance::get_position() {
    return position;
}

glm::vec3 MeshInstance::get_rotation() {
    return rotation;
}

glm::vec3 MeshInstance::get_scale() {
    return scale;
}

glm::mat4 MeshInstance::get_model_matrix() {
    if (invalid_model_matrix) {
        model_matrix = glm::mat4(1.0f);
        model_matrix = glm::translate(model_matrix, position);
        if (rotation.x != 0.0f) {
            model_matrix = glm::rotate(model_matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        }
        if (rotation.y != 0.0f) {
            model_matrix = glm::rotate(model_matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        }
        if (rotation.z != 0.0f) {
            model_matrix = glm::rotate(model_matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        }
        model_matrix = glm::scale(model_matrix, scale);
        invalid_model_matrix = false;
    }
    return model_matrix;
}

glm::mat3 MeshInstance::get_normal_matrix() {
    return (glm::mat3)glm::transpose(glm::inverse(
        MeshInstance::get_model_matrix()
    ));
}

glm::mat4 MeshInstance::get_mvp_matrix(glm::mat4 view_proj_matrix) {
    return view_proj_matrix * MeshInstance::get_model_matrix();
}



MeshManager::MeshManager(State &state, TextureManager &texture_manager) : 
        textures(texture_manager), state(state) {
    program = state.get_program();
}

unsigned int MeshManager::new_instance(
        std::string filename, 
        std::string path, 
        glm::vec3 pos
    ) {
    instance_count ++;

    MeshInstance instance(
        pos,
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1),
        filename,
        path
    ); 

    instances.push_back(instance);
    
    return instances.size() - 1;
}

MeshInstance &MeshManager::get_instance(unsigned int instance_id) {
    return instances[instance_id];
}

void MeshManager::update_instance_position(unsigned int instance_id, glm::vec3 position) {
    instances[instance_id].set_position(position);
}

void MeshManager::update_instance_rotation(unsigned int instance_id, glm::vec3 rotation) {
    instances[instance_id].set_rotation(rotation);
}

void MeshManager::update_instance_scale(unsigned int instance_id, glm::vec3 scale) {
    instances[instance_id].set_scale(scale);
}

glm::vec3 MeshManager::get_instance_position(unsigned int instance_id) {
    return instances[instance_id].get_position();
}

glm::vec3 MeshManager::get_instance_rotation(unsigned int instance_id) {
    return instances[instance_id].get_rotation();
}

glm::vec3 MeshManager::get_instance_scale(unsigned int instance_id) {
    return instances[instance_id].get_scale();
}

void MeshManager::draw() {
    glm::mat4 model_view_projection = state.generate_model_view();
    for (MeshInstance prim: instances) {
        glm::mat4 mvp = prim.get_mvp_matrix(model_view_projection);
        glm::mat3 normal_mat = prim.get_normal_matrix();
        state.renderer.active().set_uniform("MVP", mvp);
        state.renderer.active().set_uniform("NormalMat", normal_mat);
        state.renderer.active().set_uniform("texture_diffuse1", 0);
        state.renderer.active().set_uniform("use_uniform_color", false);
        state.renderer.active().set_uniform("use_shadows", false);

        prim.model.Draw(state.renderer.active().get_shader_id());
    }
    state.renderer.active().set_uniform("use_uniform_color", false);
}