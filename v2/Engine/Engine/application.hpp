#ifndef _APPLICATION_HPP
#define _APPLICATION_HPP
#include <memory>
#include <string>

#include "Engine/event/event.hpp"
#include "Engine/event/bus.hpp"
#include "Engine/window.hpp"
#include "Engine/layer_stack.hpp"
#include "Engine/interface/interface.hpp"

namespace engine{

// Base engine application
class Application {
public:
    // Should have 
    // - Window
    // - Event manager
    // - Renderer
    //
    Application();
    ~Application();

    void push_layer(Layer* layer);
    void push_overlay(Layer* layer);

    void run();
    void on_event(std::shared_ptr<Event> event);

    inline Window &get_window() { return *window; }
    inline static Application &get() { return *instance; }

private:
    std::unique_ptr<Window> window;

    LayerStack layer_stack;
    InterfaceLayer *interface_layer;

    bool running = true;
    const std::string channel = "application";

    static Application* instance;

};

} // namespace
#endif