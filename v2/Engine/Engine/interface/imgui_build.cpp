#include "engine.hpp"

#ifdef OPENGL_COMPATIBILITY
#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#include "examples/imgui_impl_opengl2.cpp"
#else
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include "examples/imgui_impl_opengl3.cpp"
#endif
#include "examples/imgui_impl_glfw.cpp"
