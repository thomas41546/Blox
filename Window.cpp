#include "Globals.hpp"
#include "CellMatrix.hpp"
#include "Entity.hpp"
#include "Window.hpp"
#include "Drawing.hpp"
#include "ShaderLoader.hpp"
#include "SDL/SDL_ttf.h"
#include "SDL/SDL_opengl.h"

Window::Window (int _x,int _y, int _width, int _height, int _maxScrollWidth, int _maxScrollHeight){
    x = _x;
    y = _y;
    width = _width;
    height = _height;
    maxScrollWidth = _maxScrollWidth;
    maxScrollHeight = _maxScrollHeight;
    
    SDL_Init( SDL_INIT_EVERYTHING );
    
    surface = SDL_SetVideoMode( width, height, 24, SDL_HWSURFACE|SDL_DOUBLEBUF| SDL_OPENGL  );

    assert(TTF_Init() != -1);
    font = TTF_OpenFont( "FreeMonoBold.ttf", 25 );
    assert(font != NULL);
    
    //Initialize Projection Matrix
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0.f, width, height, 0.f, 1.f, 1000.f);
    
    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -500.f);
    
    //Initialize clear color
    glClearColor( 1.f, 1.f, 1.f, 1.f );
    

    const GLchar *vertex_shader[] ={
        "void main(void) {\n",
        "    gl_Position = ftransform();\n",
        "    gl_FrontColor = gl_Color;\n",
        "}"};
    
    const GLchar *color_shader[] = {
        "void main() {\n",
        "   gl_FragColor = gl_Color;\n",
        "}"
    };
    static ShaderLoader prog(vertex_shader, color_shader);
    prog();
    
    //Check for error
    GLenum error = glGetError();
    assert(error == GL_NO_ERROR);
};


unsigned int Window::getX () {return x;}
unsigned int Window::getY () {return y;}

unsigned int Window::getWidth () {return width;}
unsigned int Window::getHeight () {return height;}

SDL_Surface* Window::getSurface () {return surface;}

void  Window::scrollHorizonally(int amount) {
    if(x + amount >= maxScrollWidth) {
        if(amount <= 0)
            x = 0;
        else
            x = maxScrollWidth-1;
    }
    else
        x += amount;
};

void  Window::scrollVertically(int amount) {
    if(y + amount >= maxScrollHeight){
        if(amount <= 0)
            y = 0;
        else
            y = maxScrollHeight-1;
    }
    else
        y += amount;
};

void Window::setXY(unsigned int _x, unsigned int _y){
    x = _x;
    y = _y;
};

void Window::renderStart(){
    //blank
    //SDL_Rect rect = {0,0,getWidth(), getHeight()};
    //SDL_FillRect(getSurface(), &rect, COLOR_WHITE);
    
    glClear( GL_COLOR_BUFFER_BIT );
};

void Window::renderFont(int ox, int oy, std::string text){
    
    
    SDL_Color statColor = {0,255,0,255};
    SDL_Color blackColor = {0,0,0,255};
    SDL_Surface* message = TTF_RenderText_Shaded(font, text.c_str(), statColor,blackColor );
    assert(message != NULL);
    
    SDL_Rect offset = {ox,oy,0,0};
    
    SDL_BlitSurface( message, NULL, getSurface(), &offset );
    
    SDL_FreeSurface(message);
};

void Window::renderCells(CellMatrix & cells){
    
    
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
            
            if( cells.getCellByPixel(i,j) != NULL && cells.getCellByPixel(i,j)->is_slope){
                Drawing::drawRect(rect,COLOR_BLUE);
                continue;
            }
            
            if(cells.getCellByPixel(i,j) != NULL && (cells.getCellByPixel(i,j))->is_frozen){
                Drawing::drawRect(rect,COLOR_BLACK);
                rect.x += 1;
                rect.y += 1;
                rect.w -= 2;
                rect.h -= 2;
                if(cells.getCellByPixel(i,j)->is_hit > 0){
                    Drawing::drawRect(rect,COLOR_GREEN);
                }else{
                    Drawing::drawRect(rect,COLOR_WHITE);
                }
                
            }
        }
    }
};

void Window::renderEntities(std::vector<Entity *> entities){
    for (std::vector<Entity *>::iterator it = entities.begin() ; it != entities.end(); ++it){
        ((Entity *)(*it))->render(getSurface(),getX(),getY());
    }
};

void Window::renderFinish(){
   glFlush();
   SDL_GL_SwapBuffers();
};
