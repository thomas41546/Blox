/*
 Game: Pixie
 Author: Thomas Polasek
 Date: October 22nd, 2011
 Licence: None. Use as you wish. However, some credit would be nice.
 */

#include "Blox.hpp"
#include "Globals.hpp"
#include "Entity.hpp"
#include "CellMatrix.hpp"
#include "Window.hpp"
#include "QuadTree.hpp"

static CellMatrix cells(CELL_WIDTH,CELL_HEIGHT);
static Window * mainWindow;
boost::thread * windowFlipThread;
// TODO abstract out to EntityManager
static std::vector<Entity *> entities;

int collisionDetectRIR(Double_Rect box1, Double_Rect box2)
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
    
    //boost::thread t(boost::bind(&render_flip));
    //windowFlipThread = &t;
    
    SDL_Rect playerRect = {100,100,10,20};
    playerEntity = new PlayerEntity(playerRect, &cells);
    entities.push_back((Entity *)playerEntity);
    
    SDL_Rect wormRect = {500,500,40,40};
    entities.push_back(new WormEntity(wormRect,&cells, entities));
    
    for(int i = 0; i < 20; i++){
        SDL_Rect npcRect = {rand()%200 + 300,rand()%200 + 300,11,11};
        entities.push_back(new NPCEntity(npcRect,&cells));
    }
    
    /*
    for(int i = 0; i < 1000; i++){
        SDL_Rect npcRect = {rand()%200 + 300,rand()%200 + 300,11,11};
        entities.push_back(new NPCEntity(npcRect,&cells));
    }*/
    
    
    SDL_EnableKeyRepeat(1, 5);
    
	int is_game = 1;
    int fps = 0;
    int lastFps = 0;
    unsigned int lastTime = SDL_GetTicks();
    
    bool lastPressLeft = false;
    
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
                    else if(event.key.keysym.sym == SDLK_LEFT){
                            lastPressLeft = true;
                            playerEntity->vx = -2;
                    }
                    else if(event.key.keysym.sym == SDLK_RIGHT){
                            lastPressLeft = false;
                            playerEntity->vx = 2;
                    }
                    
                    else if(event.key.keysym.sym == SDLK_UP){
                        if(playerEntity->hitGround == 4){
                            playerEntity->vy = -3;
                            playerEntity->hitGround = 0;
                        }
                    }
                    else if(event.key.keysym.sym == SDLK_SPACE){
                        
                        float radStart,radEnd;
                        if(lastPressLeft){
                            radStart = 3.00;
                            radEnd = 3.28;
                        }
                        else{
                            radStart = -0.14;
                            radEnd = 0.14;
                        }
                        
                        for(float rad = radStart; rad < radEnd; rad += 0.02){
                            SDL_Rect bulletRect = {playerEntity->x,playerEntity->y,5,5};
                            BulletEntity * bullet =new BulletEntity(bulletRect,&cells);
                            bullet->vx = cos(rad)*9;
                            bullet->vy = sin(rad)*9;
                            entities.push_back(bullet);
                        }
                        
                    }
                    break;
                    
                case SDL_MOUSEBUTTONDOWN:
                    {
                        if(event.button.button == SDL_BUTTON_LEFT)
                        {
                            int mx,my;
                            SDL_GetMouseState(&mx,&my);
                            
                            double angle = atan2 (my - (int)mainWindow->getHeight()/2/mainWindow->getZoom() ,mx - (int)mainWindow->getWidth()/2/mainWindow->getZoom());
                            
                            SDL_Rect bulletRect = {playerEntity->x,playerEntity->y,5,5};
                            BulletEntity * bullet =new BulletEntity(bulletRect,&cells);
                            bullet->vx = cos(angle)*9;
                            bullet->vy = sin(angle)*9;
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
             entity->applyAI(entities);
             
             if(entity->getType() != Entity::PLAYER && entity->getType() != Entity::DEFAULT &&
                collisionDetectRIR(playerEntity->getRect(),entity->getRect())){
                 entity->collidedWith(playerEntity);
             }
         }
        
        //physics
        for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
            Entity * entity =  ((Entity *)(*it));
            
            entity->applyGravity(0.1);
            entity->applyHorizontalDrag(0.95);
            
            if(entity->vy > MAX_VELOCITY) entity->vy = MAX_VELOCITY;
            else if(entity->vy < -1*MAX_VELOCITY) entity->vy = -1*MAX_VELOCITY;
            
            if(entity->vx > MAX_VELOCITY) entity->vx = MAX_VELOCITY;
            else if(entity->vx < -1*MAX_VELOCITY) entity->vx = -1*MAX_VELOCITY;
            
            bool collidedY = false;
            bool collidedX = false;
            int i,j;
            
            if(entity->getType() != Entity::BULLET && entity->getType() != Entity::NPC_WORM){
                
                int startIndexJ = MIN(entity->y,entity->y + entity->vy) / CellMatrix::getCellSize();
                int endIndexJ = MAX(entity->y + entity->height*0.9, entity->y + entity->height*0.9 + entity->vy) / CellMatrix::getCellSize();
                
                int startIndexI = MIN(entity->x,entity->x) / CellMatrix::getCellSize();
                int endIndexI = MAX(entity->x + entity->width*0.9, entity->x + entity->width*0.9) / CellMatrix::getCellSize();
                
                collidedY = false;
                for(j = startIndexJ; j <= endIndexJ; j++){
                    for(i = startIndexI; i <= endIndexI; i++){
                        Cell * cell = cells.getCellIndex(i,j);
                        if(cell->isFilled()){
                            collidedY = true;
                        }
                    }
                }
                if(collidedY){
                    entity->vy *= 0.3;
                    if(++entity->hitGround > 4 && entity->vy > 0) entity->hitGround = 4;
                }
                else{
                     entity->y += entity->vy;
                     entity->hitGround = 0;
                }
                
                //////// 
                
                startIndexJ = MIN(entity->y,entity->y) / CellMatrix::getCellSize();
                endIndexJ = MAX(entity->y + entity->height*0.9, entity->y + entity->height*0.9) / CellMatrix::getCellSize();
                
                startIndexI = MIN(entity->x,entity->x + entity->vx) / CellMatrix::getCellSize();
                endIndexI = MAX(entity->x + entity->width*0.9, entity->x + entity->width*0.9 + entity->vx) / CellMatrix::getCellSize();
                
                collidedX = false;
                for(j = startIndexJ; j <= endIndexJ; j++){
                    for(i = startIndexI; i <= endIndexI; i++){
                        Cell * cell = cells.getCellIndex(i,j);
                        if(cell->isFilled()){
                            collidedX = true;
                        }
                    }
                }
                if(collidedX){
                    entity->vx *= 0.3;
                }
                else{
                    entity->x += entity->vx;
                }
                
                
                //slope stuff
                /*
                int oldY = entity->y;

                startIndexJ = MIN(entity->y,entity->y) / CellMatrix::getCellSize();
                endIndexJ = MAX(entity->y + entity->height*0.9, entity->y + entity->height*0.9) / CellMatrix::getCellSize();
                
                startIndexI = MIN(entity->x,entity->x) / CellMatrix::getCellSize();
                endIndexI = MAX(entity->x + entity->width*0.9, entity->x + entity->width*0.9) / CellMatrix::getCellSize();
                
                for(j = startIndexJ; j <= endIndexJ; j++){
                    for(i = startIndexI; i <= endIndexI; i++){
                        Cell * cell = cells.getCellIndex(i,j);
                        if(cell->getSlope() != Cell::NONE){
                            
                            if(std::abs(entity->vx) > 0.1 &&  entity->vy < 0.1){
                                entity->y -= CellMatrix::getCellSize();
                            }
                            //printf("slope hit x/%2.4f y/%2.4f diff/%2.4f\n",xdiff,ydiff,std::abs(xdiff - ydiff));
 
                            goto END_SLOPE_CALC;
                        }
                    }
                }
                END_SLOPE_CALC:
                
                // check if our slope stuff caused us to collide
                startIndexJ = MIN(entity->y,entity->y) / CellMatrix::getCellSize();
                endIndexJ = MAX(entity->y + entity->height*0.9, entity->y + entity->height*0.9) / CellMatrix::getCellSize();
                for(j = startIndexJ; j <= endIndexJ; j++){
                    for(i = startIndexI; i <= endIndexI; i++){
                         Cell * cell = cells.getCellIndex(i,j);
                        if(cell->isFilled()){
                            entity->y = oldY;
                        }
                    }
                }
                continue;
                */

            }
            else{
                
                Double_Rect entityRect;
                entityRect = entity->getRect();
                entityRect.y += entity->vy;
                entityRect.x += entity->vx;
                
                if(entityRect.x < 0 || entityRect.y < 0){
                    entity->setDead();
                    continue;
                }
                
                for(j = entityRect.y; j <= entityRect.y + entityRect.h; j+= CellMatrix::getCellSize()){
                    for(i = entityRect.x; i <=  entityRect.x + entityRect.w; i+=CellMatrix::getCellSize()){
                        Cell * cell = cells.getCellByPixel(i,j);
                        if( cell!= NULL && cell->isFilled()){
                            
                            if(entity->getType() == Entity::BULLET){ //TODO check for properties in entity instead of enum
                                if(cells.destroyCellByPixel(i,j)){
                                    entity->setDead();
                                    goto NEXT_ENTITY;
                                }
                                else{
                                    continue;
                                }
                            }
                            
                            
                            if(entity->getType() == Entity::NPC_WORM){
                                cells.destroyCellByPixel(i,j);
                                continue;
                            }
                            
                        }
                    }
                }
                entity->x += entity->vx;
                entity->y += entity->vy;
            }
            
            
            NEXT_ENTITY:
            continue;
            //nothing
            
        }
        int nx,ny;
        nx = playerEntity->x - (int)mainWindow->getWidth()/2/mainWindow->getZoom();
        ny = playerEntity->y - (int)mainWindow->getHeight()/2/mainWindow->getZoom();
        if(nx < 0) nx = 0;
        if(ny < 0) ny = 0;
        /*
        if( mainWindow->getX() + mainWindow->getWidth()/mainWindow->getZoom() < playerEntity->x + playerEntity->width + CellMatrix::getCellSize()*6)
            mainWindow->setXY(playerEntity->x + playerEntity->width + CellMatrix::getCellSize()*6 - mainWindow->getWidth()/mainWindow->getZoom(), mainWindow->getY());
        else if(mainWindow->getX() > playerEntity->x  - CellMatrix::getCellSize()*6){
            mainWindow->setXY( playerEntity->x  - CellMatrix::getCellSize()*6, mainWindow->getY());
        }
        
        if( mainWindow->getY() + mainWindow->getHeight()/mainWindow->getZoom() < playerEntity->y + playerEntity->height + CellMatrix::getCellSize()*4)
            mainWindow->setXY(mainWindow->getX(), playerEntity->y + playerEntity->height + CellMatrix::getCellSize()*4 - mainWindow->getHeight()/mainWindow->getZoom() );
        else if( mainWindow->getY() > playerEntity->y - CellMatrix::getCellSize()*4)
            mainWindow->setXY( mainWindow->getX(), playerEntity->y  - CellMatrix::getCellSize()*4);
        */
        mainWindow->setXY(nx,ny);
        
        mainWindow->renderStart();
        mainWindow->renderCells(cells);
        mainWindow->renderEntities(entities);
       // mainWindow->renderFont(0,0,str(boost::format("Fps/%1% Entities/%2% Cpu/%3%") % lastFps % entities.size() % getCpuUsage() ));
        mainWindow->renderFinish(); 
        
        static unsigned int lastRenderTime = 0;
        static unsigned int curRenderTime = 0;
        
        curRenderTime = SDL_GetTicks();
        
        if(curRenderTime - lastRenderTime > 0 && curRenderTime - lastRenderTime  < 1000/60){
            usleep((1000/60 - (curRenderTime - lastRenderTime)));
        }
        
        lastRenderTime = SDL_GetTicks();

	}
    exit_Game();
	return 0;
}
