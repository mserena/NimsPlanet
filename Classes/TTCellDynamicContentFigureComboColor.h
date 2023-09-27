#ifndef __TTCellDynamicContentFigureComboColor_SCENE_H__
#define __TTCellDynamicContentFigureComboColor_SCENE_H__

#include "cocos2d.h"
#include "TTCellDynamicContent.h"

using namespace cocos2d;

class TTCellDynamicContentFigureComboColor: public TTCellDynamicContent
{

public:

    // Figure constructor
	TTCellDynamicContentFigureComboColor(cocos2d::Node* cellContentParent,int _zLayerPosition, int posXCell, int posYCell,int column, int row, int totalColumns, int totalRows, int sizeCell);
    ~TTCellDynamicContentFigureComboColor();
    
    //Acciones a la celda
    void loadGeneralCellGraphs();
    void initCell();
    void selectedCell();
    void unselectedCell();
    void launchComboFx(std::vector<Point> _positionsToKill, int _cellFigure);
    int hitCell(int _numHits = 1);
    
    // Consultas a la celda
    bool isRefillable();
    bool isSelectable();
    bool isFirstSelectable();
    bool isWallContent();
    
    void preSelectCell();
    void unPreSelectCell();
    void showPathCell(CELL_PATHS pathDirection);
    void hidePathCell();
    void prepareToAbduction(Point _abductionPoint);

    void playSelectSound(int _numSelection);
    void playUnSelectSound(int _numSelection);
    
    void launchContentAnimation();
    void stopCellAnimations();
    void finishFallCell();
    
    void showHelpCell();
    void stopHelpCell();

private:
    
    void loadAnimations();
    void launchComboStandByAnimation();
    void launchComboKamehameha(int _cellFigure, Point _destination, float _time, float _speed = 0);
    
private:
    
    float                     _scaleCell;
    Size                      _cellReferenceSize;
    
    std::vector<Animation*>   _cellAnimationsStandBy;
    Animation*                _cellAnimationsScary;
    Animation*                _cellAnimationsJump;
    Animation*                _cellAnimationsRebound;
    
};

#endif  // __TTCellDynamicContentFigureComboColor_SCENE_H__
