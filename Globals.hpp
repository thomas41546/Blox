#ifndef __GLOBALS_H_INCLUDED__
#define __GLOBALS_H_INCLUDED__

#include "SDL/SDL.h"
#include <assert.h> 

const SDL_Color COLOR_WHITE =   {255,255,255,255};
const SDL_Color COLOR_BLACK =   {0,0,0,255};
const SDL_Color COLOR_RED =     {255,0,0,255};
const SDL_Color COLOR_RED2 =    {0xEE,0,0,255};
const SDL_Color COLOR_BLUE =    {0x33,0xCC,0xFF,255};
const SDL_Color COLOR_GREEN =   {0,255,0,255};
const SDL_Color COLOR_LOUNGE = {0x70,0xA8,0xA8};

const int CELL_WIDTH =   1000;
const int CELL_HEIGHT =   1000;
const int SCREEN_WIDTH =   1200;
const int SCREEN_HEIGHT =   800;

const int MAIN_LOOP_DELAY =   10;

const int MAX_VELOCITY = 9;

const double MATH_PI = 3.14159;

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

typedef struct Double_Rect {
    double x,y,w,h;
} Double_Rect;

#endif
