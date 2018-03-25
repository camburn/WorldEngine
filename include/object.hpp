#include <unordered_map>
#include <glm/glm.hpp>
#include "graphics/renderer.hpp"


class Entity {
public:
    Entity();
    void attach(Sprite sprite_object);
private:
    glm::vec3 position;

    Sprite sprite;
    //Primitive primitive;
    //Model model;
};

class Manager {
public:
    Manager() {};
    void draw_entities();
    Entity& new_entity(string name);

private:
    std::unordered_map<std::string, Entity> entity_map;
};