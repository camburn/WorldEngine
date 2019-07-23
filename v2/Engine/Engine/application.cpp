#include "engine.hpp"
#include "application.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Platform/OpenGL/shader_loader.hpp"

namespace engine {

Application* Application::instance = nullptr;

Application::Application() {
    Log::Init();
    ENGINE_ASSERT(!instance, "Application already exists");
    instance = this;

    ENGINE_INFO("Application Started");
    bus::subscribe(channel, ENGINE_APPLICATION_EVENT | ENGINE_KEY_EVENT);

    window = std::unique_ptr<Window>(Window::create());

    interface_layer = new InterfaceLayer{};
    push_overlay(interface_layer);
}

Application::~Application() {
    glDeleteBuffers(1, &vertex_array);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &index_buffer);

}

void Application::run() {
    // SIMPLE OpenGL test Triangle
    // 1. Load shaders
    std::string vs_file = "./shaders/vertex.glsl";
    std::string fs_file = "./shaders/fragment.glsl";
    shader_id = enginegl::build_program(vs_file, fs_file);
    // 2. Generate Buffers

    #ifndef OPENGL_COMPATIBILITY
    glGenVertexArrays(1, &vertex_array);
    glBindVertexArray(vertex_array);
    #endif

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

    float vertices[3 * 3] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    glGenBuffers(1, &index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);

    unsigned int indices[3] = { 0, 1, 2 };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    // END OpenGL Test

    while (running) {
        auto event = bus::get(channel);
        on_event(event);

        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader_id);

        #ifdef OPENGL_COMPATIBILITY
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer);
        #else
        glBindVertexArray(vertex_array);
        #endif
        
        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        glUseProgram(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        for (Layer* layer: layer_stack) {
            layer->on_update();
        }

        interface_layer->begin();
        for (Layer* layer: layer_stack) {
            layer->on_ui_render();
        }
        interface_layer->end();

        window->on_update();
    }
};

void Application::push_layer(Layer* layer) {
    layer_stack.push_layer(layer); 
    layer->on_attach();
}

void Application::push_overlay(Layer* layer) {
    layer_stack.push_overlay(layer);
    layer->on_attach();
}

void Application::on_event(std::shared_ptr<Event> event) {
    if (event == nullptr) return;

    if (event->get_type() == ENGINE_APPLICATION_EVENT) {
        auto app_event = bus::get_event<ApplicationEvent>(event);
        if (app_event->data == QUIT) {
            running = false;
        }
    }
    if (event->get_type() == ENGINE_KEY_EVENT) {
        auto app_event = bus::get_event<KeyEvent>(event);
        if (app_event->key == 256 & app_event->action == 1) {
            bus::publish(std::make_unique<ApplicationEvent>(QUIT));
        }
    }
}

} //namespace
