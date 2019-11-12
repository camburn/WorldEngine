#ifndef _SCRIPTS_HPP
#define _SCRIPTS_HPP

#include <memory>
#include <string>
#include <chrono>

#include <Python.h>

#include <glm/glm.hpp>

#include "engine.hpp"
#include "Engine/objects.hpp"
//#include "Engine/python_api.hpp"


namespace engine {

/*
A script should have code that is run every update, sim update, ui update, or 
phyics update.

It should also contain a series of functions to manipulate state of Objects and
their attached components.

Finally they should have a series of defined events, these events can trigger
at the beggining of every of the above update methods but will receive
particular arguments.

i.e. Collision event args(self, other_object) would be called during the physics
update.
*/

class Script {

public:
    Script(std::string name, std::shared_ptr<Object> parent): name(name), parent(parent) {}

    ~Script(){}

    virtual void update(float delta_time) = 0;
    virtual void reload() = 0;

    std::string name;
    std::string source;

protected:
    std::shared_ptr<Object> parent;
};

}

#endif