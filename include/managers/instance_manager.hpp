#include <memory>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/renderer.hpp"

#include "state_manager.hpp"
#include "texture_manager.hpp"

#ifndef PRIMITIVE_H
#define PRIMITIVE_H
#define NULL_TEXTURE 0

class Instance{
public:
    Instance(
        vec3 uniform_color,
        glm::vec3 pos,
        glm::vec3 rot,
        glm::vec3 scale,
        std::string name,
        bool use_shading
    );

    void set_position(glm::vec3 new_position);
    void set_rotation(glm::vec3 new_rotation);
    void set_scale(glm::vec3 new_scale);

    glm::vec3 get_position();
    glm::vec3 get_rotation();
    glm::vec3 get_scale();

    glm::vec3 get_uniform_color();

    virtual void draw(State &state) {};

    bool get_texture_status();
    bool get_shading_status();

    std::string get_type();

    glm::mat4 get_model_matrix();
    glm::mat3 get_normal_matrix();
    glm::mat4 get_mvp_matrix(glm::mat4 view_proj_matrix);

protected:
    std::string name;
    glm::vec3 uniform_color;
    bool use_shading;
    bool use_texture;

    glm::vec3 position = glm::vec3(1.0f);
    glm::vec3 rotation = glm::vec3(1.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    bool invalid_model_matrix = true;
    glm::mat4 model = glm::mat4(1.0f);
};

class PrimitiveInstance : public Instance {
public:
    PrimitiveInstance(
        GLuint mesh_id,
        GLuint mesh_array_size,
        GLuint tex_id,
        glm::vec3 pos,
        glm::vec3 rot,
        glm::vec3 scale,
        GLuint program,
        std::string name
    );

    PrimitiveInstance(
        GLuint mesh_id,
        GLuint mesh_array_size,
        vec3 uniform_color,
        glm::vec3 pos,
        glm::vec3 rot,
        glm::vec3 scale,
        GLuint program,
        std::string name,
        bool use_shading
    );

    virtual void draw(State &state) override;
    GLuint get_texture_id();

private:
    GLuint mesh_array_size;
    GLuint mesh_id;
    GLuint tex_id;
    GLuint program;
};

class MeshInstance : public Instance {
public:
    MeshInstance(
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale,
        std::string filename,
        std::string path,
        std::string name,
        bool use_shading = true
    );

    virtual void draw(State &state) override;

private:
    Model model;
};


class InstanceManager {
public:
    InstanceManager(State &state, TextureManager &texture_manager);

    unsigned int new_primitive_instance(std::string type, std::string texture_name, glm::vec3 position);
    unsigned int new_primitive_instance(std::string type, glm::vec3 position, glm::vec3 color, bool use_shading);
    unsigned int new_mesh_instance(std::string filename, std::string path, glm::vec3 pos);
    Instance &get_instance(unsigned int instance_id);
    
    void draw();

    void update_instance_position(unsigned int instance_id, glm::vec3 position);
    void update_instance_rotation(unsigned int instance_id, glm::vec3 rotation);
    void update_instance_scale(unsigned int instance_id, glm::vec3 scale);

    glm::vec3 get_instance_scale(unsigned int instance_id);
    glm::vec3 get_instance_rotation(unsigned int instance_id);
    glm::vec3 get_instance_position(unsigned int instance_id);
private:
    State &state;
    TextureManager &textures;
    GLuint program;
    int instance_count = 0;
    std::unordered_map<std::string, int> primitives;
    std::unordered_map<std::string, GLuint> primitive_size;
    std::vector<std::unique_ptr<Instance>> instances;
};

#endif
