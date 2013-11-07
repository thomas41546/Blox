//
//  Drawing.h
//  
//
//  Created by Thomas Polasek on 11/6/13.
//
//

#ifndef _Drawing_h
#define _Drawing_h

#include "SDL/SDL_opengl.h"
#include <SDL/SDL_image.h> 


class Drawing {

public:
    static void drawRect(SDL_Rect & rect, const SDL_Color & color){
        glColor3f(color.r/255.0f,color.g/255.0f,color.b/255.0f);
        glTexCoord2i(0,0); glVertex2i(rect.x, rect.y);
        glTexCoord2i(1,0); glVertex2i(rect.x, rect.y + rect.h);
        glTexCoord2i(1,1); glVertex2i(rect.x + rect.w, rect.y + rect.h);
        glTexCoord2i(0,1); glVertex2i(rect.x + rect.w, rect.y);
    }
    
    static void drawRightTriangleRight(SDL_Rect & rect,  const SDL_Color & color){
        glEnd();
        glBegin(GL_TRIANGLES);
        glColor3f(color.r/255.0f,color.g/255.0f,color.b/255.0f);
        glTexCoord2i(0,0); glVertex2i(rect.x + rect.w, rect.y);
        glTexCoord2i(1,0); glVertex2i(rect.x, rect.y + rect.h);
        glTexCoord2i(0,1); glVertex2i(rect.x + rect.w, rect.y + rect.h);
        glEnd();
        glBegin(GL_QUADS);
    }
    static void drawRightTriangleLeft(SDL_Rect & rect,  const SDL_Color & color){
        glEnd();
        glBegin(GL_TRIANGLES);
        glColor3f(color.r/255.0f,color.g/255.0f,color.b/255.0f);
        glTexCoord2i(0,0); glVertex2i(rect.x, rect.y);
        glTexCoord2i(1,0); glVertex2i(rect.x, rect.y + rect.h);
        glTexCoord2i(0,1); glVertex2i(rect.x + rect.w, rect.y + rect.h);
        glEnd();
        glBegin(GL_QUADS);
    }

    
    static GLuint loadTexture(const char * file_name)
    {
        SDL_Surface* surface = IMG_Load(file_name);
        if (surface==NULL) { //If it failed, say why and don't continue loading the texture
            printf("Error: \"%s\"\n",SDL_GetError());
        }
        
        // Generate the OpenGL texture object
        GLuint texture;
        glGenTextures(1,&texture);
        glBindTexture(GL_TEXTURE_2D,texture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w,surface->h, 0, GL_RGB,GL_UNSIGNED_BYTE,surface->pixels);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        
        SDL_FreeSurface(surface);
        return texture;
    }
};


#endif
