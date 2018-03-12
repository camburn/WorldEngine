/* OpenGL specific Rendering
 */
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace opengl {

void init();

void create_window();

GLFWwindow* get_window();

void enable_debug();

}
