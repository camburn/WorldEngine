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

PerspectiveCamera::PerspectiveCamera(float fov, float aspect, float near_plane, float far_plane)
        : Camera(glm::perspective(fov, aspect, near_plane, far_plane), glm::lookAt(position, glm::vec3(0, 1, 0), glm::vec3(0, 1, 0))) {
    view_projection_matrix = projection_matrix * view_matrix;
    
}

void PerspectiveCamera::recalculate_view_matrix() {
    view_matrix = glm::lookAt(position, glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    glm::mat4 transform = 
        glm::rotate(view_matrix, glm::radians(rotation.x), glm::vec3(1, 0, 0)) *
        glm::rotate(view_matrix, glm::radians(rotation.y), glm::vec3(0, 1, 0)) *
        glm::rotate(view_matrix, glm::radians(rotation.z), glm::vec3(0, 0, 1));


    //view_matrix = glm::inverse(transform);

    view_matrix = glm::lookAt(glm::vec3(2,2,2), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));
    view_projection_matrix = projection_matrix * view_matrix;
}

} //Namespace