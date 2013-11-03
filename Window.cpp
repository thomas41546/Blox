#include "Globals.h"
#include "CellMatrix.h"
#include "Entity.h"
#include "Window.h"

Window::Window (int _x,int _y, int _width, int _height, int _maxScrollWidth, int _maxScrollHeight){
    x = _x;
    y = _y;
    width = _width;
    height = _height;
    maxScrollWidth = _maxScrollWidth;
    maxScrollHeight = _maxScrollHeight;
    
    SDL_Init( SDL_INIT_EVERYTHING );
    surface = SDL_SetVideoMode( width, height, 24, SDL_SWSURFACE );
};

unsigned int Window::getX () {return x;}
unsigned int Window::getY () {return y;}

unsigned int Window::getWidth () {return width;}
unsigned int Window::getHeight () {return height;}

SDL_Surface* Window::getSurface () {return surface;}

void  Window::scrollHorizonally(int amount) {
    if(x + amount >= maxScrollWidth) {
        if(amount <= 0)
            x = 0;
        else
            x = maxScrollWidth-1;
    }
    else
        x += amount;
};

void  Window::scrollVertically(int amount) {
    if(y + amount >= maxScrollHeight){
        if(amount <= 0)
            y = 0;
        else
            y = maxScrollHeight-1;
    }
    else
        y += amount;
};

void Window::setXY(unsigned int _x, unsigned int _y){
    x = _x;
    y = _y;
};

void Window::renderStart(){
    //blank
    SDL_Rect rect = {0,0,getWidth(), getHeight()};
    SDL_FillRect(getSurface(), &rect, COLOR_WHITE);
};

void Window::renderCells(CellMatrix & cells){
    
    
    unsigned int cellSize = CellMatrix::getCellSize();
    
    unsigned int i,j;
    
    unsigned int start_i = getX();
    unsigned int start_j = getY();
    
    unsigned int end_i = getX() + getWidth();
    unsigned int end_j = getY() + getHeight();
    
    for(j = start_j; j < end_j; j+= cellSize){
        for(i = start_i; i < end_i; i+=cellSize){
            
            SDL_Rect rect = {(i - start_i),
                (j - start_j),
                cellSize,
                cellSize};
            
            if(cells.getCellByPixel(i,j) != NULL && (cells.getCellByPixel(i,j))->is_frozen){
                SDL_FillRect(getSurface(), &rect, COLOR_BLACK);
                rect.x += 1;
                rect.y += 1;
                rect.w -= 2;
                rect.h -= 2;
                SDL_FillRect(getSurface(), &rect, COLOR_WHITE);
            }
            else{
                SDL_FillRect(getSurface(), &rect, COLOR_WHITE);
            }
        }
    }
};

void Window::renderEntities(std::vector<Entity *> entities){
    for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
        ((Entity *)(*it))->render(getSurface(),getX(),getY());
    }
};

void Window::renderFinish(){
    SDL_Flip(getSurface() );
};