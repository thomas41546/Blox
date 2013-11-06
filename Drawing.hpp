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

class Drawing {

public:
    static void drawRect(SDL_Rect & rect, SDL_Color color){
        glBegin( GL_QUADS );
        glColor3f(color.r/255.0f,color.g/255.0f,color.b/255.0f);
        glVertex2f(rect.x, rect.y);
        glVertex2f(rect.x, rect.y + rect.h);
        glVertex2f(rect.x + rect.w, rect.y + rect.h);
        glVertex2f(rect.x + rect.w, rect.y);
        glEnd();
    }
};


#endif
