#include "engine.hpp"
#include "event/event.hpp"
#include "event/bus.hpp"
#include "platform/linux_window.hpp"

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

    void run();
    void on_event(std::shared_ptr<Event> event);


    inline Window &get_window() { return *window; }

private:
    std::unique_ptr<Window> window;

    bool running = true;
    const std::string channel = "application";
};




} // namespace
