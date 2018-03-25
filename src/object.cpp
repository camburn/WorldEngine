#include "object.hpp"

Entity::Entity() {

}

void Entity::attach(Sprite sprite_object) {
    sprite = sprite_object;
}

Entity& Manager::new_entity(string name) {
    entity_map.emplace(name, Entity());
    return entity_map[name];
}