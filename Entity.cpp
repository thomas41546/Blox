#include "Globals.hpp"
#include "Entity.hpp"

Entity::Entity(SDL_Rect _dimensions){
    x = (double)_dimensions.x;
    y = (double)_dimensions.y;
    width = _dimensions.w;
    height = _dimensions.h;
    vx = 0;
    vy = 0;
    hitGround = false;
    dead = false;

    static int nextid = 0;
    id = ++nextid;
};

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


int Entity::getID()
{
    return id;
}

Point2d<float> Entity::getPosition()
{
    return Point2d<float>(x,y);
}

Point2d<float> Entity::getSize(){
    return Point2d<float>(width,height); //TODO verify this is correct
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

//WormEntity
WormEntity::WormEntity(SDL_Rect _dimensions, std::vector<Entity *> & entities) : NPCEntity(_dimensions){
    for(int i = 0; i < WORM_LENGTH; i++){
        segments[i] = new WormBodyEntity(_dimensions);
        segments[i]->x = _dimensions.x - (_dimensions.w+5)*i;
        segments[i]->y = _dimensions.y;
        segments[i]->vx = 0;
        segments[i]->vy = 0;
        entities.push_back(segments[i]);
    }
}

 void WormEntity::applyAI(std::vector<Entity *> & entities, CellMatrix & cells){
    //static double curAngle = 0;
    static double lastTargetAngle = 0;
    //static double incrementer = 0.1;
     
    if(rand()% 2 == 0)
        lastTargetAngle += 0.03;
     
    //lastTargetAngle = atan2 (playerEntity->y - y ,playerEntity->x - x);
     
    vx = cos(lastTargetAngle)*10;
    vy = sin(lastTargetAngle)*10;
    
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
BulletEntity::BulletEntity(SDL_Rect _dimensions) : Entity(_dimensions){
    lifetime = BULLET_LIFE; //x * fps
}

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
