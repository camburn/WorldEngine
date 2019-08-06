#include "engine.hpp"

#include "camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace engine {

OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
        : Camera(glm::ortho(left, right, bottom, top, -10.0f, 10.0f), glm::mat4(1.0f)) {
    view_projection_matrix = projection_matrix * view_matrix;
}

void OrthographicCamera::recalculate_view_matrix() {
    // Ortho camera only rotates on Z
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * 
        glm::rotate(glm::mat4(1.0f), glm::radians(rotation.z), glm::vec3(0, 0, 1));

    view_matrix = glm::inverse(transform);
    view_projection_matrix = projection_matrix * view_matrix;
}

} //Namespace