#include "engine.hpp"
#include <iostream>

#include "Engine/application.hpp"
#include "Engine/layer.hpp"

class MyLayer: public engine::Layer {

    void on_attach() override {
        GAME_INFO("My layer attached");
    }
};

int main() {
    engine::Application application;
    application.push_layer(new MyLayer());
    application.run();
    return 0;
}
