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

const int CELL_WIDTH =   4000;
const int CELL_HEIGHT =   4000;
const int SCREEN_WIDTH =   1200;
const int SCREEN_HEIGHT =   800;

const int MAIN_LOOP_DELAY =   10;

const int MAX_VELOCITY = 9;


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
                }*/
                 
                if(sqrt((i - 40)*(i -40)+ (j - 40)*(j - 40)) < 40)
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


class Entity{
    protected:
            bool dead;
    public:
    
        enum EntityType { NPC,NPC_WORM,PLAYER,BULLET,DEFAULT, };
    
        //TODO encapsulate
        unsigned int width,height;
        double vx,vy;
        double x,y;
        bool hitGround;
    
        static bool isEntityDead(Entity * e){
            return e->isDead();
        }
    
        Entity(SDL_Rect _dimensions){
            x = (double)_dimensions.x;
            y = (double)_dimensions.y;
            width = _dimensions.w;
            height = _dimensions.h;
            vx = 0;
            vy = 0;
            hitGround = false;
            dead = false;
        }
    
        SDL_Rect getRect(){
            SDL_Rect rect = {(unsigned int)x, (unsigned int)y, width,height};
            return rect;
        }
        void setDead(){
            dead = true;
        }
        bool isDead(){
            return dead;
        }
    
        virtual EntityType getType(){
            return DEFAULT;
        }
        
        virtual void render(SDL_Surface* surface, int offsetX, int offsetY){
            SDL_Rect rect = getRect();
            rect.x -= offsetX;
            rect.y -= offsetY;
            SDL_FillRect(surface, &rect, COLOR_BLACK);
        }
    
        virtual void applyGravity(float mag){
            vy += mag;
        }
    
        virtual void applyHorizontalDrag(float mag){
            vx *= mag;
        }
    
        virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
        }
     
    
};

class PlayerEntity: public Entity
{
public:
    PlayerEntity(SDL_Rect _dimensions) : Entity(_dimensions){

    }
    
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY){
        SDL_Rect rect = getRect();
        rect.x -= offsetX;
        rect.y -= offsetY;
        SDL_FillRect(surface, &rect, COLOR_RED);
    }
    virtual EntityType getType(){
        return PLAYER;
    }
};
static PlayerEntity *playerEntity;


class NPCEntity: public Entity
{
public:
    NPCEntity(SDL_Rect _dimensions) : Entity(_dimensions){
        
    }
    
    virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
        if(abs(vy) < 0.01 &&  abs(vx) < 0.01 && hitGround){
            vy -= rand()% 6 + 1;
            if(playerEntity->x < x)
                vx = -1*rand()% 8;
            else if(playerEntity->x > x)
                vx = rand()% 8;
        }
    }
    virtual EntityType getType(){
        return NPC;
    }
};

class WormBodyEntity: public NPCEntity
{
public:
    WormBodyEntity(SDL_Rect _dimensions) : NPCEntity(_dimensions){
    }
    
    virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
    }
    
    virtual void applyGravity(float mag){
    }
    
    virtual void applyHorizontalDrag(float mag){
    }
    
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY){
        SDL_Rect rect = getRect();
        rect.x -= offsetX;
        rect.y -= offsetY;
        SDL_FillRect(surface, &rect, COLOR_BLUE);
    }
    
    virtual EntityType getType(){
        return NPC_WORM;
    }
};

class WormEntity: public NPCEntity
{
private:
    static const int WORM_LENGTH = 20;
    static const int WORM_SPACING_DIVIDER = 5;
    
    WormBodyEntity * segments[WORM_LENGTH];
    
public:
    WormEntity(SDL_Rect _dimensions, std::vector<Entity *> & entities) : NPCEntity(_dimensions){
        for(int i = 0; i < WORM_LENGTH; i++){
            segments[i] = new WormBodyEntity(_dimensions);
            segments[i]->x = _dimensions.x - (_dimensions.w+5)*i;
            segments[i]->y = _dimensions.y;
            segments[i]->vx = 0;
            segments[i]->vy = 0;
            entities.push_back(segments[i]);
        }
        
    }
    
    virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
        static double lastRad = 0.0;
        if(rand() % 5 == 0)
            lastRad += 0.1;
        
        vx = cos(lastRad)*10;
        vy = sin(lastRad)*10;
        
        segments[0]->vx = (x -segments[0]->x)/WORM_SPACING_DIVIDER;
        segments[0]->vy = (y -segments[0]->y)/WORM_SPACING_DIVIDER;
        for(int i = 1; i < WORM_LENGTH; i++){
            segments[i]->vx = (segments[i-1]->x -segments[i]->x)/WORM_SPACING_DIVIDER;
            segments[i]->vy = (segments[i-1]->y -segments[i]->y)/WORM_SPACING_DIVIDER;
        }
    }
    
    
    virtual void applyGravity(float mag){
    }
    
    virtual void applyHorizontalDrag(float mag){
    }
    
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY){
        SDL_Rect rect = getRect();
        rect.x -= offsetX;
        rect.y -= offsetY;
        SDL_FillRect(surface, &rect, COLOR_BLUE);
    }
    
    virtual EntityType getType(){
        return NPC_WORM;
    }
    
    ~WormEntity(){
        for(int i = 0; i < WORM_LENGTH; i++){
            segments[i]->setDead();
        }
    }
};


class BulletEntity: public Entity
{
private:
    unsigned int lifetime;
    static const unsigned int BULLET_LIFE = 500;

public:
    BulletEntity(SDL_Rect _dimensions) : Entity(_dimensions){
        lifetime = BULLET_LIFE; //x * fps
    } 
    
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY){
        SDL_Rect rect = getRect();
        rect.x -= offsetX;
        rect.y -= offsetY;
        SDL_FillRect(surface, &rect, COLOR_GREEN);
    }
    
    virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
        lifetime -= 1;
        if(lifetime == 0){
            setDead();
        }
    }
        
    
    virtual void applyGravity(float mag){
    }
    
    virtual void applyHorizontalDrag(float mag){
    }
    
    virtual EntityType getType(){
        return BULLET;
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

void exit_Game(){
	SDL_Quit();
	exit(0);
}


int main( int argc, char* args[] ){
    
    static CellMatrix cells(CELL_WIDTH,CELL_HEIGHT);
    static Window * mainWindow;
    
    // TODO abstract out to EntityManager
    static std::vector<Entity *> entities;
    
    
    SDL_WM_SetCaption("Blox", "Blox");
    
    srand ( time(NULL) );
    
    mainWindow = new Window(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,
                            CELL_WIDTH*CellMatrix::getCellSize(),
                            CELL_HEIGHT*CellMatrix::getCellSize()); //TODO fix this
    
    SDL_Rect playerRect = {500,500,20,20};
    playerEntity = new PlayerEntity(playerRect);
    entities.push_back((Entity *)playerEntity);
    
    SDL_Rect wormRect = {500,500,40,40};
    entities.push_back(new WormEntity(wormRect,entities));
    
    for(int i = 0; i < 10000; i++){
        SDL_Rect npcRect = {rand()%200 + 300,rand()%200 + 300,11,11};
        entities.push_back(new NPCEntity(npcRect));
    }
    
    SDL_EnableKeyRepeat(1, 5);
    
	int is_game = 1;
    int fps = 0;
    unsigned int lastTime = SDL_GetTicks();
    
	while(is_game){
        unsigned int curTime = SDL_GetTicks();
        if(curTime - lastTime >= 1000){
            printf("FPS/%d Entities/%ld\n",fps,entities.size());
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
                    else if(event.key.keysym.sym == SDLK_SPACE){
                        
                        for(float rad = 0; rad < 6.28; rad += 0.02){
                            SDL_Rect bulletRect = {playerEntity->x,playerEntity->y,5,5};
                            BulletEntity * bullet =new BulletEntity(bulletRect);
                            bullet->vx = cos(rad + (rand() % 100)/100.0)*9;
                            bullet->vy = sin(rad + (rand() % 100)/100.0)*9;
                            
                            entities.push_back(bullet);
                        }
                        
                    }
                        
                    break;
                    
				case SDL_QUIT:
					is_game = 0;
					break;
			}
		}
        
        // Erase dead entities
        // TODO verify that correctly deallocates the entity (no memory overflow)
        entities.erase( std::remove_if(entities.begin(), entities.end(), Entity::isEntityDead), entities.end() );
        
        //AI
         for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
             Entity * entity =  ((Entity *)(*it));
             entity->applyAI(entities,cells);
         }
        
        //physics
        for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
            
            
            ((Entity *)(*it))->applyGravity(0.1);
            ((Entity *)(*it))->applyHorizontalDrag(0.9);
            
            if(((Entity *)(*it))->x < 0)((Entity *)(*it))->x = 0;
            if(((Entity *)(*it))->y < 0)((Entity *)(*it))->y = 0;
            
            if(((Entity *)(*it))->vy > MAX_VELOCITY) ((Entity *)(*it))->vy = MAX_VELOCITY;
            else if(((Entity *)(*it))->vy < -1*MAX_VELOCITY) ((Entity *)(*it))->vy = -1*MAX_VELOCITY;
            
            if(((Entity *)(*it))->vx > MAX_VELOCITY) ((Entity *)(*it))->vx = MAX_VELOCITY;
            else if(((Entity *)(*it))->vx < -1*MAX_VELOCITY) ((Entity *)(*it))->vx = -1*MAX_VELOCITY;
                
            SDL_Rect entityRect;
             
            
            entityRect = ((Entity *)(*it))->getRect();
            entityRect.y += ((Entity *)(*it))->vy;
            entityRect.x += ((Entity *)(*it))->vx;
            
            bool collidedY = false;
            bool collidedX = false;
            int i,j;
            for(j = entityRect.y; j <= entityRect.y + entityRect.h; j+= CellMatrix::getCellSize()){
                for(i = entityRect.x; i <=  entityRect.x + entityRect.w; i+=CellMatrix::getCellSize()){
                    if( cells.getCellByPixel(i,j)!= NULL && cells.getCellByPixel(i,j)->is_frozen){
                        
                        if(((Entity *)(*it))->getType() == Entity::BULLET){
                            cells.getCellByPixel(i,j)->is_frozen = 0;
                            ((Entity *)(*it))->setDead();
                            goto NEXT_ITERATION;
                        }
                        
                        if(((Entity *)(*it))->getType() == Entity::NPC_WORM){
                            cells.getCellByPixel(i,j)->is_frozen = 0;
                            continue;
                        }
                        
                        
                        //above
                        if(j <= (entityRect.y + entityRect.h/2)){
                            if(((Entity *)(*it))->vy < 0){
                                ((Entity *)(*it))->vy *= -0.1;
                                collidedY = true;
                            }
                        }
                        else{  //below
                          
                            if(((Entity *)(*it))->vy > 0){
                                ((Entity *)(*it))->vy  *= -0.1;
                                ((Entity *)(*it))->hitGround = true;
                                collidedY = true;
                            }
                        }
                        //left
                        if(i <= (entityRect.x + entityRect.w/2)){
                            if(((Entity *)(*it))->vx < 0){
                                ((Entity *)(*it))->vx  *= -0.1;
                                   collidedX = true;
                            }
                        }
                        else{  //right
                            
                            if(((Entity *)(*it))->vx > 0){
                                ((Entity *)(*it))->vx  *= -0.1;
                                collidedX = true;
                            }
                        }
                        
                    }
                }
            }
            if(!collidedY){
                  ((Entity *)(*it))->hitGround = false;   
            }
            
            if(!collidedX)
                ((Entity *)(*it))->x += ((Entity *)(*it))->vx;
            
            if(!collidedY)
                ((Entity *)(*it))->y += ((Entity *)(*it))->vy;
            
            NEXT_ITERATION:
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
        mainWindow->setXY(nx,ny);
        
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
