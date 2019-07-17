#include "application.hpp"

namespace engine {

Application::Application() {
    bus::subscribe(channel, APPLICATION_EVENT);

    window = std::unique_ptr<Window>(Window::create());
}

Application::~Application() {}

void Application::run() {
    while (running) {
        auto event = bus::get(channel);
        on_event(event);
        bus::publish(std::make_unique<ApplicationEvent>(QUIT));
    }
};

void Application::on_event(std::shared_ptr<Event> event) {
    if (event == nullptr) return;

    if (event->get_type() == APPLICATION_EVENT) {
        auto app_event = bus::get_event<ApplicationEvent>(event);
        if (app_event->data == QUIT) {
            running = false;
        }
    }
}

} //namespace
