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

#define IM_ARRAYSIZE(_ARR)  ((int)(sizeof(_ARR)/sizeof(*_ARR)))

void ShowExampleAppConsole(bool* p_open);
void ShowFrameInformation(bool* p_open);
void ShowMainMenu(bool* p_open);
void MenuParts(bool* p_open);
void set_mouse_world_pos(float x, float y);
struct ExampleAppConsole;

#endif // !CONSOLE_INCLUDED