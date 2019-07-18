/* OpenGL specific Rendering
 */
#include <string>
#include <iostream>
#include <unordered_map>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <tools/file_operations.hpp>
#include "imgui.h"
#include "graphics/buffers.hpp"
#include "managers/data_models.hpp"

namespace opengl {

void init();

void create_window();

GLFWwindow* get_window();

void enable_debug();
void set_base_state();
void create_common_buffers();
void activate_common_buffers();
void activate_buffer_cube_shadow_map();
void clear_buffers();
void bind_depth_map();
void finish_frame();
void update_uniforms(SharedState &state, LightState &light_state);

void draw_buffers(bool* p_open);
void draw_ssbo_state(bool* p_open);

GLuint LoadShader(const char* file_path, GLuint ShaderID);
GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path);
GLuint BuildGlProgram(const char* vertex_file_path, const char* fragment_file_path, const char* geometry_file_path);
void bind_jitter_map(GLuint map_id);
GLuint create_jitter_lookup(int size, int samples_u, int samples_v);

}
