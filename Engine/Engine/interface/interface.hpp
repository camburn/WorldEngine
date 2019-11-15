#ifndef _INTERFACE_HPP
#define _INTERFACE_HPP

#include "Engine/layer.hpp"

namespace engine {

class InterfaceLayer: public Layer {
public:
    InterfaceLayer();
    ~InterfaceLayer();

    virtual void on_attach() override;
    virtual void on_detach() override;

    virtual void on_ui_render() override;

    void begin();
    void end();

private:
    float time = 0.0f;
};

}  //namespace

#endif
