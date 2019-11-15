#include "engine.hpp"
#include "application.hpp"

namespace engine {

Application* Application::instance = nullptr;

Application::Application(int width, int height) {
    Log::Init();
    ENGINE_ASSERT(!instance, "Application already exists");
    instance = this;

    ENGINE_INFO("Application Started");
    bus::subscribe(channel, ENGINE_APPLICATION_EVENT | ENGINE_KEY_EVENT);
    
    window = std::unique_ptr<Window>(Window::create(width, height));

    interface_layer = new InterfaceLayer{};
    push_overlay(interface_layer);
}

Application::~Application() {

}

void Application::run() {

    while (running) {
        auto event = bus::get(channel);
        on_event(event);

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
        if ((app_event->key == 256) & (app_event->action == 1)) {
            bus::publish(std::make_unique<ApplicationEvent>(QUIT));
        }
    }
}

} //namespace
