#ifndef _LIGHTS_HPP
#define _LIGHTS_HPP

#include <glm/glm.hpp>


namespace engine {

class Light {
public:
    Light(): color(1.0f, 1.0f, 1.0f), position(0.0f, 0.0f, 0.0f), cast_shadows(true) {}
    Light(glm::vec3 color, glm::vec3 position)
        : color(color), position(position)
    {}
    Light(glm::vec3 color, glm::vec3 position, bool direction)
        : color(color), position(position), direction(direction)
    {}
    glm::vec3 color;
    glm::vec3 position;
    bool direction = false;
    bool cast_shadows = true;
    bool enabled = true;
    

    glm::vec3 get_hdr_color() { return color * 255.0f; }
};

}

#endif