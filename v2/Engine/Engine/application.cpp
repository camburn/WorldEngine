#include "engine.hpp"
#include "application.hpp"

#include "GL/gl.h"

namespace engine {

Application::Application() {
    Log::Init();
    ENGINE_INFO("Application Started");
    bus::subscribe(channel, ENGINE_APPLICATION_EVENT);

    window = std::unique_ptr<Window>(Window::create());
}

Application::~Application() {}

void Application::run() {
    while (running) {
        auto event = bus::get(channel);
        on_event(event);

        glClearColor(0.5, 0.5, 0.5, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        for (Layer* layer: layer_stack) {
            layer->on_update();
        }

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
}

} //namespace
