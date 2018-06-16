/* 
The camera can operate in two modes;
 - Orthographic
 - Perspective 

The control scheme depends on the camera mode being used.
The perspective projection uses an arcball.
Orthographic will slide along its plane
At this point Orthographic will snap to the X,Z Plane (Y for Depth)

*/
#include "graphics/camera.hpp"

bool UsePerspective = true;

// Projections
glm::mat4 ortho_proj = glm::mat4(1.0f);
glm::mat4 pers_proj = glm::mat4(1.0f);
glm::mat4 ortho_view = glm::mat4(1.0f);
glm::mat4 pers_view = glm::mat4(1.0f);

// Control Schemes
//static Arcball arcball(800.0f, 600.0f, 0.1f, true, true);
glm::vec3 ortho_pos = glm::vec3(0, 0, 0);
glm::vec3 pers_pos = glm::vec3(5, 5, 5);
glm::vec3 look_pos = glm::vec3(0, 0, 0);
glm::vec3 centre_pos = glm::vec3(0, 0, 0);

float scroll_across = 0;
float scroll_up = 0;
float camera_zoom = 1.0f;
int mouseEvent = 0;
int window_width = 1920;
int window_height = 1080;

void camera_settings(bool *p_open) {
    const float   f32_zero = 0.0f, f32_one = 25.0f;

    if (!ImGui::Begin("Settings", p_open))
	{
		ImGui::End();
		return;
	}
    if (ImGui::TreeNode("Camera Settings")) {
        ImGui::SliderScalar("camera_pos X",   ImGuiDataType_Float,  &pers_pos.x, &f32_zero, &f32_one);
        ImGui::SliderScalar("camera_pos Y",   ImGuiDataType_Float,  &pers_pos.y, &f32_zero, &f32_one);
        ImGui::SliderScalar("camera_pos Z",   ImGuiDataType_Float,  &pers_pos.z, &f32_zero, &f32_one);
        ImGui::SliderScalar("camera_lookat X",   ImGuiDataType_Float,  &look_pos.x, &f32_zero, &f32_one);
        ImGui::SliderScalar("camera_lookat Y",   ImGuiDataType_Float,  &look_pos.y, &f32_zero, &f32_one);
        ImGui::SliderScalar("camera_lookat Z",   ImGuiDataType_Float,  &look_pos.z, &f32_zero, &f32_one);
        ImGui::TreePop();
    }
    ImGui::End();
}
/* Shoot a ray out from a mouse click location to the world */
glm::vec3 camera_ray(double mouse_x, double mouse_y) {
    // Convert mouse coords to a range of -1, 1
    float x = (float)mouse_x / ((float)window_width * 0.5f) -1.0f;
    float y = (float)mouse_y / ((float)window_height * 0.5f) -1.0f;

    pers_proj;
    pers_view;
    glm::mat4 inverse_pv = glm::inverse(pers_proj * pers_view);
    glm::vec4 screen_pos = glm::vec4(x, -y, 1.0f, 1.0f);
    glm::vec4 world_pos = inverse_pv * screen_pos;

    glm::vec3 dir = glm::normalize(glm::vec3(world_pos));
    return dir;
}

glm::vec3 ray_plane_intersection(
        glm::vec3 plane_normal, glm::vec3 plane_point, 
        glm::vec3 ray_start_point, glm::vec3 ray_direction) {
    float distance = glm::dot(plane_normal, (plane_point, - ray_start_point)) / glm::dot(plane_normal, ray_direction);
    glm::vec3 intersect_point = ray_start_point + (ray_direction * distance);
    return intersect_point;
}

Arcball arcball(800.0f, 600.0f, 0.1f, true, true);

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    arcball.mouseButtonCallback(window, button, action, mods);
    mouseEvent = ( action == GLFW_PRESS && button == GLFW_MOUSE_BUTTON_LEFT );
}

void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos) {

    arcball.cursorCallback(window, xpos, ypos);
    if (mouseEvent) {
        // To get movement get pos position to game plane then 
        glm::vec3 ray_dir = camera_ray(xpos, ypos);
        glm::vec3 ray_start_pos = pers_pos;
        // Lets get the ray intersection at ground plane
        glm::vec3 intersect_point = ray_plane_intersection(glm::vec3(0, 1, 0), glm::vec3(0, 0, 0), ray_start_pos, ray_dir);
        look_pos = intersect_point;
        
    }

}

void toggleCamera() {
    if (UsePerspective) {
        UsePerspective = false;
    } else {
        UsePerspective = true;
    }
}

void zoom_in() {
    pers_pos -= glm::vec3(0.1f, 0.1, 0.1f);
    camera_zoom -= 0.1f;
    if (camera_zoom <= 0.0f) {
        camera_zoom = 0.1f;
    }
}

void zoom_out() {
    pers_pos += glm::vec3(0.1f, 0.1, 0.1f);
    camera_zoom += 0.1f;
}

void move_left() {
    scroll_across -= 1.0f;
}

void move_right() {
    scroll_across += 1.0f;
}

void move_up() {
    scroll_up += 1.0f;
}

void move_down() {
    scroll_up -= 1.0f;
}

void translate_ortho(glm::vec3 translation) {
    ortho_pos = ortho_pos + translation;
}

void set_ortho_pos(glm::vec3 pos) {
    ortho_pos = pos;
}

glm::vec3 get_ortho_pos() {
    return ortho_pos;
}

void scale_ortho(glm::vec3 scale) {
    ortho_pos = ortho_pos + scale;
}

glm::mat4& cameraUpdate(int width, int height) {
    window_height = height;
    window_width = width;
    // Update camera with new width and height data
    pers_proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    arcball = Arcball(width, height, 0.1f, true, true);

    //ortho_proj = glm::ortho(0.0f, (float)width/100, 0.0f, (float)height/100, -1.0f, 10.0f);
    //ortho_proj = glm::ortho(-180.0f, 180.0f, -90.0f, 90.0f, -1.0f, 10.0f);
    ortho_proj = glm::ortho(-180.0f + scroll_across, 180.0f + scroll_across, 
                           -90.0f + scroll_up, 90.0f + scroll_up, -1.0f, 10.0f);

    if (UsePerspective) {
        return pers_proj;
    } else {
        return ortho_proj;
    }
}

glm::mat4& getProj(){
    if (UsePerspective){
        return pers_proj;
    } else {
        return ortho_proj;
    }
}

glm::mat4 getView(){
    if (UsePerspective) {
        pers_view = glm::lookAt(pers_pos + look_pos, centre_pos + look_pos, glm::vec3(0, 1, 0));
        pers_view = pers_view * arcball.createViewRotationMatrix();
        return pers_view;
    } else {
        glm::mat4 eye = glm::mat4(1.0f);
        glm::mat4 scale = glm::scale(eye, glm::vec3(camera_zoom, camera_zoom, 1.0f));
        glm::mat4 view = glm::translate(scale, ortho_pos);
        return view;
    }
}


glm::mat4 getView(glm::vec3 custom_ortho_pos) {
    if (UsePerspective) {
        pers_view = glm::lookAt(pers_pos + look_pos, centre_pos + look_pos, glm::vec3(0, 1, 0));
        pers_view = pers_view * arcball.createViewRotationMatrix();
        return pers_view;
    } else {
        glm::mat4 eye = glm::mat4(1.0f);
        float zoomer = 1.0f * (camera_zoom * camera_zoom * camera_zoom);
        glm::mat4 scale = glm::scale(eye, glm::vec3(camera_zoom, camera_zoom, 1.0f));
        glm::mat4 view = glm::translate(scale, custom_ortho_pos);
        return view;
    }
}