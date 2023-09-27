//
//  TTBoardBoosterComboColor.cpp
//  animalcubes
//
//  Created by Marc Serena on 10/10/14.
//
//

#include "TTBoardBoosterComboColor.h"

#define NUM_COMBOS_COLOR 1

TTBoardBoosterComboColor::TTBoardBoosterComboColor():
TTBoardBoosters(BOARD_BOOSTER_COMBO_COLOR,"booster_ui_combo_color.png")
{
    
}

TTBoardBoosterComboColor::~TTBoardBoosterComboColor()
{
    
}

void TTBoardBoosterComboColor::aplyBoardBooster(std::vector<TTCell*> _cellsBoard)
{
    TTBoardBoosters::aplyBoardBooster();
    
    std::vector<TTCell*> _posibleCells = _cellsBoard;
    std::random_shuffle(_posibleCells.begin(),_posibleCells.end());
    for(int i = 0; i < NUM_COMBOS_COLOR; i++)
    {
        _posibleCells[i]->convertCellToComboColor();
    }
}