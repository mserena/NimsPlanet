//
//  TTBoardStepKillCells.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStepKillCells.h"
#include "TTAudioManager.h"

TTBoardStepKillCells::TTBoardStepKillCells(std::vector<TTCell*> cellsToKill, bool completeKill,std::vector<TTCell*> noSelectableCells,std::map<int,int> cellInstances):
TTBoardStep::TTBoardStep(),
_cellsToKill(cellsToKill),
_completeKill(completeKill),
_noSelectableCells(noSelectableCells),
_cellInstances(cellInstances),
_numCellsToKill(cellsToKill.size())
{
    _classListeners.push_back(registerListenerEvent(EVENT_BOARD_CELL_DEATH,CC_CALLBACK_1(TTBoardStepKillCells::onEvent_cellDeath,this)));
}

TTBoardStepKillCells::~TTBoardStepKillCells()
{
    //Desregistrar listeners de eventos
    removeSafeListenerEvent(_classListeners);
    
    _cellsToKill.clear();
}

void TTBoardStepKillCells::starStep()
{
/*    TTAudioManager::getInstance()->playAudio("DEATH_ANIMALS");
    
    for(int i = 0; i < _noSelectableCells.size(); i++)
    {
        int _instances = _cellInstances.find(_noSelectableCells[i]->getCellIdCode())->second;
        _noSelectableCells[i]->alertComboAffects(_instances);
    }
    
    for(int i = 0; i < _cellsToKill.size(); i++)
    {
        _cellsToKill[i]->onCellSelectConfirmation(_completeKill);
        _cellsToKill[i]->clearSelectedCell();
    }
    //si np habia celdas que eliminar, lanzar el step finished
    if (_cellsToKill.size()==0)
    {
        stepFinished();
    }*/
}

void TTBoardStepKillCells::stepFinished()
{
    TTBoardStep::stepFinished();
}

void TTBoardStepKillCells::onEvent_cellDeath(EventCustom* e)
{
    _numCellsToKill--;
    if(_numCellsToKill == 0)
        stepFinished();
}