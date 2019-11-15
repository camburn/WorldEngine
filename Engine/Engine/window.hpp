#ifndef _ENGINE_HPP
#define _ENGINE_HPP
#include <string>


namespace engine{

// Generic desktop window
class Window {
public:
    Window() {};
    Window(int width, int height) {};
    virtual ~Window() {};

    virtual void on_update() = 0;

    virtual unsigned int get_width() = 0;
    virtual unsigned int get_height() = 0;

    virtual void* get_native_window() const = 0;

    static Window* create(int width, int height);
};

}
#endif
