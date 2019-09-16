#ifndef _GLTF_LOADER_HPP
#define _GLTF_LOADER_HPP
#include <string>
#include <memory>
#include <map>
#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/texture.hpp"
#include "Engine/renderer/vertex_array.hpp"

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "glad/glad.h"

#include "tiny_gltf.h"

struct ModelObjects {
    std::vector<GLuint> texture_ids;
    std::vector<std::shared_ptr<engine::VertexArray>> vaos;
};

struct TextureObject {
    GLuint texture_id;
    GLint texture_unit;
    std::string u_name;
};

struct MaterialObject {
    bool texture_set = false;
    std::vector<TextureObject> textures;
    //GLuint texture_id;
    glm::vec4 color;
};

struct PrimitiveObject {
    MaterialObject material;
    std::shared_ptr<engine::VertexArray> vao; // Contains VBOs, IBO
    // Who stores the IBO offset?
};

struct MeshObject {
    std::vector<PrimitiveObject> primitives;
};

struct NodeObject {
    NodeObject(): 
        transform_matrix(glm::mat4(1.0f)),
        rotation(glm::quat(1.0f, 0.0f, 0.0f, 0.0f)),
        translation(glm::vec3(0.0f, 0.0f, 0.0f)),
        scale(glm::vec3(1.0f, 1.0f, 1.0f)) { }

    glm::mat4& get_matrix() {
        if (!changed) return transform_matrix;
        glm::mat4 rotation_matrix = glm::toMat4(rotation);  // Rotation
        glm::mat4 translate_matrix = glm::translate(glm::mat4(1.0f), translation);
        glm::mat4 scale_matrix = glm::scale(glm::mat4(1.0f), scale);
        transform_matrix = translate_matrix * rotation_matrix * scale_matrix;
        changed = false;
        return transform_matrix;
    }

    glm::quat get_rotation(){ return rotation; }
    glm::vec3 get_translation(){ return translation; }
    glm::vec3 get_scale(){ return scale; }

    void set_rotation(glm::quat r) {
        rotation = r;
        changed = true;
    }

    void set_translation(glm::vec3 t) {
        translation = t;
        changed = true;
    }

    void set_scale(glm::vec3 s) {
        scale = s;
        changed = true;
    }

    std::vector<NodeObject> children;
    MeshObject mesh;

private:
    glm::mat4 transform_matrix;
    glm::quat rotation;
    glm::vec3 translation;
    glm::vec3 scale;
    bool changed = false;
};

struct DrawObject {

};

std::shared_ptr<engine::VertexArray> mesh_loader(std::string filename, const std::shared_ptr<engine::Shader> &shader);
NodeObject gltf_to_opengl(ModelObjects& m_obj, std::shared_ptr<tinygltf::Model> &model, const std::shared_ptr<engine::Shader> &shader);
void draw_node_graph(NodeObject &root_node);

#endif