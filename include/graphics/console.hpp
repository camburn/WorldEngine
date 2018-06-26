#ifndef CONSOLE_INCLUDED
#define CONSOLE_INCLUDED

#include "imgui.h"
#include <ctype.h>          // toupper, isprint
#include <math.h>           // sqrtf, fabsf, fmodf, powf, cosf, sinf, floorf, ceilf
#include <stdio.h>          // vsnprintf, sscanf, printf
#include <stdlib.h>         // NULL, malloc, free, qsort, atoi
#include <stdint.h>         // intptr_t
#include <vector>

#include "pyconsole.hpp"
#include "managers/state_manager.hpp"

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void ShowPyEngineConsole(bool* p_open);
void ShowFrameInformation(bool* p_open);
void ShowMainMenu(bool* p_open);
void MenuParts(bool* p_open, State& state);
void ConsoleWriteLine(const char *line_text);
void set_mouse_world_pos(float x, float y);
struct PyEngineConsole;

#endif // !CONSOLE_INCLUDED