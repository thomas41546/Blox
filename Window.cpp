#include "Globals.hpp"
#include "CellMatrix.hpp"
#include "Entity.hpp"
#include "Window.hpp"
#include "SDL/SDL_ttf.h"

Window::Window (int _x,int _y, int _width, int _height, int _maxScrollWidth, int _maxScrollHeight){
    x = _x;
    y = _y;
    width = _width;
    height = _height;
    maxScrollWidth = _maxScrollWidth;
    maxScrollHeight = _maxScrollHeight;
    
    SDL_Init( SDL_INIT_EVERYTHING );
    
    surface = SDL_SetVideoMode( width, height, 24, SDL_HWSURFACE|SDL_DOUBLEBUF );
    
    assert(TTF_Init() != -1);
    
    font = TTF_OpenFont( "FreeMonoBold.ttf", 25 );
    assert(font != NULL);
};

void Window::lockSurface(){
    surface_mutex.lock();
}

void Window::unlockSurface(){
    surface_mutex.unlock();
}

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

void Window::renderFont(int ox, int oy, std::string text){
    
    
    SDL_Color statColor = {0,255,0,255};
    SDL_Color blackColor = {0,0,0,255};
    SDL_Surface* message = TTF_RenderText_Shaded(font, text.c_str(), statColor,blackColor );
    assert(message != NULL);
    
    SDL_Rect offset = {ox,oy,0,0};
    
    SDL_BlitSurface( message, NULL, getSurface(), &offset );
    
    SDL_FreeSurface(message);
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
        }
    }
};

void Window::renderEntities(std::vector<Entity *> entities){
    for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
        ((Entity *)(*it))->render(getSurface(),getX(),getY());
    }
};

void Window::renderFinish(){
   surface_mutex.lock();
   SDL_Flip(getSurface() );
   surface_mutex.unlock();
};
