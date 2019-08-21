#ifndef _CAMERA_HPP
#define _CAMERA_HPP

#include <glm/glm.hpp>

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

};

class PerspectiveCamera : public Camera {
public:
    PerspectiveCamera(float fov, float right, float bottom, float top);

    virtual void recalculate_view_matrix() override;
    virtual void on_ui_render(bool display) override;

private:
    glm::vec3 look_at  {0.0f, 1.0f, 0.0f};
    glm::vec3 up {0.0f, 1.0f, 0.0f};
};

}
#endif