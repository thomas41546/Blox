#include "CellMatrix.hpp"
#include "Perlin.hpp"

CellMatrix::CellMatrix(unsigned int _width, unsigned int _height){
    width = _width;
    height = _height;
    matrix = new Cell[width*height]();
    /*
    (getCellIndex(1,55))->is_frozen = 1;
    (getCellIndex(41,55))->is_frozen = 1;
    (getCellIndex(42,55))->is_frozen = 1;
     */
    //map generation
    
    Perlin noiseGen(2,50,9,0);
    
    unsigned x,y;
    for(y = 0; y < height; y++){
        for(x = 0; x < width; x++){
            
            float getnoise = noiseGen.Get((float)x/(float)width,(float)y/(float)height) ;
            if(getnoise > 0.8) getnoise = 0.8;
            if(getnoise < -0.8) getnoise = -0.8;
            getnoise *= 1.25; // ranges from [-1,1]
            getnoise = (getnoise*0.5 + 0.5); // [0,1]
            if(getnoise < 0)getnoise = 0;
            if(getnoise > 1)getnoise = 1;
            
            
            int colorGrad[] = {
                0x59360C,
                0x784800,
                0xC38A09,
                0xF8DE7F,
                0xF4ED9F,
            };
            int colorChoice = 0;
            
            int groundType = (int)round(getnoise * 7);
            switch(groundType){
                case 0:
                case 1:
                case 2:
                    (getCellIndex(x,y))->is_frozen = 0; 
                    break;
                    
                default:
                    colorChoice = colorGrad[groundType - 3];
                    (getCellIndex(x,y))->color.r = colorChoice >> 16;
                    (getCellIndex(x,y))->color.g =  (colorChoice >> 8) & 0xFF;
                    (getCellIndex(x,y))->color.b = colorChoice & 0xFF;
                    (getCellIndex(x,y))->is_frozen = 1;
                    break;
                    
            }
            
            //(getCellIndex(i,j))->is_frozen = 1; //ALL
            /*
            if(j > 55 && j < 57){
                (getCellIndex(i,j))->is_frozen = 1;
            }
            if(i > 55 && i < 57){
                (getCellIndex(i,j))->is_frozen = 1;
            }            
            
            
             if(rand() % 50 == 0)
             (getCellIndex(i,j))->is_frozen = 1;
             else{
             if(i > 0 && j > 0 && i < width-1 && j < height-1){
             if(getCellIndex(i,j+1)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
             if(getCellIndex(i+1,j)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
             if(getCellIndex(i,j-1)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
             if(getCellIndex(i-1,j)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
             if(getCellIndex(i-1,j-1)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
             if(getCellIndex(i+1,j+1)->is_frozen && rand() % 3 == 0) (getCellIndex(i,j))->is_frozen = 1;
             }
             }
            */
            /*
            if(sqrt((i - 40)*(i -40)+ (j - 40)*(j - 40)) < 40)
                (getCellIndex(i,j))->is_frozen = 0;
            else
                (getCellIndex(i,j))->is_frozen = 1;
            */
            
        }
    }
    
    unsigned i,j;
    for(j = 1; j < height - 1; j++){
        for(i = 1; i < width - 1; i++){
            calcSlopes(i,j);
        }
    }
    
}

void CellMatrix::calcSlopes(int i, int j){
    if(i <= 1 || i >= (int)width -1)return;
    if(j <= 1 || j >= (int)height -1)return;
    
    (getCellIndex(i,j))->is_slope = 0;
    if(!(getCellIndex(i,j))->is_frozen){
        if((getCellIndex(i,j+1))->is_frozen &&
           (getCellIndex(i-1,j))->is_frozen &&
           !(getCellIndex(i+1,j-1))->is_frozen &&
           !(getCellIndex(i+1,j))->is_frozen &&
           !(getCellIndex(i,j-1))->is_frozen){
            (getCellIndex(i,j))->is_slope = 1;
        }
        else if((getCellIndex(i+1,j))->is_frozen &&
                (getCellIndex(i,j+1))->is_frozen &&
                !(getCellIndex(i-1,j))->is_frozen &&
                !(getCellIndex(i-1,j-1))->is_frozen &&
                !(getCellIndex(i,j-1))->is_frozen){
            
            (getCellIndex(i,j))->is_slope = 2;
        }
    }
}

unsigned int CellMatrix::getWidth () {return width;}
unsigned int CellMatrix::getHeight () {return height;}


Cell * CellMatrix::getCellIndex(unsigned int x, unsigned int y){
    if (x < width && y < height){
        return &(matrix[x + y*width]);
    }
    else{
        return NULL;
    }
}

Cell * CellMatrix::getCellByPixel(unsigned int x, unsigned int y){
    x /= CellMatrix::getCellSize();
    y /= CellMatrix::getCellSize();
    return getCellIndex(x,y);
}

SDL_Rect CellMatrix::getCellRectByPixel(unsigned int x, unsigned int y){
    x /= CellMatrix::getCellSize();
    y /= CellMatrix::getCellSize();
    
    SDL_Rect rect = {x*CellMatrix::getCellSize(), y*CellMatrix::getCellSize(),
        CellMatrix::getCellSize(), CellMatrix::getCellSize()};
    return rect;
}
