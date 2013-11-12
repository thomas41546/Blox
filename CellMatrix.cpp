#include "CellMatrix.hpp"
#include "Perlin.hpp"

CellMatrix::CellMatrix(unsigned int _width, unsigned int _height){
    width = _width;
    height = _height;
    matrix = new Cell[width*height]();
    
    Perlin noiseGen(2,50,9,0);
    
    unsigned x,y;
    for(y = 0; y < height; y++){
        for(x = 0; x < width; x++){
            (getCellIndex(x,y))->setCellMatrix(this);
            
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
                    (getCellIndex(x,y))->resetFilled();
                    break;
                    
                default:
                    colorChoice = colorGrad[groundType - 3];
                    SDL_Color color;
                    
                    color.r = colorChoice >> 16;
                    color.g =  (colorChoice >> 8) & 0xFF;
                    color.b = colorChoice & 0xFF;
                    (getCellIndex(x,y))->setBaseColor(color);
                    
                    (getCellIndex(x,y))->setFilled();
                    break;
                    
            }
            
        }
    }
    for(y = 40; y < 60; y++){
        for(x = 40; x < 60; x++){
            (getCellIndex(x,y))->resetFilled();
        }
    }
    
    
    unsigned i,j;
    for(j = 1; j < height - 1; j++){
        for(i = 1; i < width - 1; i++){
            calcSlopes(i,j);
            calcEdges(i,j);
        }
    }
    
}

void CellMatrix::calcEdges(int i, int j){
    if(i <= 1 || i >= (int)width -1)return;
    if(j <= 1 || j >= (int)height -1)return;
    
    
    (getCellIndex(i,j))->resetEdge();
    if(!(getCellIndex(i,j))->isFilled()){
        for(int oy = -1; oy <= 1; oy++){
            for(int ox = -1; ox <= 1; ox++){
                if(ox == 0 && oy == 0) continue;
                if((getCellIndex(i+ox,j+oy))->isFilled()){
                    (getCellIndex(i,j))->setEdge();
                    return;
                }
            }
        }
    }
}

void CellMatrix::calcSlopes(int i, int j){
    if(i <= 1 || i >= (int)width -1)return;
    if(j <= 1 || j >= (int)height -1)return;
    
    (getCellIndex(i,j))->resetSlope();
    if(!(getCellIndex(i,j))->isFilled()){
        if((getCellIndex(i,j+1))->isFilled() &&
           (getCellIndex(i-1,j))->isFilled() &&
           !(getCellIndex(i+1,j-1))->isFilled() &&
           !(getCellIndex(i+1,j))->isFilled() &&
           !(getCellIndex(i,j-1))->isFilled()){
            (getCellIndex(i,j))->setSlope(Cell::LEFT);
        }
        else if((getCellIndex(i+1,j))->isFilled() &&
                (getCellIndex(i,j+1))->isFilled() &&
                !(getCellIndex(i-1,j))->isFilled() &&
                !(getCellIndex(i-1,j-1))->isFilled() &&
                !(getCellIndex(i,j-1))->isFilled()){
            (getCellIndex(i,j))->setSlope(Cell::RIGHT);
        }
    }
}

unsigned int CellMatrix::getWidth () {return width;}
unsigned int CellMatrix::getHeight () {return height;}


bool CellMatrix::destroyCellByPixel(int i, int j){
    
    Cell * cell = getCellByPixel(i,j);
    
    if(cell->destroy()){
        for(int ix = -1; ix <= 1; ix++)
            for(int iy = -1; iy <= 1; iy++){
                calcEdges(i/CellMatrix::getCellSize() + ix, j/CellMatrix::getCellSize() + iy);
                calcSlopes(i/CellMatrix::getCellSize() + ix, j/CellMatrix::getCellSize() + iy);
            }
        
        return true;
    }
    return false;
}

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
