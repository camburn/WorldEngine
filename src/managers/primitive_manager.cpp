/*
Managers are responsible for initialising all foundation data for their type.

They then make available this data via names.
*/
#include "managers/primitive_manager.hpp"


PrimitiveInstance::PrimitiveInstance(
    GLuint mesh_id,
    GLuint tex_id,
    glm::vec3 pos,
    glm::vec3 rot,
    glm::vec3 scale,
    GLuint program,
    std::string name
): 
    mesh_id(mesh_id),
    tex_id(tex_id),
    position(pos),
    rotation(rot),
    scale(scale),
    program(program),
    name(name) {
        bool use_texture = true;
    }

PrimitiveInstance::PrimitiveInstance(
    GLuint mesh_id,
    vec3 uniform_color,
    glm::vec3 pos,
    glm::vec3 rot,
    glm::vec3 scale,
    GLuint program,
    std::string name,
    bool use_shading = true
): 
    mesh_id(mesh_id),
    uniform_color(uniform_color),
    position(pos),
    rotation(rot),
    scale(scale),
    program(program),
    name(name),
    use_shading(use_shading) {
        bool use_texture = false;
    }

void PrimitiveInstance::set_position(glm::vec3 new_position) {
    invalid_model_matrix = true;
    position = new_position;
}

void PrimitiveInstance::set_rotation(glm::vec3 new_rotation) {
    invalid_model_matrix = true;
    rotation = new_rotation;
}

void PrimitiveInstance::set_scale(glm::vec3 new_scale) {
    invalid_model_matrix = true;
    scale = new_scale;
}

glm::vec3 PrimitiveInstance::get_position() {
    return position;
}

glm::vec3 PrimitiveInstance::get_rotation() {
    return rotation;
}

glm::vec3 PrimitiveInstance::get_scale() {
    return scale;
}

bool PrimitiveInstance::get_texture_status() {
    return use_texture;
}

bool PrimitiveInstance::get_shading_status() {
    return use_shading;
}

void PrimitiveInstance::draw(GLuint array_size) {
    if (use_texture) {
        glBindTexture(GL_TEXTURE_2D, get_texture_id());
    }
    glBindVertexArray(mesh_id);
    glDrawArrays(GL_TRIANGLES, 0, array_size);
    glBindVertexArray(0);
}

GLuint PrimitiveInstance::get_texture_id() {
    return tex_id;
};

std::string PrimitiveInstance::get_type() {
    return name;
}

glm::mat4 PrimitiveInstance::get_model_matrix() {
    if (invalid_model_matrix) {
        model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        if (rotation.x != 0.0f) {
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1, 0, 0));
        }
        if (rotation.y != 0.0f) {
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0, 1, 0));
        }
        if (rotation.z != 0.0f) {
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0, 0, 1));
        }
        model = glm::scale(model, scale);
        invalid_model_matrix = false;
    }
    return model;
}

glm::mat3 PrimitiveInstance::get_normal_matrix() {
    return (glm::mat3)glm::transpose(glm::inverse(
        PrimitiveInstance::get_model_matrix()
    ));
}

glm::mat4 PrimitiveInstance::get_mvp_matrix(glm::mat4 view_proj_matrix) {
    return view_proj_matrix * PrimitiveInstance::get_model_matrix();
}


PrimitiveManager::PrimitiveManager(State &state, TextureManager &texture_manager) 
        : textures(texture_manager), state(state) {
    program = state.get_program();
    GLuint CubeMesh = primitives::cube_mesh();
    GLuint PlaneMesh = primitives::plane_mesh();

    primitives.emplace(std::string("Cube"), CubeMesh);
    primitive_size.emplace(std::string("Cube"), primitives::cube_mesh_vertices);
    primitives.emplace(std::string("Plane"), PlaneMesh);
    primitive_size.emplace(std::string("Plane"), primitives::plane_mesh_vertices);
    primitives.emplace(std::string("Sphere"), 0);
    primitive_size.emplace(std::string("Sphere"), 0);
    primitives.emplace(std::string("Cylinder"), 0);
    primitive_size.emplace(std::string("Cylinder"), 0);
}

unsigned int PrimitiveManager::new_instance(
        std::string type, 
        std::string texture_name, 
        glm::vec3 position
        ) {
    instance_count ++;
    PrimitiveInstance instance(
        primitives[type],
        textures.get_texture(texture_name),
        position,
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1),
        program,
        type
    ); 

    instances.push_back(instance);
    
    return instances.size() - 1;
}

unsigned int PrimitiveManager::new_instance(
        std::string type, 
        glm::vec3 position, 
        glm::vec3 color,
        bool use_shading = true
    ) {
    instance_count ++;
    PrimitiveInstance instance(
        primitives[type],
        color,
        position,
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1),
        program,
        type,
        use_shading
    ); 

    instances.push_back(instance);
    
    return instances.size() - 1;
}

PrimitiveInstance &PrimitiveManager::get_instance(unsigned int instance_id) {
    return instances[instance_id];
}

void PrimitiveManager::update_instance_position(unsigned int instance_id, glm::vec3 position) {
    instances[instance_id].set_position(position);
}

void PrimitiveManager::update_instance_rotation(unsigned int instance_id, glm::vec3 rotation) {
    instances[instance_id].set_rotation(rotation);
}

void PrimitiveManager::update_instance_scale(unsigned int instance_id, glm::vec3 scale) {
    instances[instance_id].set_scale(scale);
}

glm::vec3 PrimitiveManager::get_instance_position(unsigned int instance_id) {
    return instances[instance_id].get_position();
}

glm::vec3 PrimitiveManager::get_instance_rotation(unsigned int instance_id) {
    return instances[instance_id].get_rotation();
}

glm::vec3 PrimitiveManager::get_instance_scale(unsigned int instance_id) {
    return instances[instance_id].get_scale();
}

void PrimitiveManager::draw() {
    glm::mat4 model_view_projection = state.generate_model_view();
    for (PrimitiveInstance prim: instances) {
        glm::mat4 mvp = prim.get_mvp_matrix(model_view_projection);
        glm::mat3 normal_mat = prim.get_normal_matrix();
        state.renderer.active().set_uniform("MVP", mvp);
        state.renderer.active().set_uniform("NormalMat", normal_mat);
        state.renderer.active().set_uniform("Model", prim.get_model_matrix());
        state.renderer.active().set_uniform("texture_diffuse1", 0);
        state.renderer.active().set_uniform("use_uniform_color", !prim.get_texture_status());
        state.renderer.active().set_uniform("use_shadows", prim.get_shading_status());

        prim.draw(primitive_size[prim.get_type()]);
    }
    state.renderer.active().set_uniform("use_uniform_color", false);
}