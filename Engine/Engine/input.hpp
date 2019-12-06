#ifndef _INPUT_HPP
#define _INPUT_HPP

#include "imgui.h"

namespace engine {

#define LEFT_CLICK 0
#define RIGHT_CLICK 1
#define MIDDLE_CLICK 2

bool get_mouse_button(int button) {
    return ImGui::IsMouseClicked(button);
}

bool get_key_button(int button) {
    return ImGui::IsKeyDown(button);
}

}

#endif