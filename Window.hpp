#ifndef __WINDOW_H_INCLUDED__
#define __WINDOW_H_INCLUDED__


#include "SDL/SDL.h"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_opengl.h"

#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/thread/thread.hpp>
#include <map>
#include <assert.h>
#include <vector>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

class Window {
private:
    unsigned int x;
    unsigned int y;
    
    unsigned int width;
    unsigned int height;
    
    unsigned int maxScrollWidth;
    unsigned int maxScrollHeight;

    std::map<std::string, GLuint> textures;
    
    TTF_Font* font;    
    SDL_Surface* surface;
    
    GLuint blockTexture;
    
    void setupOpenGL();
    
    SDL_Surface* getSurface ();
    
    
public:
    Window (int _x,int _y, int _width, int _height, int _maxScrollWidth, int _maxScrollHeight);
    
    unsigned int getX();
    unsigned int getY();
    
    unsigned int getWidth ();
    unsigned int getHeight ();
    
    SDL_Rect getRect();
    
    void scrollHorizonally(int amount);
    
    void scrollVertically(int amount);
    
    void setXY(unsigned int _x, unsigned int _y);
    void renderStart();
    void renderFont(int ox, int oy, std::string text);
    void renderCells(CellMatrix & cells);
    void renderEntities(std::vector<Entity *> entities);
    void renderFinish();
};

#endif
