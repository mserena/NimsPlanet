//
//  TTBoardBoosters.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTBoardBoosters.h"
#include "TTUserInfo.h"
#include "TTStoreManager.h"

TTBoardBoosters::TTBoardBoosters(BOARD_BOOSTERS_TYPE subtypeBooster, std::string boosterIcon):
TTBooster(BOARD_BOOSTERS),
_boosterSubType(subtypeBooster),
_boosterIcon(boosterIcon),
_boosterActivated(false)
{
    
}

TTBoardBoosters::~TTBoardBoosters()
{
    
}

//identificador interno de booster
int TTBoardBoosters::getBoosterId()
{
    return _boosterType*100+_boosterSubType;
}

bool TTBoardBoosters::canBeApplied(gameInfo* _gameinfo)
{
    //solo se puede aplicar si está dentro de la definicion del gameinfo
    return _gameinfo->isBoosterDefined(_boosterType, _boosterSubType);
}

void TTBoardBoosters::aplyBoardBooster()
{
    // Restamos el booster usado
    _boosterActivated = false;
    
    std::string _boosterItemId = std::string(std::string(ITEM_BOOSTER)+"0_"+std::to_string(_boosterSubType));
    TTStoreManager::getInstance()->takeUserVirtualItem(_boosterItemId);
}