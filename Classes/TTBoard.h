#ifndef __TTBoard_SCENE_H__
#define __TTBoard_SCENE_H__

#include "cocos2d.h"
#include "TTEventsManager.h"
#include "TTCell.h"
#include "TTGoal.h"
#include "TTBoardStepsManager.h"
#include "TTLanguageManager.h"


using namespace cocos2d;

#define HELP_ACTION_COMBINATION_TAG 130

#define MIN_SELECT_CELLS                            3
#define MIN_SELECT_CELLS_COMBO                      2
#define MIN_SELECT_CELLS_TO_CREATE_COMBO            4
#define MIN_SELECT_CELLS_TO_CREATE_COMBO_COLOR      8

#define SHOW_LABEL_MIX_TIME                         1.0f

typedef struct initBoardInfo
{
    std::vector< std::vector<std::string> > boardMap;
    int                             maxCellsBoard;
    std::string                     background;
    std::vector<int>                figuresWeight;
}initBoardInfo;

typedef struct finishPlayInfo
{
    int combinationLength;
    int combosInCombination;
    int combosColorInCombination;
    int selectionLength;
    TTCell* lastSelectedCell;
    
    int boosterPlay=-1;
    
    std::map<int,int> combinationTypes;
    
    bool isOvni = false;
}finishPlayInfo;

class CC_DLL TTBoard: public Ref
{
public:
    
    // Retorna la instancia Singleton de la clase
    static TTBoard* getInstance(void);
    
    // Destruye la instancia Singleton
    static void destroyInstance();
    
protected:
    
    TTBoard(){};

public:

	void initBoardInstance(cocos2d::Layer* parentLayer);
    ~TTBoard();

    void clearBoard();
    bool initBoard(initBoardInfo boardInfo, std::vector<TTGoal*> goals);
    void showBoard();
    void finishBoardControl(int _restrictionBonus);
    void finishBoard();
    void update(float dt);
    bool selectBoardCells(float x, float y);
    void finishSelectBoardCells();
    void finishActionsBoard();
    void verifyCellsPropagation();
    
    void stopShowHelpCombination();
    bool launchShowHelpCombination();
    
    void updateBoardState();
    void consolidateBoard();
    void unlockBoard();
    
    void setBackgroundBoard(Texture2D* texture);
    
    std::vector<TTCell*> getAllNormalCells();
    
    std::vector<TTCell*> selectCellsFromColumn(TTCell* cell);
    std::vector<TTCell*> selectCellsFromRow(TTCell* cell);
    std::vector<TTCell*> selectCellsFromDiagonal(TTCell* cell);
    std::vector<TTCell*> selectCellsSameType(TTCell* cell);
    std::vector<TTCell*> completeCellSelection(std::vector<TTCell*> _cellsToComplete);
    
    void applyBooster();
    
    void showScoreOnBoard();
    
private:
    
    void generateNewCells();
    int getLastCellOnBoardColumn(int column);
    void resetPlayCounters();
    void showResultHelp();
    void clearResultHelp();
    bool isComboSelection();
    
    CONTENT_TYPES getNewComboType(std::vector<TTCell*> _cells);
    CELL_PATHS calcPathDirection(Vec2 _currCellId, Vec2 _lastCellId);
    
    std::vector<TTCell*> getAreaCombinationAffectCells();
    void getActionAreaCells(TTCell* cell, std::vector<TTCell*> *collectVector);
    
    void stepFillHolesInBoard();
    bool searchForCombination(bool onlySearchCombos = false);
    bool searchForcombinationComboColor();
    
    bool sameCellsInSquare(TTCell* initCell,std::vector<TTCell*> *combination,bool onlySearchCombos = false);
    bool samePositionsInSquare(std::vector<int> cell, std::vector<std::vector<int> > mapPositions, std::vector<std::vector<int> > *positions,int minCombinationSearch);
    bool mixBoardCellsForCombination(bool forceCombosCombination = false);
    void mixBoardCellsRandom(std::vector<std::vector<int> > posiblePositions,std::vector<std::vector<int> > censoredPositions);
    void launchMixBoard();
    void finishMixBoard();
    
    // Sound functions
    void playSoundFX();
    
    // Animation functions
    void launchAnimStandBy();
    void showHelpCombination(bool show);
    
    //Eventos
    void onEvent_boardWoodBreak(EventCustom* e);
    void onEvent_boardCellPositioned(EventCustom* e);
    void onEvent_boardStepsFinished(EventCustom* e);
    
public:
    
    std::vector<std::vector<TTCell*>>   _Cells;             //vector de celdas
    cocos2d::Node*          _parentBoard;
    float                   _cellSize;
    
    bool                    _isUpdatingBoard;
    bool                    _isInBoardMix;
    bool                    _isBoardActive;
    
    finishPlayInfo          _finishInfo;
    
    // Gestión de agujeros en el tablero
    std::vector<Vec2>       _cellHoles;
    std::vector<Vec2>       _cellExcludeHoles;

    // Selección de boosters en tablero
    Node*                   _parentHelpBooster;
    Sprite*                 _arrowBooster;
    Sprite*                 _helpFrameBooster;
    
    TTBoardStepsManager*    _stepsManager;
    
    std::vector<CONTENT_TYPES> _lastCellsTypeHited;
    
private:
    
    cocos2d::Layer*         _parentLayer;
    Sprite*                 _boardBg;
    
    int                     _cellsToReposition;
    bool                    _needToUpdateBoard;
    
    std::vector<TTCell*>    _selectedCells;     //vector de celdas seleccionadas en un movimiento
    std::vector<TTCell*>    _helpCombination;   //vector de celdas con la combinación de ayuda
    std::vector<TTCell*>    _combosShowedHelp;  //vector de combos de los que ya hemos mostrado la ayuda
    
    std::vector<TTGoal* >   _boardGoals;                      //vector con los objectivos del tablero
    
    std::vector<EventListener*> _classListeners;

};

#endif  // __TTBoard_SCENE_H__
