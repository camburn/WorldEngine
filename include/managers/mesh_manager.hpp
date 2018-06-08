#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "graphics/renderer.hpp"
#include "graphics/model.hpp"

#include "state_manager.hpp"
#include "texture_manager.hpp"

#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H
#define NULL_TEXTURE 0

class MeshInstance {
public:
    MeshInstance(
        glm::vec3 position,
        glm::vec3 rotation,
        glm::vec3 scale,
        std::string filename,
        std::string path 
    );


    void set_position(glm::vec3 new_position);
    void set_rotation(glm::vec3 new_rotation);
    void set_scale(glm::vec3 new_scale);

    glm::vec3 get_position();
    glm::vec3 get_rotation();
    glm::vec3 get_scale();

    glm::mat4 get_model_matrix();
    glm::mat3 get_normal_matrix();
    glm::mat4 get_mvp_matrix(glm::mat4 view_proj_matrix);

    Model model;
private:
    glm::mat4 model_matrix = glm::mat4(1.0f);
    bool invalid_model_matrix = true;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;

};


class MeshManager {
public:
    MeshManager(State &state, TextureManager &texture_manager);

    unsigned int new_instance(std::string filename, std::string path, glm::vec3 position);
    unsigned int new_instance(std::string type, glm::vec3 position, glm::vec3 color, bool use_shading);
    MeshInstance &get_instance(unsigned int instance_id);
    
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
    std::vector<MeshInstance> instances;
};

#endif
