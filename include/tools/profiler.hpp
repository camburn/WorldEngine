#include <map>
#include <vector>
#include <string>
#include "imgui.h"



void frame_start (float start_time);
void frame_section(std::string name, float time);
void frame_end(float end_time);

void draw_profiler(bool* p_open);