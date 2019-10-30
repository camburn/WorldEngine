#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#ifndef M_PI
#define M_PI 3.14159f
#endif

#include "Engine/application.hpp"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace engine {

class Camera {
public:
    Camera(glm::mat4 proj, glm::mat4 view): projection_matrix(proj), view_matrix(view) {}
    const glm::vec3& get_position() const { return position; }
    void set_position(const glm::vec3& pos) { 
        position = pos;
        recalculate_view_matrix();
    }

    glm::vec3 get_rotation() const { return rotation; }
    void set_rotation(const glm::vec3& rot) {
        rotation = rot;
        recalculate_view_matrix();
    }

    virtual void recalculate_view_matrix() = 0;

    virtual void on_ui_render(bool display) = 0;

    const glm::mat4& get_projection_matrix() const { return projection_matrix; }
    const glm::mat4& get_view_matrix() const { return view_matrix; }
    const glm::mat4& get_view_projection_matrix() const { return view_projection_matrix; }

protected:
    glm::vec3 position {0.0f};
    glm::vec3 rotation {0.0f};
    glm::mat4 projection_matrix;
    glm::mat4 view_matrix;
    glm::mat4 view_projection_matrix;
};

class OrthographicCamera: public Camera {
public:
    OrthographicCamera(float left, float right, float bottom, float top);

    virtual void recalculate_view_matrix() override;
    virtual void on_ui_render(bool display) override;
    void set_view(glm::vec3 new_position, glm::vec3 new_look_at, glm::vec3 new_up);

private:
    glm::vec3 look_at {0.0f, 0.0f, 0.0f};
    glm::vec3 up {0.0f, 1.0f, 0.0f};
};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float fov, float right, float bottom, float top);
    void set_proj_matrix(float fov, float aspect, float near_plane, float far_plane);
    void set_view(glm::vec3 new_position, glm::vec3 new_look_at, glm::vec3 new_up);

    virtual void recalculate_view_matrix() override;
    virtual void on_ui_render(bool display) override;

private:
    glm::vec3 look_at {0.0f, 0.0f, 0.0f};
    glm::vec3 up {0.0f, 1.0f, 0.0f};
};

class NewPerspectiveCamera: public Camera {
public:
    NewPerspectiveCamera(float fov, float aspect, float near_plane, float far_plane)
        : Camera(glm::perspective(glm::radians(fov), aspect, near_plane, far_plane), glm::mat4{}) {
        position = {-5, 5, 5};
        rotation = {90.0f, 0.0f, 0.0f};
        focal_point = {0.0f, 0.0f, 0.0f};
        distance = glm::distance(position, focal_point);
        yaw = 3.0f * (float)M_PI / 4.0f;
        pitch = (float)M_PI / 4.0f;

        view_projection_matrix = projection_matrix * view_matrix;
    }

    std::pair<float, float> pan_speed() const {
        float x = std::min(viewport_width / 1000.0f, 2.4f);
        float x_factor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(viewport_height / 1000.0f, 2.4f);
        float y_factor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return {x_factor, y_factor};
    }

    float rotation_speed() const {
        return 0.8f;
    }

    float zoom_speed() const {
        float dis = distance * 0.2;
        dis = std::max(dis, 0.0f);
        float speed = dis * dis;
        speed = std::min(speed, 100.0f);
        return speed;
    }

    void mouse_pan(const glm::vec2& delta) {
        auto [x_speed, y_speed] = pan_speed();
        focal_point += -get_right_direction() * delta.x * x_speed * distance;
        focal_point += get_up_direction() * delta.y * y_speed * distance;
    }

    void mouse_rotate(const glm::vec2& delta) {
        float yaw_sign = get_up_direction().y < 0 ? -1.0f: 1.0f;
        yaw += yaw_sign * delta.x * rotation_speed();
        pitch += delta.y * rotation_speed();
    }

    void mouse_zoom(float delta) {
        distance -= delta * zoom_speed();
        if (distance < 1.0f) {
            focal_point += get_forward_direction();
            distance = 1.0f;
        }
    }

    glm::quat get_orientation() {
        return glm::quat(glm::vec3(-pitch, -yaw, 0.0f));
    }

    glm::vec3 get_up_direction() {
        return glm::rotate(get_orientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 get_right_direction() {
        return glm::rotate(get_orientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 get_forward_direction() {
        return glm::rotate(get_orientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::vec3 calculate_position() {
        return focal_point - get_forward_direction() * distance;
    }

    void update(float delta) {
        auto window = static_cast<GLFWwindow*>(Application::get().get_window().get_native_window());
        int state = glfwGetKey(window, GLFW_KEY_LEFT_ALT);
        if (state == GLFW_PRESS || state == GLFW_REPEAT) {}
        {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            const glm::vec2& mouse{ xpos, ypos };
            glm::vec2 mouse_delta = mouse - initial_mouse_position;
            initial_mouse_position = mouse;
            mouse_delta *= delta;

            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
                mouse_pan(mouse_delta);
            }
            else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
                mouse_rotate(mouse_delta);
            }
            else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE)) {
                mouse_zoom(mouse_delta.y);
            }
        }
        position = calculate_position();
        glm::quat orientation = get_orientation();

        rotation = glm::eulerAngles(orientation * (180.0f / (float)M_PI));
        view_matrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 1)) *
            glm::toMat4(glm::conjugate(orientation)) *
            glm::translate(glm::mat4(1.0f), - position);
        view_matrix = glm::translate(glm::mat4(1.0f), position) * glm::toMat4(orientation);
        view_matrix = glm::inverse(view_matrix);
        view_projection_matrix = projection_matrix * view_matrix;
    }

    void recalculate_view_matrix() override {}

    void on_ui_render(bool display) override {};

private:
    glm::vec3 focal_point {0.0f};
    float distance, pitch, yaw;
    bool rotating, panning;

    glm::vec2 initial_mouse_position;
    glm::vec3 initial_focal_point, initial_rotation;

    uint32_t viewport_width = 1280;
    uint32_t viewport_height = 720;
};

}
#endif
