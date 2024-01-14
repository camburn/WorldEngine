#ifndef _INPUT_HPP
#define _INPUT_HPP

#include "imgui.h"

namespace engine {

#define LEFT_CLICK 0
#define RIGHT_CLICK 1
#define MIDDLE_CLICK 2

bool is_mouse_clicked(int button) {
    return ImGui::IsMouseClicked(button);
}

bool is_key_down(int button) {
    return ImGui::IsKeyDown(button);
}

bool is_key_pressed(int button) {
    return ImGui::IsKeyPressed(button);
}

std::pair<float, float> get_mouse_position() {
    ImVec2 pos = ImGui::GetMousePos();
    return {pos.x, pos.y};
}

}

#endif