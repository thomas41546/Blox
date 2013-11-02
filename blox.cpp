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
#include <vector>

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

const int CELL_WIDTH =   400;
const int CELL_HEIGHT =   400;
const int SCREEN_WIDTH =   1200;
const int SCREEN_HEIGHT =   800;

const int SCROLL_AMOUNT = 5;

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
        
        //map generation
        unsigned i,j;
        for(j = 0; j < height; j++){
            for(i = 0; i < width; i++){
                if(rand() % 50 == 0)
                    (getCellIndex(i,j))->is_frozen = 1;
                else{
                    if(i > 0 && j > 0 && i < width-1 && j < height-1){
                        if(getCellIndex(i,j+1)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
                        if(getCellIndex(i+1,j)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
                        if(getCellIndex(i,j-1)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
                        if(getCellIndex(i-1,j)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
                        if(getCellIndex(i-1,j-1)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
                        if(getCellIndex(i+1,j+1)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
                    }
                }
            }
        }
        
        
    }
    
    // pixel units
    static const unsigned int getCellSize() {return 10;}
    
    unsigned int getWidth () {return width;}
    unsigned int getHeight () {return height;}
    
    
    Cell * getCellIndex(unsigned int x, unsigned int y){
        if (x >= 0 && x < width && y >= 0 && y < height){
            return &(matrix[x + y*width]);
        }
        else{
            return NULL;
        }
    }
    
    
    Cell * getCellByPixel(unsigned int x, unsigned int y){
        x /= CellMatrix::getCellSize();
        y /= CellMatrix::getCellSize();
        return getCellIndex(x,y);
    }
    
    ~CellMatrix(){
        delete[] matrix;
    }
};

class EntityManager{
    
    
};


class Entity{
    public:
        //TODO encapsulate
        unsigned int width,height;
        float vx,vy;
        float x,y;
    
        Entity(SDL_Rect _dimensions){
            x = (float)_dimensions.x;
            y = (float)_dimensions.y;
            width = _dimensions.w;
            height = _dimensions.h;
            vx = 0;
            vy = 0;
        }
    
        SDL_Rect getRect(){
            SDL_Rect rect = {(unsigned int)x, (unsigned int)y, width,height};
            return rect;
        }
    
        void render(SDL_Surface* surface){
            SDL_Rect rect = getRect();
            SDL_FillRect(surface, &rect, COLOR_BLACK);
        }
    
};

// TODO convert x,y,width,height to SDL_Rect
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
        if(x + amount >= maxScrollWidth) {
            if(amount <= 0)
                x = 0;
            else
                x = maxScrollWidth-1;
        }
        else
            x += amount;
    }
    
    void scrollVertically(int amount) {
        if(y + amount >= maxScrollHeight){
            if(amount <= 0)
                y = 0;
            else
                y = maxScrollHeight-1;
        }
        else
            y += amount;
    }
    
    void setXY(unsigned int _x, unsigned int _y){
        x = _x;
        y = _y;
    }
    
    void renderStart(){
        //blank
        SDL_Rect rect = {0,0,getWidth(), getHeight()};
        SDL_FillRect(getSurface(), &rect, COLOR_WHITE);
    }
    
    void renderCells(CellMatrix & cells){
        
        
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
    }
    void renderFinish(){
         SDL_Flip(getSurface() );
    }
};

static CellMatrix cells(CELL_WIDTH,CELL_HEIGHT);
static Window * mainWindow;

// TODO abstract out to EntityManager
static std::vector<Entity *> entities;
static Entity *playerEntity;


void exit_Game(){
	SDL_Quit();
	exit(0);
}

int main( int argc, char* args[] ){
    
    srand ( time(NULL) );
    
    mainWindow = new Window(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,
                            CELL_WIDTH*CellMatrix::getCellSize(),
                            CELL_HEIGHT*CellMatrix::getCellSize()); //TODO fix this
    
    SDL_Rect playerRect = {40,40,20,20};
    playerEntity = new Entity(playerRect);
    entities.push_back(playerEntity);
    
    SDL_EnableKeyRepeat(1, 5);
    
	int is_game = 1;
    int fps = 0;
    unsigned int lastTime = SDL_GetTicks();
    
	while(is_game){
        unsigned int curTime = SDL_GetTicks();
        if(curTime - lastTime >= 1000){
            printf("FPS: %d\n",fps);
            lastTime = curTime;
            fps = 0;
        }else{
            fps += 1;
        }
        
		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
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
        
        //physics
        for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
            
            ((Entity *)(*it))->vy += 0.01; // TODO define gravity;
            ((Entity *)(*it))->y += ((Entity *)(*it))->vy;
             mainWindow->setXY(((Entity *)(*it))->x,((Entity *)(*it))->y);
            
        }
        
        mainWindow->renderStart();
		mainWindow->renderCells(cells);
        for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
            ((Entity *)(*it))->render(mainWindow->getSurface());
        }
        
        mainWindow->renderFinish();
		SDL_Delay( 0 );
	}
	printf("You lost the game.\nNoob!\n");
    exit_Game();
	return 0;
}
