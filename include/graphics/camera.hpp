#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/arcball.hpp"

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos);

glm::mat4& cameraUpdate(int width, int height);
void toggleCamera();
glm::mat4& getProj();
glm::mat4& getView();

void move_left();
void move_right();
void move_up();
void move_down();
void zoom_in();
void zoom_out();