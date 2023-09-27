//
//  TTBooster.cpp
//  animalcubes
//
//  Created by Marc Serena on 29/10/14.
//
//

#include "TTBooster.h"

TTBooster::TTBooster(BOOSTERS_TYPES typeBooster):
_boosterType(typeBooster)
{
    
}

TTBooster::~TTBooster()
{
    
}

//identificador interno de booster
int TTBooster::getBoosterId()
{
    return -1;
}

bool TTBooster::canBeApplied(gameInfo* _gameinfo)
{
    //por defecto, siempre true
    return true;
}