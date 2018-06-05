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

class PrimitiveInstance {
public:
    PrimitiveInstance(
        GLuint mesh_id,
        GLuint tex_id,
        glm::vec3 pos,
        glm::vec3 rot,
        glm::vec3 scale,
        GLuint program,
        std::string name
    );

    PrimitiveInstance::PrimitiveInstance(
        GLuint mesh_id,
        vec3 uniform_color,
        glm::vec3 pos,
        glm::vec3 rot,
        glm::vec3 scale,
        GLuint program,
        std::string name,
        bool use_shading
    );

    void set_position(glm::vec3 new_position);
    void set_rotation(glm::vec3 new_rotation);
    void set_scale(glm::vec3 new_scale);

    glm::vec3 get_position();
    glm::vec3 get_rotation();
    glm::vec3 get_scale();

    void draw(GLuint array_size);

    GLuint get_texture_id();
    bool get_texture_status();
    bool get_shading_status();

    std::string get_type();

    glm::mat4 get_model_matrix();
    glm::mat3 get_normal_matrix();
    glm::mat4 get_mvp_matrix(glm::mat4 view_proj_matrix);

private:
    GLuint mesh_id;
    GLuint tex_id;
    GLuint program;
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


class PrimitiveManager {
public:
    PrimitiveManager(State &state, TextureManager &texture_manager);

    unsigned int new_instance(std::string type, std::string texture_name, glm::vec3 position);
    unsigned int new_instance(std::string type, glm::vec3 position, glm::vec3 color, bool use_shading);
    PrimitiveInstance &get_instance(unsigned int instance_id);
    
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
    std::vector<PrimitiveInstance> instances;
};

#endif