/*
 * CONTENIDO DINAMICO DE LA CELDA - CLASE MADRE
 * Esta clase representa el contenido dinámico de la celda, todo aquel que se mueve por el funcionamiento normal del juego,
 * por ejemplo, las figuras, los combos, los objectos bajables... (Todos ellos cambian de casillas durante el juego)
 *
 */

#ifndef __TTCellDynamicContent_SCENE_H__
#define __TTCellDynamicContent_SCENE_H__

#include "cocos2d.h"
#include "TTCellDefines.h"
#include "TTCellAnimations.h"
#include "thegame.h"
#include "TTCellContent.h"

using namespace cocos2d;

// Estados dynamicos de la celda
typedef enum
{
    DYNAMIC_CONTENT_STATE_NOTHING,
    DYNAMIC_CONTENT_STATE_STANDBY,
    DYNAMIC_CONTENT_STATE_SELECTED,
    DYNAMIC_CONTENT_STATE_FALLING,
    DYNAMIC_CONTENT_STATE_HELP,
    DYNAMIC_CONTENT_STATE_DEATH,
}DYNAMIC_CONTENT_STATES;

class TTCellDynamicContent : public TTCellContent
{

public:

	TTCellDynamicContent(cocos2d::Node* parentContentsCell,int _zLayerPosition,CONTENT_TYPES cellDynamicType, int posXCell, int posYCell,int column, int row, int totalColumns, int totalRows,int sizeCell);
    virtual ~TTCellDynamicContent();
    
    //Acciones a la celda
    virtual void initCell();
    virtual void selectedCell();
    virtual void unselectedCell();
    virtual int hitCell(int _numHits = 1){};
    virtual void showHelpCell(){};
    virtual void stopHelpCell(){};
    
    //Consultas a la celda
    virtual bool isRefillable() = 0;
    virtual bool isSelectable() = 0;
    virtual bool isFirstSelectable() = 0;
    virtual bool isWallContent() = 0;
    virtual bool isMovableContent();
    bool isStrongWallContent();
    int getZContent();

    bool isEmptyCell();
    bool isCombo();
    
    // Seleccion
    virtual void showPathCell(CELL_PATHS pathDirection){};
    virtual void hidePathCell(){};
    virtual void preSelectCell() = 0;
    virtual void unPreSelectCell() = 0;
    virtual void prepareToAbduction(Point _abductionPoint){};
    virtual void playSelectSound(int _numSelection){};
    virtual void playUnSelectSound(int _numSelection){};
    
    // Animaciones
    virtual void launchContentAnimation(){};
    virtual void stopCellAnimations();
    virtual void finishFallCell();
    
public:
    
    // Variables de estado
    DYNAMIC_CONTENT_STATES  _cellDynamicState;
    
    // Variables para la generación de contenido dinámico
    int                      _cellFigure;
    
    // Variables
    int         _posXCell;
    int         _posYCell;
    int         _sizeCell;
    int         _sizeFigure;
    
    // Sprites
    Sprite*     _cellFigureSprite;
    
    // Animaciones
    bool        _isAnimable;
    bool        _isOnAnimation;
    bool        _blockAnimations;
    
    // Seleccion
    Sprite*              _pathSprite;
    
    // Ubicación de la celda
    int _column;
    int _row;
    int _totalColumns;
    int _totalRows;
    
};

#endif  // __TTCellDynamicContent_SCENE_H__
