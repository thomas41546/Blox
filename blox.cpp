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
#include <math.h>

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
                /*
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
                 */
                if(sqrt((i - 40)*(i -40)+ (j - 40)*(j - 40)) < 20)
                    (getCellIndex(i,j))->is_frozen = 0;
                else
                    (getCellIndex(i,j))->is_frozen = 1;
              
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
    
    SDL_Rect getCellRectByPixel(unsigned int x, unsigned int y){
        x /= CellMatrix::getCellSize();
        y /= CellMatrix::getCellSize();
        Cell * cell = getCellIndex(x,y);
        
        SDL_Rect rect = {x*CellMatrix::getCellSize(), y*CellMatrix::getCellSize(),
                         CellMatrix::getCellSize(), CellMatrix::getCellSize()};
        return rect;
    }
    
    ~CellMatrix(){
        delete[] matrix;
    }
};


int collisionDetectRIR(SDL_Rect box1, SDL_Rect box2)
{
    if ((box2.x>box1.x)&&((box1.x+box1.w)>box2.x))
        if ((box2.y>box1.y)&&((box1.y+box1.h)>box2.y)) return 1;
    
    return 0;
}



class EntityManager{
    
};


class Entity{
    public:
        //TODO encapsulate
        unsigned int width,height;
        double vx,vy;
        double x,y;
        bool hitGround;
    
        Entity(SDL_Rect _dimensions){
            x = (double)_dimensions.x;
            y = (double)_dimensions.y;
            width = _dimensions.w;
            height = _dimensions.h;
            vx = 0;
            vy = 0;
            hitGround = false;
        }
    
        SDL_Rect getRect(){
            SDL_Rect rect = {(unsigned int)x, (unsigned int)y, width,height};
            return rect;
        }
    
        void render(SDL_Surface* surface, int offsetX, int offsetY){
            SDL_Rect rect = getRect();
            rect.x -= offsetX;
            rect.y -= offsetY;
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
    void renderEntities(std::vector<Entity *> entities){
        for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
            ((Entity *)(*it))->render(getSurface(),getX(),getY());
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
    
    SDL_Rect playerRect = {500,500,20,20};
    playerEntity = new Entity(playerRect);
    entities.push_back(playerEntity);
    
    for(int i = 0; i < 1000; i++){
        SDL_Rect npcRect = {rand()%1000,rand()%1000,3,3};
        entities.push_back(new Entity(npcRect));
    }
    
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
                        playerEntity->vx -= 0.3;
                    else if(event.key.keysym.sym == SDLK_RIGHT)
                        playerEntity->vx += 0.3;
                    
                    else if(event.key.keysym.sym == SDLK_UP){
                        if(abs(playerEntity->vy) < 0.01 && playerEntity->hitGround)
                            playerEntity->vy -= 3;
                    }
                    else if(event.key.keysym.sym == SDLK_DOWN)
                        playerEntity->y += SCROLL_AMOUNT;
                    break;
                    
				case SDL_QUIT:
					is_game = 0;
					break;
			}
		}
        //npc
         for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
             Entity * npc =  ((Entity *)(*it));
             if(npc == playerEntity) continue;
             
             if(npc->hitGround){
                 if(playerEntity->x < npc->x){
                     npc->vx += 100;
                 }
                 if(playerEntity->x > npc->x){
                     npc->vx += 100;
                 }
                 //npc->vy = -1*rand() % 8;
                // npc->vx += ((double)(rand() % 100 - 50))/5;
             }
         }
        
        //physics
        for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
            
            ((Entity *)(*it))->vy += 0.1;
            ((Entity *)(*it))->vx *= 0.90;
            
            SDL_Rect entityRect;
            
            
            entityRect = ((Entity *)(*it))->getRect();
            entityRect.y += ((Entity *)(*it))->vy;
            
            
            bool collidedY = false;
            bool collidedX = false;
            int i,j;
            for(j = entityRect.y; j <= entityRect.y + entityRect.h; j+= CellMatrix::getCellSize()){
                for(i = entityRect.x; i <=  entityRect.x + entityRect.w; i+=CellMatrix::getCellSize()){
                    if( cells.getCellByPixel(i,j)!= NULL && cells.getCellByPixel(i,j)->is_frozen){
                        
                        if(((Entity *)(*it))->vy > 0.1){((Entity *)(*it))->vy -= 0.1;}
                        ((Entity *)(*it))->y -= (((Entity *)(*it))->vy);
                        
                        //above
                        if(j <= (entityRect.y + entityRect.h/2)){
                            if(((Entity *)(*it))->vy < 0)
                                ((Entity *)(*it))->vy = 0;
                        }
                        else{  //below
                          
                            if(((Entity *)(*it))->vy > 0){
                                ((Entity *)(*it))->vy = 0;
                                ((Entity *)(*it))->hitGround = true;
                            }
                        }
                        collidedY = true;
                    }
                }
            }
            if(collidedY){
                    
            }
            else{
                ((Entity *)(*it))->hitGround = false;
            }
            
            entityRect = ((Entity *)(*it))->getRect();
            entityRect.x += ((Entity *)(*it))->vx;
            for(j = entityRect.y; j <= entityRect.y + entityRect.h; j+= CellMatrix::getCellSize()){
                for(i = entityRect.x; i <=  entityRect.x + entityRect.w; i+=CellMatrix::getCellSize()){
                    if( cells.getCellByPixel(i,j)!= NULL && cells.getCellByPixel(i,j)->is_frozen){
                        
                        ((Entity *)(*it))->x -= ((Entity *)(*it))->vx;
                        
                        //left
                        if(i <= (entityRect.x + entityRect.w/2)){
                            if(((Entity *)(*it))->vx < 0)
                                ((Entity *)(*it))->vx = 0;
                        }
                        else{  //right
                            
                            if(((Entity *)(*it))->vx > 0){
                                ((Entity *)(*it))->vx = 0;
                            }
                        }
                        collidedX = true;
                    }
                }
            }
            if(collidedX){
            }
            
            if(!collidedX)
                ((Entity *)(*it))->x += ((Entity *)(*it))->vx;
            
            if(!collidedY)
                ((Entity *)(*it))->y += ((Entity *)(*it))->vy;
            
            continue;
            //nothing
            
        }
        
        // set window position
        //TODO make this a moving window when you get to the sides
        int nx,ny;
        nx = playerEntity->x - (int)500;
        ny = playerEntity->y - (int)500;
        if(nx < 0) nx = 0;
        if(ny < 0) ny = 0;
        //mainWindow->setXY((nx/2)*2,(ny/2)*2);
        
        mainWindow->renderStart();
		mainWindow->renderCells(cells);
        mainWindow->renderEntities(entities);
        
        mainWindow->renderFinish();
		SDL_Delay( 0 );
	}
	printf("You lost the game.\nNoob!\n");
    exit_Game();
	return 0;
}
