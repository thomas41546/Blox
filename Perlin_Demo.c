#include "Perlin.hpp"
#include <stdio.h>
#include <SDL/SDL.h>
#include <math.h>

void DrawPixel(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B);
SDL_Surface *Render_Noise(Perlin & noiseGen, int w, int h, int r, int g, int b);

int main(int argc, char **argv)
{
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_Surface *screen = SDL_SetVideoMode(1000,1000,24,SDL_SWSURFACE);
    
    
    //SDL_SaveBMP(noise, "noise.bmp");
    
    int i = 0;
    int octaves = 8;
    float freq = 8;
    float amp = 1;
    
    while(true)
    {
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            switch (event.type) {
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_q){
                        SDL_Quit();
                        return 0;
                    }
                    else if(event.key.keysym.sym == SDLK_LEFT)
                        freq -= 0.1;
                    else if(event.key.keysym.sym == SDLK_RIGHT)
                        freq += 0.1;
                    else if(event.key.keysym.sym == SDLK_UP)
                        amp += 0.02;
                    else if(event.key.keysym.sym == SDLK_UP)
                        amp -= 0.02;
                    break;
            }
        }
        
        printf("octave/%d freq/%2.4f amp/%2.4f\n",octaves,freq,amp);
        srand(0);
        Perlin noiseGen(octaves,freq,amp,0);
        SDL_Surface *noise = Render_Noise(noiseGen,1000,1000,255,255,255);
        SDL_BlitSurface(noise,NULL,screen,NULL);
        SDL_Flip(screen);
        SDL_FreeSurface(noise);
        SDL_Delay(100);
        

    }
    
    SDL_Quit();
    
    return 0;
}

void DrawPixel(SDL_Surface *screen, int x, int y, Uint8 R, Uint8 G, Uint8 B)
{
    Uint32 color = SDL_MapRGB(screen->format, R, G, B);
    
    if(SDL_MUSTLOCK(screen))
    {
        if(SDL_LockSurface(screen) < 0)
        {
            return;
        }
    }
    
    switch(screen->format->BytesPerPixel)
    {
        case 1:
        {
            Uint8 *bufp;
            
            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *bufp = color;
        }
            break;
            
        case 2:
        {
            Uint16 *bufp;
            bufp = (Uint16 *)screen->pixels + y*screen->pitch/2 + x;
            *bufp = color;
        }
            break;
            
        case 3:
        {
            Uint8 *bufp;
            
            bufp = (Uint8 *)screen->pixels + y*screen->pitch + x;
            *(bufp+screen->format->Rshift/8) = R;
            *(bufp+screen->format->Bshift/8) = B;
            *(bufp+screen->format->Gshift/8) = G;
        }
            break;
            
        case 4:
        {
            Uint32 *bufp;
            
            bufp = (Uint32 *)screen->pixels + y*screen->pitch/4 + x;
            *bufp = color;
        }
            break;
    }
    
    if(SDL_MUSTLOCK(screen))
    {
        SDL_UnlockSurface(screen);
    }
    SDL_UpdateRect(screen, x, y, 1, 1);
}

const static int colors[] = {
    0xFFFFFF,
    0xFFFFFF,
    0x000000,
    0x000000,
    0x000000
};


SDL_Surface *Render_Noise(Perlin & noiseGen, int w, int h, int r, int g, int b)
{
    SDL_Surface *ret = SDL_CreateRGBSurface(SDL_SWSURFACE,w,h,32,0,0,0,0);          //create an empty image
    
    for(int y = 0; y < h; y++)
    {
        for(int x = 0; x < w; x++)
        {
            float getnoise = noiseGen.Get((float)x/(float)w,(float)y/(float)h) ;
            if(getnoise > 0.8) getnoise = 0.8;
            if(getnoise < -0.8) getnoise = -0.8;
            getnoise *= 1.25; // ranges from [-1,1]
            getnoise = (getnoise*0.5 + 0.5); // [0,1]
            if(getnoise < 0)getnoise = 0;
            if(getnoise > 1)getnoise = 1;
            
            
            int numColors = sizeof(colors)/sizeof(int) - 1;
            
            int color = colors[(int)round((getnoise * numColors))];
            
            DrawPixel( ret, x, y,color >> 16,  (color >> 8) & 0xFF, color & 0xFF );
        }
    }
    
    return ret;
}