#pragma once
#include <math.h>
#include <string>
#include <iostream>
#include <shapefil.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <glm/glm.hpp>

#include "graphics/triangulate.hpp"
#include "graphics/buffers.hpp"

void LoadShapeFile(const std::string file_name);
void draw_shapes();
