//
//  TTBoardStepHitCells.cpp
//  animalcubes
//
//  Created by Marc Serena on 06/10/14.
//
//

#include "TTBoardStepHitCells.h"
#include "TTAudioManager.h"

TTBoardStepHitCells::TTBoardStepHitCells(std::map<TTCell*,int> cellsHitsMap):
TTBoardStep::TTBoardStep(),
_cellsHitsMap(cellsHitsMap),
_isRunningStep(false)
{

}

TTBoardStepHitCells::~TTBoardStepHitCells()
{

}

void TTBoardStepHitCells::starStep()
{
    _isRunningStep = true;
    
    bool _hitWood = false;
    bool _hitMud = false;
    bool _hitStone = false;
    
    std::map<TTCell*, int>::const_iterator itrHitCells;
    for(itrHitCells = _cellsHitsMap.begin(); itrHitCells != _cellsHitsMap.end(); ++itrHitCells)
    {
        CCLOG("Aplicamos %d hits a la celda %d,%d",(*itrHitCells).second,(*itrHitCells).first->_column,(*itrHitCells).first->_row);
        (*itrHitCells).first->hitCell((*itrHitCells).second);
        
        if((*itrHitCells).first->_cellContentLayers.size() > 0 && typeid(*(*itrHitCells).first->_cellContentLayers.back()) == typeid(TTCellStaticContentStone))
        {
            _hitStone = true;
        }
        else if((*itrHitCells).first->_cellContentLayers.size() > 1 && typeid(*(*itrHitCells).first->_cellContentLayers[(*itrHitCells).first->_cellContentLayers.size()-2]) == typeid(TTCellStaticContentMud))
        {
            _hitMud = true;
        }
        else if((*itrHitCells).first->_cellContentLayers.size() > 0 && typeid(*(*itrHitCells).first->_cellContentLayers.back()) == typeid(TTCellStaticContentWood))
        {
            _hitWood = true;
        }
    }
    
    //Play de sonidos
    if(_hitStone) TTAudioManager::getInstance()->playAudio("STONE_BREAK");
    if(_hitMud) TTAudioManager::getInstance()->playAudio("MUD_BREAK");
    if(_hitWood) TTAudioManager::getInstance()->playAudio("WOOD_BREAK");
    
    TTAudioManager::getInstance()->playAudio("DEATH_ANIMALS");
    
    stepFinished();
}

void TTBoardStepHitCells::stepFinished()
{
    _isRunningStep = false;
    TTBoardStep::stepFinished();
}
