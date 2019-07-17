#include "platform/linux_window.hpp"

namespace engine {

Window* Window::create() {
    return new LinuxWindow();
}

LinuxWindow::LinuxWindow() {}
void LinuxWindow::on_update() {}

unsigned int LinuxWindow::get_width() {
    return 0;
}

unsigned int LinuxWindow::get_height() {
    return 0;
}

} // namespace
