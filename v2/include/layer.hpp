#ifndef _LAYER_HPP
#define _LAYER_HPP
#include <string>
#include "event/event.hpp"

namespace engine {

class Layer{
public:
    Layer(const std::string &name = "Layer");
    virtual ~Layer();

    virtual void on_attach() {}
    virtual void on_detach() {}
    virtual void on_update() {}
    virtual void on_ui_render() {}
    virtual void on_event(Event &event) {}
protected:
    std::string name;
};

}
#endif
