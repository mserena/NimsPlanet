//
//  TTBoardBoosters.h
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#ifndef __animalcubes__TTBoardBoosters__
#define __animalcubes__TTBoardBoosters__

#include "cocos2d.h"
#include "TTBoosterDefines.h"
#include "TTBooster.h"

using namespace cocos2d;

class TTBoardBoosters: public TTBooster
{
    
public:
    
    TTBoardBoosters(BOARD_BOOSTERS_TYPE subtypeBooster, std::string boosterIcon);
    ~TTBoardBoosters();
    
    //identificador interno de booster
    int getBoosterId();
    
    virtual void aplyBoardBooster();

    //devuelve si el booster puede ser o no aplicado en el actual game
    virtual bool canBeApplied(gameInfo* _gameinfo);
    
public:
    
    BOARD_BOOSTERS_TYPE     _boosterSubType;
    int                     _boosterPrize;
    bool                    _boosterActivated;
    std::string             _boosterIcon;
};


#endif /* defined(__animalcubes__TTBoardBoosters__) */
