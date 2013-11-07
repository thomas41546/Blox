

#include <SDL/SDL.h>
#include <SDL/SDL_image.h> //Needed for IMG_Load.  If you want to use bitmaps (SDL_LoadBMP), it appears to not be necessary
#include <SDL/SDL_opengl.h>


const int screen_size[2] = {800,600};

GLuint texture;
void init_texture(void) {
    //Load the image from the file into SDL's surface representation
    SDL_Surface* surface = IMG_Load("checkers.png");
    if (surface==NULL) { //If it failed, say why and don't continue loading the texture
        printf("Error: \"%s\"\n",SDL_GetError()); return;
    }
    
    //Generate an array of textures.  We only want one texture (one element array), so trick
    //it by treating "texture" as array of length one.
    glGenTextures(1,&texture);
    //Select (bind) the texture we just generated as the current 2D texture OpenGL is using/modifying.
    //All subsequent changes to OpenGL's texturing state for 2D textures will affect this texture.
    glBindTexture(GL_TEXTURE_2D,texture);
    //Specify the texture's data.  This function is a bit tricky, and it's hard to find helpful documentation.  A summary:
    //   GL_TEXTURE_2D:    The currently bound 2D texture (i.e. the one we just made)
    //               0:    The mipmap level.  0, since we want to update the base level mipmap image (i.e., the image itself,
    //                         not cached smaller copies)
    //         GL_RGBA:    The internal format of the texture.  This is how OpenGL will store the texture internally (kinda)--
    //                         it's essentially the texture's type.
    //      surface->w:    The width of the texture
    //      surface->h:    The height of the texture
    //               0:    The border.  Don't worry about this if you're just starting.
    //          GL_RGB:    The format that the *data* is in--NOT the texture!  Our test image doesn't have an alpha channel,
    //                         so this must be RGB.
    //GL_UNSIGNED_BYTE:    The type the data is in.  In SDL, the data is stored as an array of bytes, with each channel
    //                         getting one byte.  This is fairly typical--it means that the image can store, for each channel,
    //                         any value that fits in one byte (so 0 through 255).  These values are to interpreted as
    //                         *unsigned* values (since 0x00 should be dark and 0xFF shold be bright).
    // surface->pixels:    The actual data.  As above, SDL's array of bytes.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_RGB,GL_UNSIGNED_BYTE,surface->pixels);
    //Set the minification and magnification filters.  In this case, when the texture is minified (i.e., the texture's pixels (texels) are
    //*smaller* than the screen pixels you're seeing them on, linearly filter them (i.e. blend them together).  This blends four texels for
    //each sample--which is not very much.  Mipmapping can give better results.  Find a texturing tutorial that discusses these issues
    //further.  Conversely, when the texture is magnified (i.e., the texture's texels are *larger* than the screen pixels you're seeing
    //them on), linearly filter them.  Qualitatively, this causes "blown up" (overmagnified) textures to look blurry instead of blocky.
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    
    //Unload SDL's copy of the data; we don't need it anymore because OpenGL now stores it in the texture.
    SDL_FreeSurface(surface);
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
    
    SDL_SetVideoMode(screen_size[0],screen_size[1], 24, SDL_HWSURFACE|SDL_DOUBLEBUF|SDL_OPENGL);
    
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
