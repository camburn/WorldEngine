#include "tools/profiler.hpp"

float frame_start_time;
float frame_end_time;
float last_time;


class Average {
public:
    Average(unsigned int max_size=100) : max_size(max_size) {
        series.reserve(100);
        counter = 0;
        sample_count = 0;
    }
    
    void add_sample(float sample) {
        if (sample_count < max_size) {
            sample_count ++;
            series.push_back(sample);
        } else {
            // we are full
            if (counter >= max_size) {
                counter = 0;
            }
            series[counter] = sample;
        }
    }

    float get_average() {
        float total = 0.0f;
        for (float sample : series) {
            total += sample;
        }
        return total / sample_count;
    }

private:
    std::vector<float> series;
    unsigned int max_size;
    unsigned int counter;
    unsigned int sample_count;
};

std::map<std::string, Average> frame_sections;

//accumulator = (alpha * new_value) + (1.0 - alpha) * accumulator
float alpha = 1.0f;

float accumulate(float delta, float accumulation) {
    accumulation = (alpha * delta) + (1.0 - alpha) * accumulation;
    return accumulation;
}

void frame_start (float start_time) {
    float delta_time = start_time - last_time;
    last_time = start_time;
    // reset previous frame
    //frame_sections.clear();
    if (frame_sections.count("start") == 0) {
        frame_sections.emplace("start", Average{});
    }
    frame_sections["start"].add_sample(delta_time);
    frame_start_time = start_time;
}

void frame_section(std::string name, float section_time) {
    float delta_time = section_time - last_time;
    last_time = section_time;
    if (frame_sections.count(name) == 0) {
        frame_sections.emplace(name, Average{});
    }
    frame_sections[name].add_sample(delta_time);
}

void frame_end(float end_time) {
    float delta_time = end_time - last_time;
    last_time = end_time;

    if (frame_sections.count("end") == 0) {
        frame_sections.emplace("end", Average{});
    }
    frame_sections["end"].add_sample(delta_time);
    frame_end_time = end_time;
}

#include <stdint.h>

//  Windows
#ifdef _WIN32

#include <intrin.h>
uint64_t rdtsc(){
    return __rdtsc();
}

//  Linux/GCC
#else

uint64_t rdtsc(){
    unsigned int lo,hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}

#endif

void draw_profiler(bool* p_open) {
    float total_frame_time = frame_end_time - frame_start_time;
    frame_sections["total_frame_time"].add_sample(total_frame_time);
    ImGui::Begin("Menu", p_open);
    
    ImGui::Columns(3, "mycolumns");
    ImGui::Separator();
    ImGui::Text("Name"); ImGui::NextColumn();
    ImGui::Text("Time"); ImGui::NextColumn();
    ImGui::Text("Percent"); ImGui::NextColumn();
    ImGui::Separator();

    float frame_time_average = frame_sections["total_frame_time"].get_average();
    for (std::pair<std::string, Average> section : frame_sections) {
        float average = section.second.get_average();
        ImGui::Text(section.first.c_str()); ImGui::NextColumn();
        ImGui::Text("%f", average); ImGui::NextColumn();
        ImGui::Text("%f", average / frame_time_average); ImGui::NextColumn();
    }
    ImGui::Text("Total"); ImGui::NextColumn();
    ImGui::Text("%f", total_frame_time); ImGui::NextColumn();
    ImGui::NextColumn();

    uint64_t cycle_count = rdtsc();

    ImGui::Text("Cycles"); ImGui::NextColumn();
    ImGui::Text("%i", cycle_count); ImGui::NextColumn();
    ImGui::NextColumn();

    ImGui::End();
}
