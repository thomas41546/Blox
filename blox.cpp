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

#include <stdlib.h>
#include <stdio.h>
#include <time.h>


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
const int MAIN_LOOP_DELAY =   10;

typedef struct {
	unsigned int x;
	unsigned int y;
    
    unsigned int width;
	unsigned int height;
} Window;

typedef struct {
	unsigned int is_frozen;
	unsigned int is_herd;
} Cell;

typedef struct {
	unsigned int width,height;
	Cell * array;
} Cells;

typedef struct {
    unsigned int width, height;
	SDL_Surface* surface;
} Screen;

static Screen screen;
static Cells cells;
static Window mainWindow= {0, 0, 100, 100};

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
	
	int i,j;
	
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
	int i,j;

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
