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


const int CELL_WIDTH =   120;
const int CELL_HEIGHT =   80;
const int SCREEN_WIDTH =   1200;
const int SCREEN_HEIGHT =   800;

const int SCROLL_AMOUNT = 100;

const int MAIN_LOOP_DELAY =   10;


class Cell{
    public:
        unsigned int is_frozen;
        unsigned int is_herd;
};

class CellMatrix {
    private:
        Cell * matrix;
        unsigned int width;
        unsigned int height;
    
    public:
        CellMatrix(unsigned int _width, unsigned int _height){
            width = _width;
            height = _height;
            matrix = new Cell[width*height]();
        }
    
        unsigned int getWidth () {return width;}
        unsigned int getHeight () {return height;}
        
        Cell * getCellIndex(unsigned int x, unsigned int y){
            if (y >= 0 && x < width && y >= 0 && y < height){
                return &(matrix[x + y*width]);
            }
            else{
                return NULL;
            }
        }
};

class Window {
    private:
        unsigned int x;
        unsigned int y;
        
        unsigned int width;
        unsigned int height;
    
        unsigned int maxScrollWidth;
        unsigned int maxScrollHeight;
    
    public:
        Window (int _x,int _y, int _width, int _height, int _maxScrollWidth, int _maxScrollHeight){
            x = _x;
            y = _y;
            width = _width;
            height = _height;
            maxScrollWidth = _maxScrollWidth;
            maxScrollHeight = _maxScrollHeight;
            
        }
    
    unsigned int getX () {return x;}
    unsigned int getY () {return y;}
    
    unsigned int getWidth () {return width;}
    unsigned int getHeight () {return height;}
    
    void scrollHorizonally(int amount) {
        x += amount;
        if(x < 0) x = 0;
        if(x >= maxScrollWidth) x = maxScrollWidth-1;
    }
    
    void scrollVertically(int amount) {
        y += amount;
        if(y < 0) y = 0;
        if(y >= maxScrollHeight) y = maxScrollHeight-1;
    }
};

typedef struct {
    unsigned int width, height;
	SDL_Surface* surface;
} Screen;

static Screen screen;
static CellMatrix cells(CELL_WIDTH,CELL_HEIGHT);
static Window mainWindow(0,0,100,100,3000,3000);


int calculate_Cell_X(int x){
	return  x/(screen.width/cells.width);
}

int calculate_Cell_Y(int y){
	return y/(screen.height/cells.height);
}

void toggle_Cell(int mouse_x, int mouse_y){
	int i = calculate_Cell_X(mouse_x);
	int j = calculate_Cell_Y(mouse_y);
	(cells.array + j*cells.width + i)->is_frozen ^= 1;
}


void render_Screen(){
	SDL_Rect rect = {0,0,screen.width,screen.height};
	SDL_FillRect(screen.surface, &rect, COLOR_WHITE);
	
	
	int cell_pixel_width = screen.width/cells.width;
	int cell_pixel_height = screen.height/cells.height;
	
	unsigned int i,j;
	
	for(j = 0; j < cells.height; j++){
		for(i = 0; i < cells.width; i++){
			
			SDL_Rect rect = {i*cell_pixel_width,j*cell_pixel_height,
							 cell_pixel_width,cell_pixel_height};
			
			if((cells.array + j*cells.width + i)->is_frozen){
                SDL_FillRect(screen.surface, &rect, COLOR_RED);
			}
			else{
				SDL_FillRect(screen.surface, &rect, COLOR_WHITE);
			}
		}
		
	}
	
    SDL_Flip( screen.surface );
}

void exit_Game(){
	free(cells.array);
	SDL_Quit();
	exit(0);
}

int main( int argc, char* args[] ){
	unsigned int i,j;

    std::string hello( "Hello, world!" );
    BOOST_FOREACH( char ch, hello )
    {
        std::cout << ch;
    }
	
	srand ( time(NULL) );
	
    SDL_Init( SDL_INIT_EVERYTHING );

	screen.width = SCREEN_WIDTH;
	screen.height = SCREEN_HEIGHT;
    screen.surface = SDL_SetVideoMode( screen.width, screen.height, 24, SDL_SWSURFACE );
	
	cells.width = CELL_WIDTH;
	cells.height = CELL_HEIGHT;
	cells.array = (Cell *)malloc(sizeof(Cell)*cells.width*cells.height);

	for(j = 0; j < cells.height; j++){
		for(i = 0; i < cells.width; i++){
			(cells.array + j*cells.width + i)->is_frozen = 0;
		}
	}
	
	int is_game = 1;
    
	while(is_game){
		SDL_Event event;
		while ( SDL_PollEvent(&event) ) {
			switch (event.type) {
				case SDL_MOUSEBUTTONDOWN:
					toggle_Cell(event.button.x, event.button.y);
					break;
                
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_LEFT)
                        mainWindow.scrollHorizonally(-1*SCROLL_AMOUNT);
                    else if(event.key.keysym.sym == SDLK_RIGHT)
                        mainWindow.scrollHorizonally(SCROLL_AMOUNT);
                    else if(event.key.keysym.sym == SDLK_UP)
                        mainWindow.scrollVertically(-1*SCROLL_AMOUNT);
                    else if(event.key.keysym.sym == SDLK_DOWN)
                        mainWindow.scrollVertically(SCROLL_AMOUNT);
                    break;
				
				case SDL_QUIT:
					is_game = 0;
					break;
			}
		}
        
		render_Screen();
		SDL_Delay( MAIN_LOOP_DELAY );
	}
	printf("You lost the game.\nNoob!\n");
    exit_Game();
	return 0;
}
