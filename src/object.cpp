#include "object.hpp"

Entity::Entity() {

}

void Entity::attach(Sprite sprite_object) {

}

void Entity::render() {
    // How does my entity find the renderer?
    // maybe it doesn't? probly doesn't need to know what `render` even is
    // Just relay on the Sprite/Model/Primitive manager to draw things.
    // This can just manage the data/state of those things
}

void Entity::set_position(glm::vec3 new_position) {
    // if (Sprite) {} // No null
}

void Entity::attach(Sprite sprite_object) {
    sprite = sprite_object;
}

Entity& Manager::new_entity(string name) {
    entity_map.emplace(name, Entity());
    return entity_map[name];
}