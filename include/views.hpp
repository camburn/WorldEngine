#include <string>

#ifndef VIEW_H
#define VIEW_H
class View {
public:
    View();

    bool attach_mesh(std::string mesh_name);
    bool attach_sprite(std::string sprite_name);
    bool attach_primitive(std::string primitive_name);

private:
};
#endif 