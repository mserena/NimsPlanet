/*
 * CONTENIDO ESTATICO DE LA CELDA - CLASE MADRE
 * Esta clase representa el contenido estático de la celda, todo aquel que permanece estático en el juego,
 * por ejemplo, las casillas, el barro...
 */

#ifndef __TTCellStaticContent_SCENE_H__
#define __TTCellStaticContent_SCENE_H__

#include "cocos2d.h"
#include "TTCellDefines.h"
#include "TTCellAnimations.h"
#include "TTCellContent.h"

using namespace cocos2d;

// Estados dynamicos de la celda
typedef enum
{
    STATIC_CONTENT_STATE_NOTHING,
}STATIC_CONTENT_STATES;


class TTCellStaticContent : public TTCellContent
{

public:

	TTCellStaticContent(cocos2d::Node* parentBoard,int _zLayerPosition,CONTENT_TYPES cellStaticType, int posXCell, int posYCell,int sizeCell,Vec2 ownerCellId);
    virtual ~TTCellStaticContent();

    //Acciones a la celda
    virtual void initCell();
    virtual void selectedCell(){};
    virtual void unselectedCell(){};
    virtual int hitCell(int _numHits = 1){};
    
    //Consultas a la celda
    virtual bool isRefillable() = 0;
    virtual bool isSelectable() = 0;
    virtual bool isFirstSelectable() = 0;
    virtual bool isWallContent() = 0;
    virtual bool isStrongWallContent() = 0;
    bool isMovableContent();
    int getZContent();
    
    bool isEmptyCell();
    virtual void preSelectCell(){};
    virtual void unPreSelectCell(){};
    
public:
    
    // Variables de estado
    CONTENT_TYPES   _cellStaticType;
    STATIC_CONTENT_STATES  _cellStaticState;
    
    // Variables
    int         _posXCell;
    int         _posYCell;
    int         _sizeCell;
    bool        _emptyContent;
    
    // Referencia a las celdas
    Vec2    _ownerCellId;
    
};

#endif  // __TTCellStaticContent_SCENE_H__
