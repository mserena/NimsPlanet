//
//  TTBooster.h
//  animalcubes
//
//  Created by Marc Serena on 29/10/14.
//
//

#ifndef __animalcubes__TTBooster__
#define __animalcubes__TTBooster__

#include "TTBoosterDefines.h"
#include "TTGameInfo.h"

class TTBooster
{
    
public:
    
    TTBooster(BOOSTERS_TYPES boosterType);
    ~TTBooster();
    
    //identificador interno de booster
    virtual int getBoosterId() ;
    
    //deveulve si el booster puede ser o no aplicado en el actual game
    virtual bool canBeApplied(gameInfo* _gameinfo);
    
public:
    
    BOOSTERS_TYPES     _boosterType;
};


#endif /* defined(__animalcubes__TTBooster__) */
