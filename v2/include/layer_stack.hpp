#ifndef _LAYER_STACK_HPP
#define _LAYER_STACK_HPP
#include <vector>
#include "layer.hpp"

namespace engine {

class LayerStack {
public:
    LayerStack();
    ~LayerStack();

    void push_layer(Layer* layer);
    void push_overlay(Layer* overlay);
    void pop_layer(Layer* layer);
    void pop_overlay(Layer* overlay);

    std::vector<Layer*>::iterator begin() { return layers.begin(); }
    std::vector<Layer*>::iterator end() { return layers.end(); }

private:
    std::vector<Layer*> layers;
    unsigned int layer_insert_index = 0;
};

}  //namespace

#endif
