#include "Globals.hpp"
#include "CellMatrix.hpp"
#include "Entity.hpp"
#include "Window.hpp"
#include "Drawing.hpp"
#include "ShaderLoader.hpp"

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
    
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0.f, width, height, 0.f, 1.f, 1000.f);
    
    //Initialize Modelview Matrix
    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -500.f);
    
    glClearColor( COLOR_LOUNGE.r/255.0, COLOR_LOUNGE.g/255.0,COLOR_LOUNGE.b/255.0, 1.f );
    
    blockTexture = Drawing::loadTexture("dirty1.png");
    
    const GLchar *vertex_shader[] ={
        "void main(void) {\n",
        "    gl_Position = ftransform();\n",
        "gl_FrontColor = gl_Color;\n",
        "    gl_TexCoord[0] =  gl_MultiTexCoord0;\n",
        "   gl_TexCoord[1] = gl_MultiTexCoord1;\n",
        "}"};
    
    const GLchar *fragment_shader[] = {
        "uniform sampler2D myTexture1;",
        "void main() {\n",
        "      vec4 texval1 = texture2D(myTexture1, vec2(gl_TexCoord[0]));",
        "       gl_FragColor = normalize(texval1 + gl_Color);",
        "}"
    };
    static ShaderLoader prog(vertex_shader, fragment_shader);
    //prog();
    //TODO fix fragment_shader
    
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
    glEnable( GL_TEXTURE_2D );
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);//GL_MODULATE
    glBindTexture( GL_TEXTURE_2D, blockTexture );
    glBegin( GL_QUADS );
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
            
            if(cells.getCellByPixel(i,j) != NULL){
                cells.getCellByPixel(i,j)->render(rect);
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
   glEnd();
   glFlush();
   glDisable(GL_TEXTURE_2D);
   SDL_GL_SwapBuffers();
};
