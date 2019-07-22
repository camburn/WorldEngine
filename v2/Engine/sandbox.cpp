#include "engine.hpp"
#include <iostream>

#include "imgui.h"

#include "Engine/application.hpp"
#include "Engine/layer.hpp"

class MyLayer: public engine::Layer {

    void on_attach() override {
        GAME_INFO("My layer attached");
    }

    void on_ui_render() override {
        static int counter = 0;
        ImGui::Begin("Test");
        ImGui::Text("Sandbox UI");
        if (ImGui::Button("Count")){
            counter++;
        }
        ImGui::SameLine();
        ImGui::Text("Count: %i", counter);
        ImGui::End();
    }
};

int main() {
    engine::Application application;
    application.push_layer(new MyLayer());
    application.run();
    return 0;
}
