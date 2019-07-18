#include "engine.hpp"
#include "layer_stack.hpp"

namespace engine {

LayerStack::LayerStack(){

}

LayerStack::~LayerStack(){
    for (Layer* layer: layers) {
        delete layer;
    }
}

void LayerStack::push_layer(Layer* layer) {
    layers.emplace(layers.begin() + layer_insert_index, layer);
    layer_insert_index++;
}

void LayerStack::push_overlay(Layer* overlay) {
    layers.emplace_back(overlay);
}

void LayerStack::pop_layer(Layer* layer) {
    auto it = std::find(layers.begin(), layers.end(), layer);
    if (it != layers.end()) {
        layers.erase(it);
        layer_insert_index--;
    }
}

void LayerStack::pop_overlay(Layer* overlay) {
    auto it = std::find(layers.begin(), layers.end(), overlay);
    if (it != layers.end()) {
        layers.erase(it);
    }
}

}
