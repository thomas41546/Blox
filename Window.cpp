#include "Globals.hpp"
#include "CellMatrix.hpp"
#include "Entity.hpp"
#include "Window.hpp"
#include "Drawing.hpp"
#include "ShaderLoader.hpp"

Window::Window (double _x,double _y, int _width, int _height, int _maxScrollWidth, int _maxScrollHeight){
    x = _x;
    y = _y;
    zoom = 4;
    width = _width;
    height = _height;
    maxScrollWidth = _maxScrollWidth;
    maxScrollHeight = _maxScrollHeight;
    
    SDL_Init( SDL_INIT_EVERYTHING );
    
    surface = SDL_SetVideoMode( width, height, 24, SDL_HWSURFACE|SDL_DOUBLEBUF| SDL_OPENGL  );

    assert(TTF_Init() != -1);
    font = TTF_OpenFont( "FreeMonoBold.ttf", 25 );
    assert(font != NULL);
    
    setupOpenGL();
    
};

void Window::setupOpenGL(){
    //Load Textures
    textures["block"] = Drawing::loadTexture("dirty1.png");
    
    // Opengl Defaults for 2D
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glOrtho(0.f, ((float)width)/zoom, ((float)height)/zoom, 0.f, 1.f, 1000.f);

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glTranslatef(0.f, 0.f, -500.f);
    glClearColor( 0.f,1.f,0.f, 1.f );
    
    //Setup shaders
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
    //prog(); TODO fix fragment_shader
    
    GLenum error = glGetError();
    assert(error == GL_NO_ERROR);
}


double Window::getX () {return x;}
double Window::getY () {return y;}
unsigned int Window::getZoom () {return zoom;}

unsigned int Window::getWidth () {return width;}
unsigned int Window::getHeight () {return height;}

SDL_Surface* Window::getSurface () {return surface;}


Double_Rect Window::getRect(){
    Double_Rect rect = {x,y,width,height};
    return rect;
}

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

void Window::setXY(double _x, double  _y){
    //TODO fix jitter
    x = _x;
    y = _y;
    //x = round(_x/CellMatrix::getCellSize())*CellMatrix::getCellSize();
    //y = round(_y/CellMatrix::getCellSize())*CellMatrix::getCellSize();
};

void Window::renderStart(){
    
    glClear( GL_COLOR_BUFFER_BIT );
    glDisable(GL_TEXTURE_2D);
    glBegin( GL_QUADS );
    Double_Rect rect = getRect();
    rect.x = 0;
    rect.y = 0;
    Drawing::drawRect(rect,COLOR_LOUNGE);
    glEnd();

    
    glEnable( GL_TEXTURE_2D );
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,GL_MODULATE);//GL_MODULATE
    glBindTexture( GL_TEXTURE_2D, textures["block"] );
    glBegin( GL_QUADS );
    
};

void Window::renderCells(CellMatrix & cells){
    
    
    unsigned int cellSize = CellMatrix::getCellSize();
    
    unsigned int i,j;
    
    double start_i = getX();
    double start_j = getY();
    
    double end_i = getX() + getWidth();
    double end_j = getY() + getHeight();
    
    for(j = start_j; j < end_j; j+= cellSize){
        for(i = start_i; i < end_i; i+=cellSize){
            
            Double_Rect rect = {(i - start_i),
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
