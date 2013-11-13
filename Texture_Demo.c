

#include <SDL/SDL.h>
#include <SDL/SDL_image.h> //Needed for IMG_Load.  If you want to use bitmaps (SDL_LoadBMP), it appears to not be necessary
#include <SDL/SDL_opengl.h>

const int screen_size[2] = {800,600};

GLuint texture;
void init_texture(void) {
    
    SDL_Surface* Surface = IMG_Load("texture.png");
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    int Mode = GL_RGB;
    if(Surface->format->BytesPerPixel == 4) {
        Mode = GL_RGBA;
    }
    
    glTexImage2D(GL_TEXTURE_2D, 0, Mode, Surface->w, Surface->h, 0, Mode, GL_UNSIGNED_BYTE, Surface->pixels);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    //Unload SDL's copy of the data; we don't need it anymore because OpenGL now stores it in the texture.
    SDL_FreeSurface(Surface);
}
void deinit_texture(void) {
    //Deallocate an array of textures.  A lot of people forget to do this.
    glDeleteTextures(1,&texture);
}

bool get_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT: return false; //The little X in the window got pressed
            case SDL_KEYDOWN:
                if (event.key.keysym.sym==SDLK_ESCAPE) {
                    return false;
                }
                break;
        }
    }
    return true;
}
void draw(void) {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    
    glViewport(0,0,screen_size[0],screen_size[1]);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)(screen_size[0])/(double)(screen_size[1]), 0.1f,100.0f);
    glMatrixMode(GL_MODELVIEW);
    
    glLoadIdentity();
    gluLookAt(2.0,3.0,4.0, 0.0,0.0,0.0, 0.0,1.0,0.0);
    
    //Set our loaded texture as the current 2D texture (this isn't actually technically necessary since our
    //texture was never unselected from above, but this is most clear)
    glBindTexture(GL_TEXTURE_2D,texture);
    //Tell OpenGL that all subsequent drawing operations should try to use the current 2D texture
    glEnable(GL_TEXTURE_2D);
    
    glBegin(GL_TRIANGLES);
    glTexCoord2f(0.0f,0.0f); //All subsequent vertices will have an associated texture coordinate of (0,0)
    glVertex3f( 0.0f, 0.1f, 0.0f);
    glTexCoord2f(1.0f,0.0f); //All subsequent vertices will have an associated texture coordinate of (1,0)
    glVertex3f(-0.1f,-0.1f, 0.7f);
    glTexCoord2f(0.0f,1.0f); //All subsequent vertices will have an associated texture coordinate of (0,1)
    glVertex3f( 1.0f,-0.2f, 0.0f);
    glEnd();
    
    //Tell OpenGL that all subsequent drawing operations should NOT try to use the current 2D texture
    glDisable(GL_TEXTURE_2D);
    
    glBegin(GL_LINES);
    glColor3f(1.0f,0.0f,0.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(1.0f,0.0,0.0);
    glColor3f(0.0f,1.0f,0.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,1.0,0.0);
    glColor3f(0.0f,0.0f,1.0f); glVertex3f(0.0f,0.0f,0.0f); glVertex3f(0.0f,0.0,1.0);
    glColor3f(1.0f,1.0f,1.0f);
    glEnd();
    
    SDL_GL_SwapBuffers();
}
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_EVERYTHING|SDL_INIT_NOPARACHUTE);
    
    SDL_SetVideoMode(screen_size[0],screen_size[1], 32, SDL_OPENGL);
    
    glEnable(GL_DEPTH_TEST);
    
    init_texture();
    
    while (true) {
        if (!get_input()) break;
        draw();
    }
    
    deinit_texture();
    
    SDL_Quit();
    
    return 0;
}