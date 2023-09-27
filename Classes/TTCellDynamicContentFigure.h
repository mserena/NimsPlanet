#ifndef __TTCellDynamicContentFigure_SCENE_H__
#define __TTCellDynamicContentFigure_SCENE_H__

#include "cocos2d.h"
#include "TTCellDynamicContent.h"

using namespace cocos2d;

typedef enum
{
    ARROW_PATH_O,
    ARROW_PATH_NO,
    ARROW_PATH_N,
    ARROW_PATH_NE,
    ARROW_PATH_E,
    ARROW_PATH_SE,
    ARROW_PATH_S,
    ARROW_PATH_SO
}ARROW_DIRECTIONS;

// Informacion sobre la figura
typedef struct figureInfo
{
    std::string _figureNameFile;
    bool        _isSmallFigure;
    
    figureInfo(std::string figureNameFile,bool isSmallFigure)
    {
        _figureNameFile = figureNameFile;
        _isSmallFigure = isSmallFigure;
    }
}figureInfo;

//Definicion de los sprites de las figuras
const std::map<CELL_FIGURES, figureInfo> _figureSpritesMap{ {FIGURE_1, figureInfo("figura1.png",false)},
                                                            {FIGURE_2, figureInfo("figura2.png",true)},
                                                            {FIGURE_3, figureInfo("figura3.png",true)},
                                                            {FIGURE_4, figureInfo("figura4.png",false)},
                                                            {FIGURE_5, figureInfo("figura5.png",false)},
                                                            {FIGURE_6, figureInfo("figura6.png",false)}};

//Definicion de los sprites para los combos
const std::map<CELL_FIGURES, figureInfo> _figureComboSpritesMap{{FIGURE_1, figureInfo("figura1_bomba.png",false)},
                                                                {FIGURE_2, figureInfo("figura2_bomba.png",true)},
                                                                {FIGURE_3, figureInfo("figura3_bomba.png",true)},
                                                                {FIGURE_4, figureInfo("figura4_bomba.png",false)},
                                                                {FIGURE_5, figureInfo("figura5_bomba.png",false)},
                                                                {FIGURE_6, figureInfo("figura6_bomba.png",false)}};

class TTCellDynamicContentFigure: public TTCellDynamicContent
{

public:

    // Figure constructor
	TTCellDynamicContentFigure(cocos2d::Node* cellContentParent,int _zLayerPosition,CONTENT_TYPES cellDynamicType, int posXCell, int posYCell,int column, int row, int totalColumns, int totalRows, int sizeCell, int cellInitSubType);
    ~TTCellDynamicContentFigure();
    
    //Acciones a la celda
    void initCell();
    void loadGeneralCellGraphs();
    void loadComboArrows();
    void selectedCell();
    void unselectedCell();
    void launchComboFX();
    int hitCell(int _numHits = 1);
    
    //Consultas a la celda
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
    
    void convertCellToNormalFigure(int cellFigure);
    void convertCellToCombo(CONTENT_TYPES comboType);
    
    void launchContentAnimation();
    void stopCellAnimations();
    void finishFallCell();
    
    void showHelpCell();
    void stopHelpCell();

private:
    
    void loadAnimations();
    
    // combos
    void launchComboStandByAnimation();
    void getArrowPositions(ARROW_DIRECTIONS _arrowDirection, float _offset, Point* _initPoint, Point* _finishPoint);
    void launchComboKamehameha(ARROW_DIRECTIONS _kamehamehaDirection, float _offset, float _time, float _speed = 0);

public:

    float                     _scaleCell;

private:
    
    std::vector<Animation*>   _cellAnimationsStandBy;
    Animation*                _cellAnimationsScary;
    Animation*                _cellAnimationsJump;
    Animation*                _cellAnimationsRebound;
    
    // Variables para los combos
    std::vector<Sprite*>      _comboArrows;
    
};

#endif  // __TTCellDynamicContentFigure_SCENE_H__
