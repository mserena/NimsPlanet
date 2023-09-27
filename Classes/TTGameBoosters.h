//
//  TTGameBoosters.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTGameBoosters__
#define __animalcubes__TTGameBoosters__

#include "cocos2d.h"
#include "TTBoosterDefines.h"
#include "TTBoard.h"
#include "TTEventsManager.h"
#include "TTBooster.h"
#include "TTAudioManager.h"

using namespace cocos2d;

class TTGameBoosters: public TTBooster
{
    
public:
    
    TTGameBoosters(GAME_BOOSTERS_TYPE subtypeBooster, std::string boosterIcon, std::vector<CONTENT_TYPES> cellsAllowed = {});
    ~TTGameBoosters();
    
    //identificador interno de booster
    int getBoosterId();
    
    virtual void showApplyPosition();
    virtual void moveApplyPosition(Point _touchPoint);
    virtual void aplyGameBooster();
    virtual void resetBooster();
    
    //devuelve si el booster puede ser o no aplicado en el actual game
    virtual bool canBeApplied(gameInfo* _gameinfo);

protected:
    
    bool isCellAllowed(TTCell* _cell);
    
public:
    
    GAME_BOOSTERS_TYPE      _boosterSubType;
    int                     _boosterPrize;
    bool                    _boosterActivated;
    std::string             _boosterIcon;
    
    bool                    _activeSelection;
    TTCell*                 _selectedCell;
    
    std::vector<CONTENT_TYPES>  _cellsAllowed;
    
    bool                    _onlySelectableCells;
    
};

#endif /* defined(__animalcubes__TTGameBoosters__) */
