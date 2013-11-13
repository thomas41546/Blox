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
    static void drawRect(Double_Rect & rect, const SDL_Color & color){
        glColor3f(color.r/255.0f,color.g/255.0f,color.b/255.0f);
        glTexCoord2i(0,0); glVertex2i(rect.x, rect.y);
        glTexCoord2i(1,0); glVertex2i(rect.x, rect.y + rect.h);
        glTexCoord2i(1,1); glVertex2i(rect.x + rect.w, rect.y + rect.h);
        glTexCoord2i(0,1); glVertex2i(rect.x + rect.w, rect.y);
    }
    
    static void drawRightTriangleRight(Double_Rect & rect,  const SDL_Color & color){
        glEnd();
        glBegin(GL_TRIANGLES);
        glColor3f(color.r/255.0f,color.g/255.0f,color.b/255.0f);
        glTexCoord2i(0,0); glVertex2i(rect.x + rect.w, rect.y);
        glTexCoord2i(1,0); glVertex2i(rect.x, rect.y + rect.h);
        glTexCoord2i(0,1); glVertex2i(rect.x + rect.w, rect.y + rect.h);
        glEnd();
        glBegin(GL_QUADS);
    }
    static void drawRightTriangleLeft(Double_Rect & rect,  const SDL_Color & color){
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
        GLuint texture;
        
        SDL_Surface* Surface = IMG_Load(file_name);
        
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
        return texture;
    }
};


#endif
