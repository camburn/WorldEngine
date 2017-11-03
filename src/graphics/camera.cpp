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
glm::vec3 ortho_pos = glm::vec3(0, 1, 0);
glm::vec3 pers_pos = glm::vec3(7, 3, 6);

Arcball arcball(800.0f, 600.0f, 0.1f, true, true);

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    arcball.mouseButtonCallback(window, button, action, mods);
}

void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos) {
    arcball.cursorCallback(window, xpos, ypos);
}

void toggleCamera() {
    if (UsePerspective) {
        UsePerspective = false;
    } else {
        UsePerspective = true;
    }
}

glm::mat4& cameraUpdate(int width, int height) {
    // Update camera with new width and height data
    pers_proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
    arcball = Arcball(width, height, 0.1f, true, true);

    ortho_proj = glm::ortho(0.0f, (float)width/100, 0.0f, (float)height/100, -1.0f, 10.0f);

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

glm::mat4& getView(){
    if (UsePerspective) {
        pers_view = glm::lookAt(pers_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        pers_view = pers_view * arcball.createViewRotationMatrix();
        return pers_view;
    } else {
        ortho_view = glm::lookAt(ortho_pos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        ortho_view = glm::mat4(1.0f);
        return ortho_view;
    }
}