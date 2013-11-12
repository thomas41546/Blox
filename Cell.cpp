//
//  Cell.cpp
//  
//
//  Created by Thomas Polasek on 11/11/13.
//
//

#include "Cell.hpp"


Cell::Cell(){
    cellMatrix = NULL;
    is_breakable = true;
    is_filled = false;
    is_edge = false;
    slope = NONE;
}
void Cell::setCellMatrix(CellMatrix * _cellMatrix){
    cellMatrix = _cellMatrix;
}

bool Cell::destroy(){
    if(!(is_filled && is_breakable))
        return false;
    
    is_filled = false;
    return true;
}


void Cell::setEdge(){
    is_edge = true;
}

void Cell::resetEdge(){
    is_edge = false;
}
bool Cell::isEdge(){
    return is_edge;
}


void Cell::setFilled(){
    is_filled = true;
}

void Cell::resetFilled(){
    is_filled = false;
}

void Cell::setSlope(SlopeType _slope){
    slope = _slope;
}

void Cell::resetSlope(){
    slope = NONE;
}


bool Cell::isFilled(){
    return is_filled;
}


void Cell::setBaseColor(SDL_Color _baseColor){
    base_color = _baseColor;
}

void Cell::render(SDL_Rect & rect){
    
    if(is_filled){
        Drawing::drawRect(rect,base_color);
    }
    else if( slope == LEFT){
        Drawing::drawRightTriangleLeft(rect,base_color);
    }
    else if( slope == RIGHT){
        Drawing::drawRightTriangleRight(rect,base_color);
    }
    
    if(is_edge){
        Drawing::drawRect(rect,COLOR_RED);
    }
    
    
}