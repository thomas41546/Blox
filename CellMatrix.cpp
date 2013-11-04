#include "CellMatrix.hpp"

CellMatrix::CellMatrix(unsigned int _width, unsigned int _height){
    width = _width;
    height = _height;
    matrix = new Cell[width*height]();
    
    //map generation
    unsigned i,j;
    for(j = 0; j < height; j++){
        for(i = 0; i < width; i++){
            /*
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
             }*/
            
            if(sqrt((i - 40)*(i -40)+ (j - 40)*(j - 40)) < 40)
                (getCellIndex(i,j))->is_frozen = 0;
            else
                (getCellIndex(i,j))->is_frozen = 1;
            
        }
    }
    
    
}

unsigned int CellMatrix::getWidth () {return width;}
unsigned int CellMatrix::getHeight () {return height;}


Cell * CellMatrix::getCellIndex(unsigned int x, unsigned int y){
    if (x >= 0 && x < width && y >= 0 && y < height){
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
