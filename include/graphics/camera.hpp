#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/arcball.hpp"

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void mouseCursorCallback(GLFWwindow* window, double xpos, double ypos);

glm::mat4& cameraUpdate(int width, int height);
void toggleCamera();
glm::mat4& getProj();
glm::mat4& getView();
glm::mat4& getView(glm::vec3 custom_ortho_pos);

void translate_ortho(glm::vec3 translation);
void set_ortho_pos(glm::vec3 pos);
glm::vec3 get_ortho_pos();
void move_left();
void move_right();
void move_up();
void move_down();
void zoom_in();
void zoom_out();