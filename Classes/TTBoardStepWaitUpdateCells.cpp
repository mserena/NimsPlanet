//
//  TTBoardStepWaitUpdateCells.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStepWaitUpdateCells.h"
#include "TTAudioManager.h"
#include "TTBoard.h"

TTBoardStepWaitUpdateCells::TTBoardStepWaitUpdateCells():
TTBoardStep::TTBoardStep(),
_isRunningStep(false)
{

}

TTBoardStepWaitUpdateCells::~TTBoardStepWaitUpdateCells()
{

}

void TTBoardStepWaitUpdateCells::updateStep(float dt)
{
    if(!_isRunningStep)
        return;
    
    // Verificamos si hay alguna celda updatedando
    bool _allCellsUpdated = true;
    std::vector<std::vector<TTCell*> > _boardCells = TTBoard::getInstance()->_Cells;
    for(int i = 0; i < _boardCells.size(); i++)
    {
        for(int j = 0; j < _boardCells[i].size(); j++)
        {
            if(_boardCells[i][j]->isUpdatingCell())
            {
                _allCellsUpdated = false;
                break;
            }
        }
    }
    
    if(_allCellsUpdated)
    {
        stepFinished();
    }
}

void TTBoardStepWaitUpdateCells::starStep()
{
    _isRunningStep = true;
}

void TTBoardStepWaitUpdateCells::stepFinished()
{
    _isRunningStep = false;
    TTBoardStep::stepFinished();
}
