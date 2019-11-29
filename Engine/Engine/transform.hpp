#ifndef _TRANSFORM_HPP
#define _TRANSFORM_HPP

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/common.hpp>

namespace engine {

class Transform {
public:
    Transform() {}
    Transform(glm::vec3 translation, glm::vec3 scale, glm::quat rotation)
            : translation(translation), scale(scale), rotation(rotation) {
        state_changed = true;
    }
    Transform(glm::vec3 translation, glm::vec3 scale)
            : translation(translation), scale(scale), rotation(glm::quat{1.0f, 0.0f, 0.0f, 0.0f}) {
        state_changed = true;
    }
    Transform(glm::mat4 model) {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(model, scale, rotation, translation, skew, perspective);
        state_changed = true;
    }

    void set_model_matrix(glm::mat4 model) {
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(model, scale, rotation, translation, skew, perspective);
        state_changed = true;
    }
    
    void disable_rotation(){ _disable_rotation = true; }
    void disable_scale(){ _disable_scale = true; }

    void set_translation(glm::vec3 value) { state_changed = true; translation = value; }
    void set_scale(glm::vec3 value) {
        if (_disable_scale) return;
        state_changed = true;
        scale = value;
    }
    void set_rotation(glm::quat value) {
        if (_disable_rotation) return;
        state_changed = true;
        rotation = value;
    }

    glm::vec3 get_translation(bool local = false) {
        if (parent_set && !local) {
            return translation + parent->get_translation();
        }
        return translation;
    }
    glm::vec3 get_scale(bool local = false) {
        if (parent_set && !local && !_disable_scale) {
            return scale + parent->get_scale();
        }
        return scale;
    }
    glm::quat get_rotation(bool local = false) {
        if (parent_set && !local && !_disable_rotation) {
            return rotation * parent->get_rotation();
        }
        return rotation;
    }

    void set_parent(std::shared_ptr<Transform> transform) {
        parent = transform;
        parent_set = true;
    }

    glm::mat4 get_model_matrix() {
        if (state_changed) {
            model_matrix = glm::translate(glm::mat4(1.0f), translation) *
                glm::toMat4(rotation) *
                glm::scale(glm::mat4(1.0f), glm::vec3(scale));
            state_changed = false;
        }
        if (parent_set) {
            if (_disable_scale) {
                // FIXME: Rotation being combined incorrectly
                glm::vec3 parent_translate = parent->get_translation();
                glm::quat parent_rotation = parent->get_rotation();
                glm::mat4 m = glm::toMat4(parent_rotation) * model_matrix;
                m = glm::translate(m, parent_translate);
                return m;
            }
            return model_matrix * parent->get_model_matrix();
        }
        return model_matrix;
    }

    Transform operator+(Transform &transform) {
        glm::mat4 output = get_model_matrix() * transform.get_model_matrix();
        return Transform(output);
    }

private:
    glm::vec3 translation {0.0f, 0.0f, 0.0f};
    glm::vec3 scale {1.0f, 1.0f, 1.0f};
    glm::quat rotation {1.0f, 0.0f, 0.0f, 0.0f};
    glm::mat4 model_matrix {1.0f};
    bool state_changed = false;
    bool parent_set = false;
    bool _disable_scale = false;
    bool _disable_rotation = false;
    std::shared_ptr<Transform> parent {nullptr};
};

}

#endif