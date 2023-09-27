//
//  TTBoardBoosterCombos.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTBoardBoosterCombos.h"

#define NUM_COMBOS_BOOSTER 3

TTBoardBoosterCombos::TTBoardBoosterCombos():
TTBoardBoosters(BOARD_BOOSTER_COMBOS,"booster_ui_combos.png")
{
    
}

TTBoardBoosterCombos::~TTBoardBoosterCombos()
{
    
}

void TTBoardBoosterCombos::aplyBoardBooster(std::vector<TTCell*> _cellsBoard)
{
    TTBoardBoosters::aplyBoardBooster();
    
    std::vector<TTCell*> _posibleCells = _cellsBoard;
    std::random_shuffle(_posibleCells.begin(),_posibleCells.end());
    for(int i = 0; i < NUM_COMBOS_BOOSTER; i++)
    {
        int _typeCombo = COMBO_VERTICAL_DYNAMIC_CONTENT+rand()%(COMBO_AREA_DYNAMIC_CONTENT-COMBO_VERTICAL_DYNAMIC_CONTENT);
        _posibleCells[i]->convertCellToCombo(CONTENT_TYPES(_typeCombo));
    }
}