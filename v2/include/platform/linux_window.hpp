#include "window.hpp"

namespace engine {

class LinuxWindow: public Window {

public:
    LinuxWindow();

    void on_update() override;

    unsigned int get_width() override;
    unsigned int get_height() override;
};

}
