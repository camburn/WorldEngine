#include "engine.hpp"
#include "application.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Engine/renderer/shader.hpp"
#include "Engine/renderer/buffer.hpp"
#include "Engine/renderer/vertex_array.hpp"


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

}

void Application::run() {
    // SIMPLE OpenGL test Triangle
    // 1. Load shaders
    std::string vs_file = "./shaders/vertex.glsl";
    std::string fs_file = "./shaders/fragment.glsl";
    Shader shader{vs_file, fs_file};
    // 2. Generate Buffers

    std::shared_ptr<VertexArray> vao;
    vao.reset(VertexArray::create());

    float vertices[3 * 3] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    BufferLayout layout {
        {ShaderDataType::Float3, "position"}
    };
    std::shared_ptr<VertexBuffer> buffer;
    buffer.reset(VertexBuffer::create(vertices, sizeof(vertices)));
    buffer->set_layout(layout);

    uint32_t indices[3] = { 0, 1, 2 };
    std::shared_ptr<IndexBuffer> index_buffer;
    index_buffer.reset(IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t)));

    vao->add_vertex_buffer(buffer);
    vao->set_index_buffer(index_buffer);
    // END OpenGL Test

    while (running) {
        auto event = bus::get(channel);
        on_event(event);

        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.bind();

        vao->bind();

        glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, nullptr);

        

        for (Layer* layer: layer_stack) {
            layer->on_update();
        }

        shader.unbind();

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
        if ((app_event->key == 256) & (app_event->action == 1)) {
            bus::publish(std::make_unique<ApplicationEvent>(QUIT));
        }
    }
}

} //namespace
