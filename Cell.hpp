#ifndef _Cell_hpp
#define _Cell_hpp
#include "Globals.hpp"
#include "Drawing.hpp"

#include "SDL/SDL.h"


class CellMatrix;

class Cell{
    
public:
    enum SlopeType { NONE, LEFT, RIGHT};
    
    
private:    
    bool is_filled;
    bool is_breakable;
    SlopeType slope;
    
    CellMatrix * cellMatrix;
    
    SDL_Color base_color;
    
public:
    
    Cell();
    void setCellMatrix(CellMatrix * _cellMatrix);
    bool destroy();
    
    void setSlope(SlopeType _slope);
    void resetSlope();
    
    void setFilled();
    void resetFilled();
    bool isFilled();
    
    void setBaseColor(SDL_Color _baseColor);
    void render(SDL_Rect & rect);
    
    
};


#endif
