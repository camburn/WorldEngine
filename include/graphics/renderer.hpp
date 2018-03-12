/* Base Renderer Class
 *
 */
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "graphics/opengl/gl_renderer.hpp"

class Renderer {
public:
    Renderer();
    void LoadShaders();
    GLFWwindow* get_window();
private:
};
