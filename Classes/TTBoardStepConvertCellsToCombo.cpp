//
//  TTBoardStepConvertCellsToCombo.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStepConvertCellsToCombo.h"

TTBoardStepConvertCellsToCombo::TTBoardStepConvertCellsToCombo(std::vector<TTCell*> cellsToConvert, CONTENT_TYPES comboType):
TTBoardStep::TTBoardStep(),
_cellsToConvert(cellsToConvert),
_comboType(comboType)
{

}

TTBoardStepConvertCellsToCombo::~TTBoardStepConvertCellsToCombo()
{
    
}

void TTBoardStepConvertCellsToCombo::starStep()
{
    for(int i = 0; i < _cellsToConvert.size(); i++)
    {
        if(_cellsToConvert[i]->isWallCell()) continue;
        (static_cast<TTCellDynamicContentFigure*>(_cellsToConvert[i]->_cellContentLayers.back()))->convertCellToCombo(_comboType);
    }
    
    stepFinished();
}

void TTBoardStepConvertCellsToCombo::stepFinished()
{
    TTBoardStep::stepFinished();
}