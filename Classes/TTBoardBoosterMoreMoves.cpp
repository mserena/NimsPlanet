//
//  TTBoardBoosterMoreMoves.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTBoardBoosterMoreMoves.h"

#define NUM_COMBOS_COLOR 1

TTBoardBoosterMoreMoves::TTBoardBoosterMoreMoves():
TTBoardBoosters(BOARD_BOOSTER_MORE_MOVEMENTS_5,"booster_ui_movimientos.png")
{
    
}

TTBoardBoosterMoreMoves::~TTBoardBoosterMoreMoves()
{
    
}

void TTBoardBoosterMoreMoves::aplyBoardBooster(gameInfo* _gameConfig)
{
    TTBoardBoosters::aplyBoardBooster();

    _gameConfig->moveRestriction->addExtraMoves(5);
}