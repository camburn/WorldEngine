#include <unordered_map>
#include <vector>
#include <string>

std::unordered_map<std::string, float> frame_sections;


void frame_start (float start_time) {
    // reset previous frame
    frame_sections.clear();
    frame_sections.emplace("start", start_time);
}

void frame_section(std::string name, float time) {
    frame_sections.emplace(name, time);
}

void frame_end(float end_time) {
    frame_sections.emplace("end", end_time);
}
