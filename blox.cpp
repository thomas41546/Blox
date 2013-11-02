/*
 Game: Pixie
 Author: Thomas Polasek
 Date: October 22nd, 2011
 Licence: None. Use as you wish. However, some credit would be nice.
 */

#include "SDL/SDL.h"

#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <assert.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


// colors
const int COLOR_WHITE =   0xFFFFFF;
const int COLOR_BLACK =   0x000000;
const int COLOR_RED =     0xFF0000;
const int COLOR_RED2 =     0xEE0000;
const int COLOR_BLUE =  0x33CCFF;
const int COLOR_GREEN =   0x00FF00;

const int CELL_WIDTH =   120;
const int CELL_HEIGHT =   80;
const int SCREEN_WIDTH =   1200;
const int SCREEN_HEIGHT =   800;

const int SCROLL_AMOUNT = 1;

const int MAIN_LOOP_DELAY =   10;


class Cell{
public:
    unsigned int is_frozen;
    unsigned int is_herd;
};

class CellMatrix {
private:
    Cell * matrix;
    unsigned int width;  //unit cell
    unsigned int height; //unit cell
    
public:
    CellMatrix(unsigned int _width, unsigned int _height){
        width = _width;
        height = _height;
        matrix = new Cell[width*height]();
        
        unsigned i,j;
        for(j = 0; j < height; j++){
            for(i = 0; i < width; i++){
                (getCellIndex(i,j))->is_frozen = 0;
            }
        }
    }
    
    // pixel units
    static unsigned int getCellSize() {return 10;}
    
    unsigned int getWidth () {return width;}
    unsigned int getHeight () {return height;}
    
    Cell * getCellIndex(unsigned int x, unsigned int y){
        if (y >= 0 && x < width && y >= 0 && y < height){
            return &(matrix[x + y*width]);
        }
        else{
            return NULL;
        }
    }
    
    ~CellMatrix(){
        delete[] matrix;
    }
};

class Window {
private:
    unsigned int x;
    unsigned int y;
    
    unsigned int width;
    unsigned int height;
    
    unsigned int maxScrollWidth;
    unsigned int maxScrollHeight;
    
    SDL_Surface* surface;
    
public:
    Window (int _x,int _y, int _width, int _height, int _maxScrollWidth, int _maxScrollHeight){
        x = _x;
        y = _y;
        width = _width;
        height = _height;
        maxScrollWidth = _maxScrollWidth;
        maxScrollHeight = _maxScrollHeight;
        
        SDL_Init( SDL_INIT_EVERYTHING );
        surface = SDL_SetVideoMode( width, height, 24, SDL_SWSURFACE );
    }
    
    unsigned int getX () {return x;}
    unsigned int getY () {return y;}
    
    unsigned int getWidth () {return width;}
    unsigned int getHeight () {return height;}
    
    SDL_Surface* getSurface () {return surface;}
    
    void scrollHorizonally(int amount) {
        x += amount;
        if(x < 0) x = 0;
        if(x >= maxScrollWidth) x = maxScrollWidth-1;
    }
    
    void scrollVertically(int amount) {
        y += amount;
        if(y < 0) y = 0;
        if(y >= maxScrollHeight) y = maxScrollHeight-1;
    }
    
    void renderCells(CellMatrix & cells){
        
        //blank
        SDL_Rect rect = {0,0,getWidth(), getHeight()};
        SDL_FillRect(getSurface(), &rect, COLOR_WHITE);
        
        int cell_pixel_width = CellMatrix::getCellSize();
        int cell_pixel_height = CellMatrix::getCellSize();
        
        unsigned int i,j;
        
        for(j = 0; j < cells.getHeight(); j++){
            for(i = 0; i < cells.getWidth(); i++){
                
                SDL_Rect rect = {i*cell_pixel_width,
                                 j*cell_pixel_height,
                                 cell_pixel_width,
                                 cell_pixel_height};
                
                rect.x -= getX();
                rect.y -= getY();
                
                if((cells.getCellIndex(i,j))->is_frozen){
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
    }
    void renderFinish(){
         SDL_Flip(getSurface() );
    }
};

static CellMatrix cells(CELL_WIDTH,CELL_HEIGHT);
static Window * mainWindow;


int calculate_Cell_X(int x){
	return  x/(mainWindow->getWidth()/cells.getWidth());
}

int calculate_Cell_Y(int y){
	return y/(mainWindow->getHeight()/cells.getHeight());
}

void toggle_Cell(int mouse_x, int mouse_y){
	int i = calculate_Cell_X(mouse_x);
	int j = calculate_Cell_Y(mouse_y);
    (cells.getCellIndex(i,j))->is_frozen ^= 1;
}


void exit_Game(){
	SDL_Quit();
	exit(0);
}

int main( int argc, char* args[] ){
    
    srand ( time(NULL) );
    
    mainWindow = new Window(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,3000,3000);
    
	int is_game = 1;
    
	while(is_game){
		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_MOUSEBUTTONDOWN:
					toggle_Cell(event.button.x, event.button.y);
					break;
                    
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_LEFT)
                        mainWindow->scrollHorizonally(-1*SCROLL_AMOUNT);
                    else if(event.key.keysym.sym == SDLK_RIGHT)
                        mainWindow->scrollHorizonally(SCROLL_AMOUNT);
                    else if(event.key.keysym.sym == SDLK_UP)
                        mainWindow->scrollVertically(-1*SCROLL_AMOUNT);
                    else if(event.key.keysym.sym == SDLK_DOWN)
                        mainWindow->scrollVertically(SCROLL_AMOUNT);
                    break;
                    
				case SDL_QUIT:
					is_game = 0;
					break;
			}
		}
        
		mainWindow->renderCells(cells);
        mainWindow->renderFinish();
		SDL_Delay( MAIN_LOOP_DELAY );
	}
	printf("You lost the game.\nNoob!\n");
    exit_Game();
	return 0;
}
