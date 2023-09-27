#ifndef __TTCellDynamicContentObjectDown_SCENE_H__
#define __TTCellDynamicContentObjectDown_SCENE_H__

#include "cocos2d.h"
#include "TTCellDynamicContent.h"

using namespace cocos2d;

//Definicion de los objetos para bajar
#define DOWN_OBJECT_SPRITE "paja.png"

class TTCellDynamicContentObjectDown: public TTCellDynamicContent
{

public:
	TTCellDynamicContentObjectDown(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellDynamicType, int posXCell, int posYCell,int column, int row, int totalColumns, int totalRows,int sizeCell);
    ~TTCellDynamicContentObjectDown();
    
    //Acciones a la celda
    void initCell();
    int hitCell(int _numHits = 1);
    
    // Consultas a la celda
    bool isRefillable();
    bool isSelectable();
    bool isFirstSelectable();
    bool isWallContent();
    
    void preSelectCell(){};
    void unPreSelectCell(){};
    
    float moveDownObjectOutOfBoard(int _posYDestination,int _rowDestination,int _rowInit, int _totalRows);
    void finishMoveDownObjectOutOfBoard(Sprite* _tmpDownObject);
    
    void moveCellAlert(int moves);
};

#endif  // __TTCellDynamicContentObjectDown_SCENE_H__
