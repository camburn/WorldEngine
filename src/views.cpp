/*
Views are the highest level of some visualisation

ViewObject
 |
 +-- 3D Mesh
 +-- 3D Primitive (Cube, Plane, Cylinder, Sphere, Etc...)
 +-- Debug View (Lines, Cube outline)
 +-- Sprite (Plane with texture)


*/
#include "views.hpp"

View::View() {
    int var = 1;
}

bool attach_mesh(std::string mesh_name) {
    return false;
}

bool attach_sprite(std::string sprite_name) {
    return false;
}

bool attach_primitive(std::string primitive_name) {
    return false;
}
