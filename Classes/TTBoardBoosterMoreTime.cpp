//
//  TTBoardBoosterMoreTime.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTBoardBoosterMoreTime.h"

#define NUM_COMBOS_COLOR 1

TTBoardBoosterMoreTime::TTBoardBoosterMoreTime():
TTBoardBoosters(BOARD_BOOSTER_MORE_TIME_15,"booster_ui_tiempo.png")
{
    
}

TTBoardBoosterMoreTime::~TTBoardBoosterMoreTime()
{
    
}

void TTBoardBoosterMoreTime::aplyBoardBooster(gameInfo* _gameConfig)
{
    TTBoardBoosters::aplyBoardBooster();

    _gameConfig->timeRestriction->addExtraTime(15);
}