/*
Managers are responsible for initialising all foundation data for their type.

They then make available this data via names.
*/
#include "managers/instance_manager.hpp"


Instance::Instance(
        vec3 uniform_color = glm::vec3(1,1,1),
        glm::vec3 pos = glm::vec3(0,0,0),
        glm::vec3 rot = glm::vec3(0,0,0),
        glm::vec3 scale = glm::vec3(1,1,1),
        std::string name = std::string("Default"),
        bool use_shading = true 
    ): 
        uniform_color(uniform_color),
        position(pos),
        rotation(rot),
        scale(scale),
        name(name),
        use_shading(use_shading)
     {
}

void Instance::set_position(glm::vec3 new_position) {
    invalid_model_matrix = true;
    position = new_position;
}

void Instance::set_rotation(glm::vec3 new_rotation) {
    invalid_model_matrix = true;
    rotation = new_rotation;
}

void Instance::set_scale(glm::vec3 new_scale) {
    invalid_model_matrix = true;
    scale = new_scale;
}

glm::vec3 Instance::get_position() {
    return position;
}

glm::vec3 Instance::get_rotation() {
    return rotation;
}

glm::vec3 Instance::get_scale() {
    return scale;
}

bool Instance::get_texture_status() {
    return use_texture;
}

bool Instance::get_shading_status() {
    return use_shading;
}

glm::vec3 Instance::get_uniform_color() {
    return uniform_color;
}

std::string Instance::get_type() {
    return name;
}

glm::mat4 Instance::get_model_matrix() {
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

glm::mat3 Instance::get_normal_matrix() {
    return (glm::mat3)glm::transpose(glm::inverse(
        Instance::get_model_matrix()
    ));
}

glm::mat4 Instance::get_mvp_matrix(glm::mat4 view_proj_matrix) {
    return view_proj_matrix * Instance::get_model_matrix();
}


PrimitiveInstance::PrimitiveInstance(
    GLuint mesh_id,
    GLuint mesh_array_size,
    GLuint tex_id,
    glm::vec3 pos,
    glm::vec3 rot,
    glm::vec3 scale,
    GLuint program,
    std::string name
): 
    Instance(glm::vec3(0.25, 0.25, 0.25), pos, rot, scale, name, use_shading),
    mesh_id(mesh_id),
    mesh_array_size(mesh_array_size),
    tex_id(tex_id) {
        use_texture = true;
}

PrimitiveInstance::PrimitiveInstance(
    GLuint mesh_id,
    GLuint mesh_array_size,
    vec3 uniform_color,
    glm::vec3 pos,
    glm::vec3 rot,
    glm::vec3 scale,
    GLuint program,
    std::string name,
    bool use_shading = true
): 
    Instance(uniform_color, pos, rot, scale, name, use_shading),
    mesh_id(mesh_id),
    mesh_array_size(mesh_array_size),
    program(program) {
        use_texture = false;
}

void PrimitiveInstance::draw(State &state) {
    if (use_texture) {
        glBindTexture(GL_TEXTURE_2D, get_texture_id());
    }
    glBindVertexArray(mesh_id);
    glDrawArrays(GL_TRIANGLES, 0, mesh_array_size);
    glBindVertexArray(0);
}

GLuint PrimitiveInstance::get_texture_id() {
    return tex_id;
};


MeshInstance::MeshInstance(
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale,
        std::string filename,
        std::string path,
        std::string name,
        bool use_shading
    ): 
        Instance(glm::vec3(0.25, 0.25, 0.25), position, rotation, scale, name, use_shading),
        model(path, filename) {
            use_shading = true;
            use_texture = true;
}

void MeshInstance::draw(State &state) {
    model.Draw(state.renderer.active().get_shader_id());
}



InstanceManager::InstanceManager(State &state, TextureManager &texture_manager) 
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

unsigned int InstanceManager::new_primitive_instance(
        std::string type, 
        std::string texture_name, 
        glm::vec3 position
        ) {
    instance_count ++;
    instances.emplace_back(new PrimitiveInstance(
        primitives[type],
        primitive_size[type],
        textures.get_texture(texture_name),
        position,
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1),
        program,
        type
    )); 
    
    return instances.size() - 1;
}

unsigned int InstanceManager::new_primitive_instance(
        std::string type, 
        glm::vec3 position, 
        glm::vec3 color,
        bool use_shading = true
    ) {
    instance_count ++;
    instances.emplace_back(new PrimitiveInstance(
        primitives[type],
        primitive_size[type],
        color,
        position,
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1),
        program,
        type,
        use_shading
    )); 
    
    return instances.size() - 1;
}

unsigned int InstanceManager::new_mesh_instance(
        std::string filename, 
        std::string path, 
        glm::vec3 pos
    ) {
    instance_count ++;

    instances.emplace_back(new MeshInstance(
        pos,
        glm::vec3(0, 0, 0),
        glm::vec3(1, 1, 1),
        filename,
        path,
        filename
    )); 
    
    return instances.size() - 1;
}

Instance &InstanceManager::get_instance(unsigned int instance_id) {
    return *instances[instance_id];
}

void InstanceManager::update_instance_position(unsigned int instance_id, glm::vec3 position) {
    instances[instance_id]->set_position(position);
}

void InstanceManager::update_instance_rotation(unsigned int instance_id, glm::vec3 rotation) {
    instances[instance_id]->set_rotation(rotation);
}

void InstanceManager::update_instance_scale(unsigned int instance_id, glm::vec3 scale) {
    instances[instance_id]->set_scale(scale);
}

glm::vec3 InstanceManager::get_instance_position(unsigned int instance_id) {
    return instances[instance_id]->get_position();
}

glm::vec3 InstanceManager::get_instance_rotation(unsigned int instance_id) {
    return instances[instance_id]->get_rotation();
}

glm::vec3 InstanceManager::get_instance_scale(unsigned int instance_id) {
    return instances[instance_id]->get_scale();
}

void InstanceManager::draw() {
    glm::mat4 model_view_projection = state.generate_model_view();
    for (auto &prim: instances) {
        glm::mat4 mvp = prim->get_mvp_matrix(model_view_projection);
        glm::mat3 normal_mat = prim->get_normal_matrix();
        state.renderer.active().set_uniform("MVP", mvp);
        state.renderer.active().set_uniform("NormalMat", normal_mat);
        state.renderer.active().set_uniform("Model", prim->get_model_matrix());
        state.renderer.active().set_uniform("texture_diffuse1", 0);
        state.renderer.active().set_uniform("uniform_color", prim->get_uniform_color());
        state.renderer.active().set_uniform("use_uniform_color", !prim->get_texture_status());
        state.renderer.active().set_uniform("use_shadows", prim->get_shading_status());

        prim->draw(state);
    }
    state.renderer.active().set_uniform("use_uniform_color", false);
}