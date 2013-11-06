#ifndef __CELLMATRIX_H_INCLUDED__
#define __CELLMATRIX_H_INCLUDED__
#include "SDL/SDL.h"
#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <assert.h>
#include <vector>
#include <math.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
class Cell{
public:
    unsigned int is_frozen;
    unsigned int is_slope;
    int is_hit;
};

class CellMatrix {
private:
    Cell * matrix;
    unsigned int width;  //unit cell
    unsigned int height; //unit cell
    
public:
    CellMatrix(unsigned int _width, unsigned int _height);
    
    // pixel units
    static const unsigned int getCellSize() {return 10;}
    
    unsigned int getWidth ();
    unsigned int getHeight ();
    
    Cell * getCellIndex(unsigned int x, unsigned int y);
    Cell * getCellByPixel(unsigned int x, unsigned int y);
    SDL_Rect getCellRectByPixel(unsigned int x, unsigned int y);
    ~CellMatrix(){ delete[] matrix;}
};



#endif