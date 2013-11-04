/*
 Game: Pixie
 Author: Thomas Polasek
 Date: October 22nd, 2011
 Licence: None. Use as you wish. However, some credit would be nice.
 */

#include "SDL/SDL.h"

#include "Globals.h"
#include "Entity.h"
#include "CellMatrix.h"
#include "Window.h"
#include "QuadTree.hpp"

#include <string>
#include <iostream>
#include <boost/foreach.hpp>
#include <vector>
#include <math.h>

#include <stdlib.h>
#include <stdio.h>
#include <time.h>

PlayerEntity * playerEntity;


int collisionDetectRIR(SDL_Rect box1, SDL_Rect box2)
{
    if ((box2.x>box1.x)&&((box1.x+box1.w)>box2.x))
        if ((box2.y>box1.y)&&((box1.y+box1.h)>box2.y)) return 1;
    
    return 0;
}

void exit_Game(){
	SDL_Quit();
	exit(0);
}


int main( int argc, char* args[] ){
    static CellMatrix cells(CELL_WIDTH,CELL_HEIGHT);
    static Window * mainWindow;
    
    
    // TODO abstract out to EntityManager
    static std::vector<Entity *> entities;
    
    QuadTree * qt = new QuadTree(Point2d<float>(500,500), Point2d<float>(1000,1000), 0, 5, 100000, NULL);
    
    
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
