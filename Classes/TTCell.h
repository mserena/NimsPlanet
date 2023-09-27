#ifndef __TTCell_SCENE_H__
#define __TTCell_SCENE_H__

#include "cocos2d.h"
#include "TTCellAnimations.h"
#include "TTCellDefines.h"

// Contenido de la celda
#include "TTCellContent.h"

// Dynamic content
#include "TTCellDynamicContent.h"
#include "TTCellDynamicContentFigure.h"
#include "TTCellDynamicContentFigureComboColor.h"
#include "TTCellDynamicContentObjectDown.h"

// Static content
#include "TTCellStaticContent.h"
#include "TTCellStaticContentMud.h"
#include "TTCellStaticContentWood.h"
#include "TTCellStaticContentStone.h"
#include "TTCellStaticContentNetwork.h"
#include "TTCellStaticContentGlue.h"

using namespace cocos2d;

typedef enum
{
    CELL_EMPTY,                     // Celda no usada en el nivel
    CELL_WITH_NO_CONTENT,           // Celda usada en el nivel sin contenido
    CELL_WITH_CONTENT,              // Celda usada en el nivel con contenido
    CELL_WITH_EXTERNAL_RESOURCE,    // Celda no usada en el nivel con contenido externo (imagen,animación...)
}CELL_TYPE_ID;

class TTCell
{

public:

    TTCell(cocos2d::Layer* parentLayer,cocos2d::Node* parentBoard, std::string cellType, int posXCell, int posYCell,int sizeCell, int column, int row, int totalColumns, int totalRows);
    ~TTCell();
    
    void loadCellGraph();
    
    void initCell();
    bool isCellTouched(float x, float y, std::vector<CONTENT_TYPES> _cellsAllowed={});
    
    bool isUpdatingCell();
    
    bool isSelectableCell();
    bool isFirstSelectableCell();
    bool isComboCell();
    bool isComboColorCell();
    bool isDownObjectCell();
    bool isEmptyCell();                     // Es una celda vacía que no pertenece al mapa?
    bool isRefillableCell();                // Es una celda que puede ser rellenada con contenido?
    bool isAffectedFromAreaCombination();
    
    std::vector<CONTENT_TYPES> getCellContentTypes();        // Regresa los tipos de contenido de las celdas
    
    cocos2d::Vec2 getCellId();
    int getCellIdCode();
    
    void prepareToAbduction(Point _abductionPoint);
    
    float moveDownObjectOutOfBoard(TTCell* _cellDestination);
    void moveCell(TTCell* _cellDestination);
    void generateNewCell(CONTENT_TYPES newCellType, int _numNewCells, int idNewCell, std::vector<int> _emptyCellsMap);
    void convertCellToComboColor();
    void convertCellToCombo(CONTENT_TYPES _newComboType);
    void switchCell(TTCell* _cellDestination);
    
    void hitCell(int _numHits = 1);
    void killCell();
    void consolidateCell();
    
    bool hasMovableContent();
    
    int getCellFigure();
    
    //delegate methods
    void preSelectCell();
    void unPreSelectCell();
    void selectCell();
    void unselectCell();
    void showPathCell(CELL_PATHS pathDirection);
    void hidePathCell();
    void playSelectionSound(int _numSelection);
    void playUnSelectionSound(int _numSelection);
    
    //animaciones
    void launchCellAnimationStandBy();
    
    //devuelve si la celda puede ser atravesada o no
    bool isWallCell();
    bool isStrongWallCell();
    
public:
    
    cocos2d::Layer*   _parentLayer;
    cocos2d::Node*    _parentBoard;

    int         _posXCell;
    int         _posYCell;
    int         _sizeCell;
    int         _sizeFigure;
    
    int _column;
    int _row;
    int _totalRows;
    int _totalColumns;
    
    // Sprites
    Sprite*                     _cellBackground;
    Sprite*                     _cellSelectedMark;
    Sprite*                     _cellPreSelectedMark;
    Sprite*                     _cellTutorialGlaze;
    
    // Content Variables
    std::vector<TTCellContent*> _cellContentLayers;
    CELL_TYPE_ID                _cellTypeID;
    
};

#endif  // __TTBoard_SCENE_H__
