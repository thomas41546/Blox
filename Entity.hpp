#ifndef __ENTITY_H_INCLUDED__   
#define __ENTITY_H_INCLUDED__   

#include "CellMatrix.hpp"
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
    enum EntityType { NPC,NPC_WORM,NPC_LINEY,PLAYER,BULLET,DEFAULT};
    
    //TODO encapsulate
    unsigned int width,height;
    double vx,vy;
    double x,y;
    int hitGround;
    int id;
    CellMatrix * cellMatrix;
    
    static bool isEntityDead(Entity * e){
        return e->isDead();
    }
    
    Entity(SDL_Rect _dimensions,CellMatrix * _cellMatrix);

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
    virtual void applyAI(std::vector<Entity *> & entities);
    
    virtual void collidedWith(Entity * other);
    
};

class PlayerEntity: public Entity{
public:
    PlayerEntity(SDL_Rect _dimensions, CellMatrix * _cellMatrix) : Entity(_dimensions, _cellMatrix){}
    virtual EntityType getType();
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY);
};


class NPCEntity: public Entity{
public:
    NPCEntity(SDL_Rect _dimensions, CellMatrix * _cellMatrix) : Entity(_dimensions, _cellMatrix){}
    virtual EntityType getType();
    virtual void applyAI(std::vector<Entity *> & entities);
    virtual void collidedWith(Entity * other);
};

class LineyEntity: public NPCEntity{
public:
    LineyEntity(SDL_Rect _dimensions,  CellMatrix * _cellMatrix);
    virtual EntityType getType();
    virtual void applyGravity(float mag);
    virtual void applyHorizontalDrag(float mag);
    virtual void applyAI(std::vector<Entity *> & entities);
};

class WormBodyEntity: public NPCEntity{
public:
    WormBodyEntity(SDL_Rect _dimensions, CellMatrix * _cellMatrix) : NPCEntity(_dimensions, _cellMatrix){}
    virtual EntityType getType();
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY);
    virtual void applyGravity(float mag);
    virtual void applyHorizontalDrag(float mag);
    virtual void applyAI(std::vector<Entity *> & entities);
};

class WormEntity: public NPCEntity{
private:
    static const int WORM_LENGTH = 20;
    static const int WORM_SPACING_DIVIDER = 5;
    WormBodyEntity * segments[WORM_LENGTH];

public:
    WormEntity(SDL_Rect _dimensions,  CellMatrix * _cellMatrix, std::vector<Entity *> & entities);

    virtual EntityType getType();
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY);
    virtual void applyGravity(float mag);
    virtual void applyHorizontalDrag(float mag);
    virtual void applyAI(std::vector<Entity *> & entities);
    
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
    BulletEntity(SDL_Rect _dimensions,  CellMatrix * _cellMatrix);
    virtual EntityType getType();
    virtual void render(SDL_Surface* surface, int offsetX, int offsetY);
    virtual void applyGravity(float mag);
    virtual void applyHorizontalDrag(float mag);
    virtual void applyAI(std::vector<Entity *> & entities);
};


#endif