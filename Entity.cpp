#include "Globals.h"
#include "Entity.h"

SDL_Rect Entity::getRect(){
    SDL_Rect rect = {(unsigned int)x, (unsigned int)y, width,height};
    return rect;
}

void Entity::setDead(){
    dead = true;
}
bool Entity::isDead(){
    return dead;
}

 Entity::EntityType Entity::getType(){
    return DEFAULT;
}

 void Entity::render(SDL_Surface* surface, int offsetX, int offsetY){
    SDL_Rect rect = getRect();
    rect.x -= offsetX;
    rect.y -= offsetY;
    SDL_FillRect(surface, &rect, COLOR_BLACK);
}

 void Entity::applyGravity(float mag){
    vy += mag;
}

 void Entity::applyHorizontalDrag(float mag){
    vx *= mag;
}

 void Entity::applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
}



// PlayerEntity

 void PlayerEntity::render(SDL_Surface* surface, int offsetX, int offsetY){
    SDL_Rect rect = getRect();
    rect.x -= offsetX;
    rect.y -= offsetY;
    SDL_FillRect(surface, &rect, COLOR_RED);
}
 Entity::EntityType PlayerEntity::getType(){
    return PLAYER;
}
extern  PlayerEntity * playerEntity;

//NPCEntity

 void NPCEntity::applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
    if(abs(vy) < 0.01 &&  abs(vx) < 0.01 && hitGround){
        vy -= rand()% 6 + 1;
        if(playerEntity->x < x)
            vx -= rand()% 5 + 3;
        else if(playerEntity->x > x)
            vx += rand()% 5 + 3;
    }
}
 Entity::EntityType NPCEntity::getType(){
    return NPC;
}

//WormBodyEntity


 void WormBodyEntity::applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
}

 void WormBodyEntity::applyGravity(float mag){
}

 void WormBodyEntity::applyHorizontalDrag(float mag){
}

 void WormBodyEntity::render(SDL_Surface* surface, int offsetX, int offsetY){
    SDL_Rect rect = getRect();
    rect.x -= offsetX;
    rect.y -= offsetY;
    SDL_FillRect(surface, &rect, COLOR_BLUE);
}

 Entity::EntityType WormBodyEntity::getType(){
    return NPC_WORM;
}


 void WormEntity::applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
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


 void WormEntity::applyGravity(float mag){
}

 void WormEntity::applyHorizontalDrag(float mag){
}

 void WormEntity::render(SDL_Surface* surface, int offsetX, int offsetY){
    SDL_Rect rect = getRect();
    rect.x -= offsetX;
    rect.y -= offsetY;
    SDL_FillRect(surface, &rect, COLOR_BLUE);
}

 Entity::EntityType WormEntity::getType(){
    return NPC_WORM;
}



//BulletEntity


 void BulletEntity::render(SDL_Surface* surface, int offsetX, int offsetY){
    SDL_Rect rect = getRect();
    rect.x -= offsetX;
    rect.y -= offsetY;
    SDL_FillRect(surface, &rect, COLOR_GREEN);
}

 void BulletEntity::applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
    lifetime -= 1;
    if(lifetime == 0){
        setDead();
    }
}


 void BulletEntity::applyGravity(float mag){
}

 void BulletEntity::applyHorizontalDrag(float mag){
}

 Entity::EntityType BulletEntity::getType(){
    return BULLET;
}
