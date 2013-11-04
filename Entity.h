#ifndef __ENTITY_H_INCLUDED__   
#define __ENTITY_H_INCLUDED__   

#include "CellMatrix.h"
#include "Point2d.hpp"

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


class Entity{
protected:
    bool dead;
public:
    enum EntityType { NPC,NPC_WORM,PLAYER,BULLET,DEFAULT};
    
    //TODO encapsulate
    unsigned int width,height;
    double vx,vy;
    double x,y;
    bool hitGround;
    int id;
    
    static bool isEntityDead(Entity * e){
        return e->isDead();
    }
    
    Entity(SDL_Rect _dimensions);

    SDL_Rect getRect();
    void setDead();
    bool isDead();
    
    int getID();
    Point2d<float> getPosition();
    Point2d<float> getSize();
    
    
    virtual EntityType getType();
    
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY);
    
    virtual void applyGravity(float mag);
    
    virtual void applyHorizontalDrag(float mag);
    
    virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells);
    
};

class PlayerEntity: public Entity{
public:
    PlayerEntity(SDL_Rect _dimensions) : Entity(_dimensions){}
    virtual EntityType getType();
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY);
};


class NPCEntity: public Entity{
public:
    NPCEntity(SDL_Rect _dimensions) : Entity(_dimensions){}
    virtual EntityType getType();
    virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells);
    
};

class WormBodyEntity: public NPCEntity{
public:
    WormBodyEntity(SDL_Rect _dimensions) : NPCEntity(_dimensions){
    }
    virtual EntityType getType();
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY);
    virtual void applyGravity(float mag);
    virtual void applyHorizontalDrag(float mag);
    virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells);
};

class WormEntity: public NPCEntity{
private:
    static const int WORM_LENGTH = 20;
    static const int WORM_SPACING_DIVIDER = 5;
    WormBodyEntity * segments[WORM_LENGTH];

public:
    WormEntity(SDL_Rect _dimensions, std::vector<Entity *> & entities);

    virtual EntityType getType();
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY);
    virtual void applyGravity(float mag);
    virtual void applyHorizontalDrag(float mag);
    virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells);
    
    ~WormEntity(){
        for(int i = 0; i < WORM_LENGTH; i++){
            segments[i]->setDead();
        }
    }
};


class BulletEntity: public Entity{
private:
    unsigned int lifetime;
    static const unsigned int BULLET_LIFE = 500;
    
public:
    BulletEntity(SDL_Rect _dimensions);
    virtual EntityType getType();
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY);
    virtual void applyGravity(float mag);
    virtual void applyHorizontalDrag(float mag);
    virtual void applyAI(std::vector<Entity *> & entities, CellMatrix & cells);
};


#endif