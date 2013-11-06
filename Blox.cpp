/*
 Game: Pixie
 Author: Thomas Polasek
 Date: October 22nd, 2011
 Licence: None. Use as you wish. However, some credit would be nice.
 */

#include "SDL/SDL.h"

#include "Globals.hpp"
#include "Entity.hpp"
#include "CellMatrix.hpp"
#include "Window.hpp"
#include "QuadTree.hpp"

#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <vector>
#include <math.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

static CellMatrix cells(CELL_WIDTH,CELL_HEIGHT);
static Window * mainWindow;
boost::thread * windowFlipThread;
// TODO abstract out to EntityManager
static std::vector<Entity *> entities;

int collisionDetectRIR(SDL_Rect box1, SDL_Rect box2)
{
    int returnValue = 0;
    if((box2.x>box1.x)&&((box1.x+box1.w)>box2.x))returnValue |= 1;
    if((box2.y>box1.y)&&((box1.y+box1.h)>box2.y))returnValue |= 2;
    return returnValue;
}

int getCpuUsage() {
    std::string cmd = str(boost::format("ps -o pcpu -p %1% | grep -v CPU") % getpid());
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return 0;
    char buffer[128];
    std::string result = "";
    while(!feof(pipe)) {
    	if(fgets(buffer, 128, pipe) != NULL)
    		result += buffer;
    }
    pclose(pipe);
    return atoi(result.c_str());
}


PlayerEntity * playerEntity;

void exit_Game(){
    if(windowFlipThread != NULL)
        windowFlipThread->interrupt();
	SDL_Quit();
	exit(0);
}

void render_flip(){
    while(true){
        mainWindow->renderFinish();
        usleep(16000); //60fps
    }
}

int main( int argc, char* args[] ){
    //QuadTree * qt = new QuadTree(Point2d<float>(500,500), Point2d<float>(1000,1000), 0, 5, 100000, NULL);
    
    
    SDL_WM_SetCaption("Blox", "Blox");
    
    srand ( time(NULL) );
    
    mainWindow = new Window(0,0,SCREEN_WIDTH,SCREEN_HEIGHT,
                            CELL_WIDTH*CellMatrix::getCellSize(),
                            CELL_HEIGHT*CellMatrix::getCellSize()); //TODO fix this
    
    boost::thread t(boost::bind(&render_flip));
    windowFlipThread = &t;
    
    SDL_Rect playerRect = {500,500,20,20};
    playerEntity = new PlayerEntity(playerRect);
    entities.push_back((Entity *)playerEntity);
    
    //SDL_Rect wormRect = {500,500,40,40};
    //entities.push_back(new WormEntity(wormRect,entities));
    /*
    for(int i = 0; i < 100; i++){
        SDL_Rect npcRect = {rand()%200 + 300,rand()%200 + 300,11,11};
        entities.push_back(new NPCEntity(npcRect));
    }*/
    
    
    SDL_EnableKeyRepeat(1, 5);
    
	int is_game = 1;
    int fps = 0;
    int lastFps = 0;
    unsigned int lastTime = SDL_GetTicks();
    
	while(is_game){
        unsigned int curTime = SDL_GetTicks();
        if(curTime - lastTime >= 1000){
            printf("FPS/%d Entities/%ld\n",fps,entities.size());
            lastFps = fps;
            lastTime = curTime;
            fps = 0;
        }else{
            fps += 1;
        }
        
		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_q)
                        is_game = 0;
                    else if(event.key.keysym.sym == SDLK_LEFT)
                            playerEntity->vx -= 0.3;
                    else if(event.key.keysym.sym == SDLK_RIGHT)
                            playerEntity->vx += 0.3;
                    
                    else if(event.key.keysym.sym == SDLK_UP){
                        if( playerEntity->hitGround)
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
             
             if(entity->getType() != Entity::PLAYER && entity->getType() != Entity::DEFAULT &&
                collisionDetectRIR(playerEntity->getRect(),entity->getRect())){
                 entity->collidedWith(playerEntity);
             }
         }
        
        
        //DEBUG TODO remove
        
        for(int j = 0; j <= 2000 ; j+= CellMatrix::getCellSize()){
            for(int i = 0; i <=  2000; i+=CellMatrix::getCellSize()){
                Cell * cell = cells.getCellByPixel(i,j);
                if( cell!= NULL){
                    cell->is_hit -= 1;
                    if(cell->is_hit  < 0) cell->is_hit  =0;
                }
            }
        }
        
        
        //physics
        for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
            Entity * entity =  ((Entity *)(*it));
            
            entity->applyGravity(0.1);
            entity->applyHorizontalDrag(0.9);
            entity->hitGround = 0;
            
            if(entity->vy > MAX_VELOCITY) entity->vy = MAX_VELOCITY;
            else if(entity->vy < -1*MAX_VELOCITY) entity->vy = -1*MAX_VELOCITY;
            
            if(entity->vx > MAX_VELOCITY) entity->vx = MAX_VELOCITY;
            else if(entity->vx < -1*MAX_VELOCITY) entity->vx = -1*MAX_VELOCITY;
                
            SDL_Rect entityRect;
            entityRect = entity->getRect();
            entityRect.y += entity->vy;
            entityRect.x += entity->vx;
            if(entityRect.x < 0)entityRect.x = 0;
            if(entityRect.y < 0)entityRect.y = 0;
            
            bool collidedY = false;
            bool collidedX = false;
            int i,j;
            
            
            for(j = entityRect.y; j <= entityRect.y + entityRect.h; j+= CellMatrix::getCellSize()){
                for(i = entityRect.x; i <=  entityRect.x + entityRect.w; i+=CellMatrix::getCellSize()){
                    Cell * cell = cells.getCellByPixel(i,j);
                    if( cell!= NULL && cell->is_frozen){
                        cell->is_hit = 100;;
                        
                        if(entity->getType() == Entity::BULLET){
                            cells.getCellByPixel(i,j)->is_frozen = 0;
                            //TODO: re-detect slopes perhaps add function to cells to recalculate surrounding cell slope values.
                            entity->setDead();
                            goto NEXT_ENTITY;
                        }
                        
                        if(entity->getType() == Entity::NPC_WORM){
                            cells.getCellByPixel(i,j)->is_frozen = 0;
                            //TODO: re-detect slopes
                            continue;
                        }
                        
                        entity->vx = 0;
                        entity->vy = 0;
                        collidedY = 1;
                        collidedX = 1;
                        
                    }
                }
            }
            
            if(!collidedY){
                entity->y += entity->vy;
                if(entity->y < 0)entity->y = 0;
            }
            
            if(!collidedX){
                entity->x += entity->vx;
                if(entity->x < 0)entity->x = 0;
            }
            
            //if(abs(entity->vy) < 0.01) entity->vy = 0;
            //if(abs(entity->vx) < 0.01) entity->vx = 0;
            
            NEXT_ENTITY:
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
        
        static unsigned int lastRenderTime = 0;
        unsigned int curRenderTime = SDL_GetTicks();
        if(curRenderTime - lastRenderTime >= 1000/60){
            mainWindow->setXY(nx,ny);
            
            mainWindow->lockSurface();
            mainWindow->renderStart();
            mainWindow->renderCells(cells);
            mainWindow->renderEntities(entities);
        
            mainWindow->renderFont(0,0,str(boost::format("Fps/%1% Entities/%2% Cpu/%3%") % lastFps % entities.size() % getCpuUsage() ));
            mainWindow->unlockSurface();
            
            lastRenderTime = SDL_GetTicks();
        }
        else{
            usleep((1000/60 - (curRenderTime - lastRenderTime))*1000);//todo improve this, caps it 50-60 try to make it consitent, corporate mainWIndow draws
        }

	}
    exit_Game();
	return 0;
}
