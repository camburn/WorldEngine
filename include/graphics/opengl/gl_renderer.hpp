/* OpenGL specific Rendering
 */
#include <string>
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tools/file_operations.hpp>

namespace opengl {

void init();

void create_window();

GLFWwindow* get_window();

void enable_debug();
void set_base_state();

GLuint LoadShader(const char* file_path, GLuint ShaderID);
GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path);
GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path);

}