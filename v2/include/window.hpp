#ifndef _ENGINE_HPP
#define _ENGINE_HPP
#include "engine.hpp"


namespace engine{

// Generic desktop window
class Window {
public:
    Window() {};
    virtual ~Window() {};

    virtual void on_update() = 0;

    virtual unsigned int get_width() = 0;
    virtual unsigned int get_height() = 0;

    static Window* create();

};

}
#endif
