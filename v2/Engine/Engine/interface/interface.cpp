#include "engine.hpp"
#include "interface.hpp"

#include "imgui.h"
#include "examples/imgui_impl_glfw.h"

#define IMGUI_IMPL_OPENGL_LOADER_GLEW
#ifdef OPENGL_COMPATIBILITY
#include "examples/imgui_impl_opengl2.h"
#else
#include "examples/imgui_impl_opengl3.h"
#endif

#include "Engine/application.hpp"

// TODO: REMOVE THIS 
#include <GLFW/glfw3.h>


namespace engine {

InterfaceLayer::InterfaceLayer(): Layer("Interface Layer") {}
InterfaceLayer::~InterfaceLayer() {}

void InterfaceLayer::on_attach() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    Application& app = Application::get();
    GLFWwindow* window = static_cast<GLFWwindow*>(app.get_window().get_native_window());

    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);

    #ifdef OPENGL_COMPATIBILITY
    ImGui_ImplOpenGL2_Init();
    #else
    ImGui_ImplOpenGL3_Init("#version 410");
    #endif
}
void InterfaceLayer::on_detach() {
    #ifdef OPENGL_COMPATIBILITY
    ImGui_ImplOpenGL2_Shutdown();
    #else
    ImGui_ImplOpenGL3_Shutdown();
    #endif

    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void InterfaceLayer::on_ui_render() {
    static bool show = true;
    ImGui::ShowDemoWindow(&show);
}

void InterfaceLayer::begin() {
    #ifdef OPENGL_COMPATIBILITY
    ImGui_ImplOpenGL2_NewFrame();
    #else
    ImGui_ImplOpenGL3_NewFrame();
    #endif

    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
}

void InterfaceLayer::end() {
    ImGuiIO& io = ImGui::GetIO();
    Application& app = Application::get();
    io.DisplaySize = ImVec2(
        (float)app.get_window().get_width(),
        (float)app.get_window().get_height()
    );

    // Rendering
    ImGui::Render();

    #ifdef OPENGL_COMPATIBILITY
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    #else
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    #endif

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }
}

} // namespace
